/*
 * $Id: L293D.c,v 1.2 2011/07/09 16:43:49 clivewebster Exp $
 * Revision History
 * ================
 * $Log: L293D.c,v $
 * Revision 1.2  2011/07/09 16:43:49  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.1  2010/10/08 00:15:45  clivewebster
 * Add specific L293D DC motor 2 pin and 3 pin support
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
 *        File: L293D.c
 *  Created on: 7 Oct 2010
 *      Author: Clive Webster
 *
 *      L293D and compatibles like the SN754410.
 *
 *      Implement 2 and 3 wire connections.
 *      Motors are defined via motorPWN.h using MAKE_MOTOR_2/3_PIN
 *      They are bundled into a MOTOR_LIST which is passed to a MOTOR_DRIVER
 *      The driver is initialised using motorL293Init so that this code is enabled
 *
 */
#include "../motorPWM.h"
#include "../errors.h"
#include "../iopin.h"
#include "../timer.h"
#include "../core.h"


// Call back - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	MOTOR* motor = (MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pwm);
	const Timer* timer = compareGetTimer(channel);
	uint16_t top = timerGetTOP(timer);

	// New compare threshold
	uint16_t delay=0;

	if( speed > 0 ){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MAX, 0 , top);

		if(motor->direction2==null){
			// one wire so delay = top - delay
			delay = top - delay;
		}

		// Set direction1 high, direction2 low (if there is one)
		pin_make_output(motor->direction1,TRUE);
		pin_make_output(motor->direction2,FALSE);
	}else if(speed < 0){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MIN,  0 , top);

		// Set direction1 low, direction2 high (if there is one)
		pin_make_output(motor->direction1,FALSE);
		pin_make_output(motor->direction2,TRUE);
	}else{
		// brake
		if(motor->direction2){
			// There are two direction pins - so set both to same value
			pin_make_output(motor->direction1,FALSE);
			pin_make_output(motor->direction2,FALSE);
			delay = top; // full speed brake
		}else{
			// Only has one direction pin
			// Set direction1 low
			pin_make_output(motor->direction1,FALSE);
			// PWM delay = 0 so pwm = low
			// ie both low = brake
		}
	}

	// Change the duty cycle
	compareSetThreshold(channel,delay);
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	MOTOR* motor = (MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pwm);

	// Turn on/off the pin to start/stop sending PWM
	// reverts to default output logic level of low
	compareSetOutputMode(channel, (connected) ? CHANNEL_MODE_NON_INVERTING : CHANNEL_MODE_DISCONNECT);
	pin_make_output(motor->direction1,FALSE);
}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_l293 = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);


// Pass the list of motors, the list should be in PROGMEM space to save Flash RAM
// The specified Timer must implement timer compare interrupts and, if so, it will
// use the timer compare channel A (if there is more than one)
void motorL293Init(MOTOR_DRIVER* driver, uint32_t freq){
	uint32_t deciHertz = 10 * freq;
	// Make sure each servo is set as an output
	for(int i= driver->num_motors-1;i>=0;i--){
		MOTOR* motor = (MOTOR*)pgm_read_word(&driver->motors[i]);

		// Find the PWM timer compare channel for the output pin
		const TimerCompare* channel = compareFromIOPin(motor->pwm);
		if(channel==null){
			setError(PWM_PIN_NOT_AVAILABLE);
			continue;
		}
		if(compareIsInUse(channel)){
			setError(PWM_PIN_IN_USE);
			continue;
		}

		TIMER_MODE mode;
		uint16_t icr;
		uint16_t prescaler;

		const Timer* timer = compareGetTimer(channel);

		// Find the best PWM setting for 10kHz, with 128 steps
		boolean valid = timerCalcPwm(timer, deciHertz, 128, &mode, &icr, &prescaler);

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
			}

			// Connect motor to driver
			motor->actuator.class = &c_l293;

			// Make sure the motor pins are set as output pins
			pin_make_output(motor->pwm, FALSE);
			pin_make_output(motor->direction1, FALSE);
			pin_make_output(motor->direction2, FALSE);

			// Mark the channel as in use
			compareAttach(channel,&nullTimerCompareCallback,0,null);

			// Do this last as it then turns on the timer
			timerSetPrescaler(timer,prescaler);

			// Start off braking
			act_setSpeed(motor,DRIVE_SPEED_BRAKE);

			// Indicate the motor is connected
			act_setConnected(motor,TRUE);
		}
	}
}

