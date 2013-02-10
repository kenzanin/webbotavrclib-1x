 /*
 * $Id: servoPWM.c,v 1.9 2010/10/06 12:13:53 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: servoPWM.c,v $
 * Revision 1.9  2010/10/06 12:13:53  clivewebster
 * Only set driver class if there are no errors
 *
 * Revision 1.8  2010/07/01 23:54:29  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.7  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/02/06 13:26:04  clivewebster
 * Allow setServoConfig to change the settings for a servo
 *
 * Revision 1.5  2009/11/02 19:05:25  clivewebster
 * Added revision log
 *
 * ===========
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
 * servoPWM.c
 *
 *  Control servos using PWM
 *
 *  Created on: 16-Mar-2009
 *      Author: Clive Webster
 */
#include "errors.h"
#include "iopin.h"
#include "timer.h"
#include "core.h"
#include "servos.h"

// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SERVO* servo = (SERVO*)actuator;
	const TimerCompare* channel = compareFromIOPin(servo->pin);

	// Interpolate the values
	uint16_t delay = interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, servo->min_ticks , servo->max_ticks);
//	servo->delay = delay;

	// Change the duty cycle
	compareSetThreshold(channel,delay);
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	SERVO* servo = (SERVO*)actuator;
	const TimerCompare* channel = compareFromIOPin(servo->pin);

	// Turn on/off the pin
	compareSetOutputMode(channel, (connected) ? CHANNEL_MODE_NON_INVERTING : CHANNEL_MODE_DISCONNECT);
}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_servoPWM = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);


// Pass the list of servos, the list should be in PROGMEM space to save Flash RAM
// The specified Timer must implement timer compare interrupts and, if so, it will
// ise the timer compare channel A (if there is more than one)
void servoPWMInit(SERVO_DRIVER* driver){
	// Make sure each servo is set as an output
	for(int i= driver->num_servos-1;i>=0;i--){
		SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);

		// Connect the servo to this driver
		servo->driver = driver;

		const TimerCompare* channel = compareFromIOPin(servo->pin);
		if(channel==null){
			setError(PWM_PIN_NOT_AVAILABLE);
			continue;
		}
		if(compareIsInUse(channel)){
			setError(PWM_PIN_IN_USE);
			continue;
		}

		// Find which timer the compare unit is attached to
		const Timer* timer = compareGetTimer(channel);

		// We need a 16 bit timer
		if(!timerIs16bit(timer)){
			setError(SERVO_TIMER_MUST_BE_16_BIT);
			continue;
		}

		// Make sure it supports fast pwm using ICR
		if(!timerIsModeSupported(timer,TIMER_MODE_PWM_FAST_ICR)){
			setError(PWM_TIMER_MODE_PWM_FAST_ICR_UNSUPPORTED);
			continue;
		}

		// Find the optimum prescale value
		uint16_t prescale=timerGetBestPrescaler(timer, SERVO_CYCLE);
		if(timerIsInUse(timer) && (timerGetPrescaler(timer)!=prescale || timerGetMode(timer)!=TIMER_MODE_PWM_FAST_ICR)){
			setError(PWM_TIMER_IN_USE);
			continue;
		}

		// Connect to driver
		servo->actuator.class = &c_servoPWM;

		// Make sure its an output pin, low
		pin_make_output(servo->pin, FALSE);

		// put the timer into the correct mode
		timerSetMode(timer,TIMER_MODE_PWM_FAST_ICR);

		// Set top for the correct cycle time
		uint16_t top = ticks_per_ms(SERVO_CYCLE,prescale);
		compareAttach(channel,&nullTimerCompareCallback,0,null);
		PORT icr = pgm_read_word(&timer->pgm_icr);
		_SFR_MEM16(icr)=top;
		servo->top = top;

//		rprintf("prescale=%d",prescale);rprintf("top=%d\n",top);

		// Take the useable servo range and calculate the min/max pulses
		// (SERVO_CYCLE*1000) = top
		//        min			    x
		// x = (min * g_servo_cycle)/(SERVO_CYCLE*1000)
		/*
		uint32_t min = servo->center_us - servo->range_us;
		uint32_t m1 =  top / 1000;
		uint32_t m2 =  m1 * min;
		uint32_t m3 =  m2 / SERVO_CYCLE;
		servo->min_ticks = m3;

		uint32_t max = servo->center_us + servo->range_us;
		m2 =  m1 * max;
		m3 =  m2 / SERVO_CYCLE;
		servo->max_ticks = m3;
		*/
		servoSetConfig(servo, servo->center_us, servo->range_us);

		// Do this last as it then turns on the timer
		timerSetPrescaler(timer,prescale);

		// Start off braking
		act_setSpeed(servo,DRIVE_SPEED_BRAKE);

		// Indicate the servo is connected
		act_setConnected(servo,TRUE);
	}
}

