/*
 * $Id: servoMux.c,v 1.1 2011/02/21 20:29:31 clivewebster Exp $
 * Revision History
 * ================
 * $Log: servoMux.c,v $
 * Revision 1.1  2011/02/21 20:29:31  clivewebster
 * Added muxServo
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: servoMux.c
 *  Created on: 19 Feb 2011
 *      Author: Clive Webster
 *
 *  Drive up to 8 servos with one 16 bit PWM pin. The PWM output goes into a de-multiplexer
 *  such as the 74HC238 and its output is sent to one of 8 output by using 3 I/O pins to control
 *  the de-multiplexer.
 *
 */
#include "timer.h"
#include "servos.h"


static volatile uint16_t maxDelay;

/**
 * We have just hit top and are starting to count down again.
 * The new PWM duty cycle has been clocked in for the next servo
 */
static void service(const Timer *timer, void* data){
	SERVO_DRIVER* driver = data;
	uint8_t i;

	// Move to the next servo
	uint8_t index = (driver->specific.softwareMUX.currentServo + 1)& 7;
	driver->specific.softwareMUX.currentServo = index;

	// Set the multiplex pins so the pulse goes to the correct servo
	// This needs to be done quickly to stop the start of the pulse
	// going to the wrong sevo
	uint8_t bits = index;
	for (i = 0; i < NUM_MUX_PINS; i++){
		const IOPin* pin = driver->specific.softwareMUX.muxPins[i];
		if(bits & 1){
			pin_high(pin);
		}else{
			pin_low(pin);
		}
		bits >>= 1;
	}

	// Keep track of the lowest value of the timer counter
	// Setting a delay between this value and TOP will mean the mux bits dont get changed in time
	uint16_t thisDelay = timerGetCounter(timer);
	if(thisDelay < maxDelay){
		maxDelay = thisDelay;
	}

	// Time critical part is over
	// Now calculate the pulse width for next servo
	uint16_t newPos=0;
	index = (index + 1) & 7;
	if(index < driver->num_servos){
		SERVO* servo = (SERVO *)pgm_read_word(&driver->servos[index]);
		if(servo->actuator.connected){
			newPos = servo->delay;

			// Limit to the maximum value
			uint16_t limit = maxDelay;
			if(newPos > limit){
				newPos = limit;
			}

		}
	}

	// Set the threshold inline - to save more registers push/pops
	//compareSetThreshold(channel,newPos);
	PORT port = driver->specific.softwareMUX.pwmPort;
	_SFR_MEM16(port) = newPos; 					// set 16 bit word
}

static void mux_setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SERVO* servo = (SERVO*)actuator;

	// Interpolate the values
	uint16_t ticks=interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, servo->min_ticks , servo->max_ticks);

	// The min/max ticks are based on a 20ms frequency, but we actually using 2.5ms
	// so multiply by 20/2.5 = 8
	ticks *= 8;		//

	if(ticks!=servo->delay){
		CRITICAL_SECTION{
			servo->delay = ticks;
		};
	}
}

// Define the class
static const __ACTUATOR_DRIVER_CLASS c_mux_Servos = MAKE_ACTUATOR_DRIVER_CLASS(&mux_setSpeed,null);

void servosMuxInit(SERVO_DRIVER* driver, const IOPin* pwm16, const IOPin* mux1, const IOPin* mux2, const IOPin* mux3){
	const TimerCompare* channel = compareFromIOPin(pwm16);
	const Timer* timer = (channel==null) ? null : compareGetTimer(channel);

	if(timer!=null && timerIs16bit(timer)){
		int i;

		// Select a prescaler that allows us to have a cycle time of 2.5ms
		// ie 20ms / 8 servers = 2.5ms per servo. 2.5ms = 400Hz
		uint16_t prescale;
		uint16_t bestTop=0;
		uint32_t maxTop = (timerIs16bit(timer)) ? 0xFFFFU : 0xFFU;
		for(prescale=1; prescale<=MAX_PRESCALE; prescale<<=1){
			int inx = __timerPrescalerIndex(timer, prescale);
			if(inx!=-1){
				// TOP=F_CPU / (2 * Prescaler * 400Hz)
			    uint32_t top = cpu_speed / (uint16_t)(2U*400U * prescale);
			    if(top <= maxTop){
			    	bestTop = top;
			    	break;
			    }
			}
		}


		// If the timer is already in use then find its prescaler
		if(timerIsInUse(timer)){
			if(timerGetMode(timer) != TIMER_MODE_PWM_PHASE_CORRECT_ICR
					|| timerGetPrescaler(timer)!=prescale
					|| timerGetTOP(timer)!=bestTop
					){
				setError(PWM_TIMER_IN_USE);
			}
		}else{
			timerSetMode(timer,TIMER_MODE_PWM_PHASE_CORRECT_ICR);
			// Set the ICR
			PORT icrPort = pgm_read_word(&timer->pgm_icr);
			_SFR_MEM16(icrPort)=bestTop;
		}
		// Use non-inverted PWM
		compareSetOutputMode(channel,CHANNEL_MODE_NON_INVERTING);


		driver->specific.softwareMUX.currentServo = driver->num_servos;
		driver->specific.softwareMUX.servo_cycle = bestTop;

		// Set all the multiplex pins as output low
		driver->specific.softwareMUX.muxPins[0] = mux1;
		driver->specific.softwareMUX.muxPins[1] = mux2;
		driver->specific.softwareMUX.muxPins[2] = mux3;
		PORT port = (PORT)pgm_read_word(&channel->pgm_threshold);
		driver->specific.softwareMUX.pwmPort = port;
		pin_make_output(mux1,FALSE);
		pin_make_output(mux2,FALSE);
		pin_make_output(mux3,FALSE);

		maxDelay = bestTop;

		// Initialise each servo
		for(i= driver->num_servos-1;i>=0;i--){
			SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);

			// Connect the servo to this driver
			servo->actuator.class = &c_mux_Servos;
			servo->driver = driver;
			servo->top = driver->specific.softwareMUX.servo_cycle;

			servoSetConfig(servo,servo->center_us,servo->range_us);

			// The servo is connected
			act_setConnected(servo,TRUE);

			// Start off braking
			act_setSpeed(servo,DRIVE_SPEED_BRAKE);
		}


		// Set up the timer to start processing interrupts
		if(getError()==0){
			CRITICAL_SECTION{
				timerCaptureAttach(timer,&service,driver,false);
				// Set 0% duty cycle to start with
				compareSetThreshold(channel,0);

				// Do this last as it then turns on the timer
				timerSetPrescaler(timer,prescale);
			}
		}



	}else{
		// We need a 16 bit bit timer
		setError(SERVO_TIMER_MUST_BE_16_BIT);
	}

}
