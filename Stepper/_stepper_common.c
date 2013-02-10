/*
 * $Id: _stepper_common.c,v 1.4 2010/09/14 15:04:55 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _stepper_common.c,v $
 * Revision 1.4  2010/09/14 15:04:55  clivewebster
 * Fix bugs in acceleration logic
 *
 * Revision 1.3  2010/08/14 21:03:13  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2010/08/11 17:19:52  clivewebster
 * Version 1.24
 *
 * Revision 1.1  2010/08/10 22:51:58  clivewebster
 * Created
 *
 * ================
 *
 * Copyright (C) 2010 Clive Webster (webbot@webbot.org.uk)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *        File: _stepper_common.c
 *  Created on: 6 Aug 2010
 *      Author: Clive Webster
 */

#include "_stepper_common.h"
#include "../core.h"
#include "../timer.h"

// --- Implement the actuator interface ---------------

/*
 *  Implements actuator.setSpeed
 *  Set the target speed for the motor
 */
static void impl_act_setSpeed(__ACTUATOR* act,DRIVE_SPEED speed){
	STEPPER_MOTOR* motor = (STEPPER_MOTOR*)act;
	CRITICAL_SECTION {
		motor->continuous = TRUE;
		motor->stepsRemaining=0;
	}
}

/*
 * Implements actuator.setConnected
 * by delegating it to the code for the given stepper motor
 */
static void impl_act_setConnected(__ACTUATOR* act,boolean connected){
	STEPPER_MOTOR* motor = (STEPPER_MOTOR*)act;
	// Get the driver class
	const STEPPER_MOTOR_IMPL* driver=motor->class;

	// Call the setConnected method on the class
	if(driver){
		void (*fn)(STEPPER_MOTOR*,boolean connected) = (void (*)(STEPPER_MOTOR*, boolean connected))pgm_read_word(&driver->setConnected);
		if(fn!=null){
			fn(motor,connected);
		}
	}
}

__ACTUATOR_DRIVER_CLASS c_stepper_actuator = MAKE_ACTUATOR_DRIVER_CLASS(&impl_act_setSpeed,&impl_act_setConnected);

// --- End of actuator interface ----------------------

/*
 * Set the actual speed for the motor
 */
static void setActualSpeed(STEPPER_MOTOR* motor, DRIVE_SPEED speed){

	motor->actualSpeed = CLAMP(speed,DRIVE_SPEED_MIN,DRIVE_SPEED_MAX);

	// Get the driver class
	const STEPPER_MOTOR_IMPL* driver=motor->class;

	// Call the setSpeed method on the class
	if(driver){
		void (*fn)(STEPPER_MOTOR*,DRIVE_SPEED speed) = (void (*)(STEPPER_MOTOR*, DRIVE_SPEED speed))pgm_read_word(&driver->setSpeed);
		CRITICAL_SECTION{
			// If stopped then reset counters
			if(speed == 0){
				motor->stepCount = 0;
			}

			if(fn!=null){
				fn(motor,(motor->actuator.inverted) ? speed*-1 : speed);
			}
		}
	}
}


// Accelerate if necessary
static void accelerate(STEPPER_MOTOR* motor){

	int16_t maxAccel = ABS(motor->accelerationStep); // Maximum acceleration
	int16_t minAccel = -1 * maxAccel;				 // Maximum brake
	int16_t  requiredSpeed;

	if(!motor->continuous){
		// We are trying to hit a target

		// Find the number of steps beyond which there is overflow
		int16_t maxAccelSteps = DRIVE_SPEED_MAX / maxAccel;

		if( motor->stepsRemaining > 0 && motor->actualSpeed >= 0){
			// We are travelling the correct way (forwards)
			if(motor->stepsRemaining > maxAccelSteps){
				requiredSpeed = DRIVE_SPEED_MAX;
			}else{
				requiredSpeed = motor->stepsRemaining;
				requiredSpeed *= maxAccel;
			}
		}else if(motor->stepsRemaining < 0 && motor->actualSpeed <= 0 ){
			// We are travelling the correct way (backwards)
			if(motor->stepsRemaining < -maxAccelSteps){
				requiredSpeed = DRIVE_SPEED_MIN;
			}else{
				requiredSpeed = motor->stepsRemaining;
				requiredSpeed *= maxAccel;
			}
		}else{
			// We need to brake either cause we are there
			// or we are travelling in the wrong direction
			requiredSpeed=DRIVE_SPEED_BRAKE;
		}
	}else{
		// Continuous rotation; required = request speed
		requiredSpeed = motor->actuator.required_speed;
	}


	// Limit the acceleration
	int16_t speedDif = requiredSpeed - ((int16_t)motor->actualSpeed);
	speedDif = CLAMP( (int16_t)speedDif, (int16_t)minAccel, (int16_t)maxAccel );


	if(speedDif){
		// If moving to a target then no point have an acceleration < min
		/* Already done by CLAMP above
		if(!motor->continuous){
			if(speedDif < 0){
				if(speedDif < minAccel){
					speedDif = minAccel;
				}
			}else{
				if(speedDif > maxAccel){
					speedDif = maxAccel;
				}
			}
		}
		 */

		int16_t newSpeed = motor->actualSpeed + speedDif;
		newSpeed = CLAMP(newSpeed, DRIVE_SPEED_MIN,DRIVE_SPEED_MAX);


		// Set the actual speed
		setActualSpeed(motor, (DRIVE_SPEED)newSpeed);
	}
}


/*
 * Tell the motor to move by the given number
 * of steps based on its current position
 */
void __stepper_move(STEPPER_MOTOR* motor, int16_t steps){
	if(steps){
		CRITICAL_SECTION{
			motor->continuous = FALSE;
			motor->stepsRemaining = steps;
		}
	}
}

/*
 * Process the internal timer tick interrupt for the driver
 * and update the motors attached to this driver
 */
static void tick(const Timer *timer, void* data){
	STEPPER_DRIVER* driver = data;

	// The current tick number
	uint16_t tick = driver->tickNumber++;

	for(int i= driver->num_motors-1;i>=0;i--){
		STEPPER_MOTOR* motor = (STEPPER_MOTOR*)pgm_read_word(&driver->motors[i]);

		// Accelerate if required
		if(motor->accelerateEvery && tick % motor->accelerateEvery == 0){
			accelerate(motor);
		}

		if(motor->actualSpeed && motor->actuator.connected){
			// The motor is moving and connected

			// Decide if we need to generate a new pulse to the motor
			motor->stepCount += motor->actualSpeed;
			int8_t tick = 0;

			if(motor->stepCount < 0){
				motor->stepCount += DRIVE_SPEED_MAX;
				tick = -1;
				if(motor->position == 0){
					motor->position = motor->stepsPerRevolution;
				}
				motor->position+=tick;
			}else if(motor->stepCount >= DRIVE_SPEED_MAX){
				motor->stepCount -= DRIVE_SPEED_MAX;
				tick = 1;
				motor->position+=tick;
				if(motor->position >= motor->stepsPerRevolution){
					motor->position -= motor->stepsPerRevolution;
				}
			}

			if(tick){
				// Generate another step
				const struct c_stepper_impl* clazz = motor->class;
				void (*fn)(STEPPER_MOTOR*) = (void (*)(STEPPER_MOTOR*))pgm_read_word(&clazz->step);
				if(fn){
					fn(motor);
				}

				// Update number of ticks remaining
				if(!motor->continuous){
					motor->stepsRemaining -= tick;
					if(motor->stepsRemaining == 0){
						// We have reached the goal
						motor->actuator.required_speed = DRIVE_SPEED_BRAKE;
						setActualSpeed(motor,DRIVE_SPEED_BRAKE);
						motor->continuous = TRUE;
					}
				}
			}
		}
	}
}

// Set up the timer to generate the pulses
void stepper_init(STEPPER_DRIVER* driver, const Timer* timer){

	// initialise each motor
	for(int i= driver->num_motors-1;i>=0;i--){
		STEPPER_MOTOR* motor = (STEPPER_MOTOR*)pgm_read_word(&driver->motors[i]);

		// Initialise the motor
		const struct c_stepper_impl* clazz = motor->class;
		void (*fn)(STEPPER_MOTOR*) = (void (*)(STEPPER_MOTOR*))pgm_read_word(&clazz->init);
		if(fn){
			fn(motor);
		}

		// Start off braking
		act_setSpeed(motor,DRIVE_SPEED_BRAKE);

		// Indicate the motor is connected
		act_setConnected(motor,TRUE);
	}


	uint32_t deciHertz = driver->stepFrequency;
	deciHertz *= 10;

	// Find the best PWM setting
	TIMER_MODE mode;
	uint16_t icr;
	uint16_t prescaler;
	boolean valid = timerCalcPwm(timer, deciHertz, 1, &mode, &icr, &prescaler);

	if(!valid){
		// There is no PWM setting that is valid
		setError( (timerIsInUse(timer)) ? PWM_TIMER_IN_USE : TIMER_HAS_NO_PWM );
	}else{
		// Lets set up the PWM
		if(!timerIsInUse(timer)){
			timerSetMode(timer,mode);
			if(modeIsICR(mode)){
				// Set the ICR
				PORT icrPort = pgm_read_word(&timer->pgm_icr);
				_SFR_MEM16(icrPort)=icr;
			}
		}else{
			// Can't change the prescaler so just use the existing
			prescaler = timerGetPrescaler(timer);
		}

		// Set the overflow
		timerOverflowAttach(timer, &tick, driver);

		// Turn on the timer
		timerSetPrescaler(timer,prescaler);

		// Set the return value
		driver->actualFrquency  = TimerGetPwmDeciHertz(timer) / 10;
	}

}
