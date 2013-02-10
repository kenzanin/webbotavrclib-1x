/*
 * $Id: servos.c,v 1.11 2010/10/06 12:13:53 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: servos.c,v $
 * Revision 1.11  2010/10/06 12:13:53  clivewebster
 * Only set driver class if there are no errors
 *
 * Revision 1.10  2010/09/30 17:00:21  clivewebster
 * Refactored so that specialist info is now in a union to reduce memory.
 *
 * Revision 1.9  2010/07/01 23:54:46  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.8  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.7  2010/03/19 01:50:11  clivewebster
 * Improve software PWM handling
 *
 * Revision 1.6  2010/02/06 13:26:04  clivewebster
 * Allow setServoConfig to change the settings for a servo
 *
 * Revision 1.5  2009/10/26 18:53:31  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/10/09 17:41:29  clivewebster
 * *** empty log message ***
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
 *
 * servos.c
 *
 * A software PWM driver for between 8 and 10 servos.
 * NB If you have more servos then you can you create multiple servo drivers.
 *
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "servos.h"
#include "errors.h"
#include "iopin.h"
#include "timer.h"
#include "core.h"

// Define the timer channels used to time the next servo on and the previous servo off
#define CHANNEL_ON  0
#define CHANNEL_OFF 1


// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SERVO* servo = (SERVO*)actuator;

	// Interpolate the values
	uint16_t ticks=interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, servo->min_ticks , servo->max_ticks);
	if(ticks!=servo->delay){
		CRITICAL_SECTION{
			servo->delay = ticks;
		};
	}
}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_Servos = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,null);


// Called to turn off the current pulse
static void __servoEndService(const TimerCompare *timer_compare, void* data){
	SERVO_DRIVER* driver = (SERVO_DRIVER*)data;

	if(driver->specific.softwarePWM.currentServo < driver->num_servos){
		// turn off the previous channel
		SERVO* servo = (SERVO *)pgm_read_word(&driver->servos[driver->specific.softwarePWM.currentServo]);
		pin_low(servo->pin);
	}

	// disable channel that turns pulses off
	__portMaskClear(&timer_compare->pgm_intenable);

}

static void __servoStartService(const TimerCompare *timer_compare, void* data){
	uint16_t delay;	// The next delay in cycles
	SERVO_DRIVER* driver = (SERVO_DRIVER*)data;
	const Timer* timer = compareGetTimer(timer_compare);
	const TimerCompare* turnOff = timerGetCompare(timer,CHANNEL_OFF);

	// Wait if previous pulse is still going
	while( __portMaskGet(&turnOff->pgm_intenable) ){
		breathe();
	}

	// Start processing the next one
	if(driver->servos_init){
		driver->specific.softwarePWM.currentServo++;
	}else{
		driver->servos_init=TRUE;
		// Set the threshold to when the first compare was fired
		// so that if there are multiple banks then each bank is
		// reset to use its actual start time.
		compareSetThreshold(timer_compare,timerGetCounter(compareGetTimer(timer_compare)));
	}

	if(driver->specific.softwarePWM.currentServo != driver->num_servos){
		// do the next servo
		if(driver->specific.softwarePWM.currentServo > driver->num_servos){
			// restart from the beginning
			driver->specific.softwarePWM.currentServo = 0;
			driver->specific.softwarePWM.total_delay = 0;

		}

		// Locate the servo
		SERVO* servo = (SERVO *)pgm_read_word(&driver->servos[driver->specific.softwarePWM.currentServo]);
		delay = servo->delay;		// The pulse length to set

		uint16_t now = timerGetCounter(timer);

		// ignore the servo if its not connected
		if(act_isConnected(servo)){
			// turn on servo channel bit
			pin_high(servo->pin);
		}


		// Set time to turn pin low
		compareSetThreshold(turnOff,servo->delay + now);

		// enable channel that turns pulses off
		compareClearInterruptPending(turnOff);
		__portMaskSet(&turnOff->pgm_intenable);

		// Set the time to check the next servo
		compareSetThreshold(timer_compare,servo->max_ticks + now);

		// Accumulate total time in this pass of the servos
		driver->specific.softwarePWM.total_delay += servo->max_ticks;

	}else{
		// disable channel that turns pulses off
		__portMaskClear(&turnOff->pgm_intenable);

		// pause before going back to the first servo
		delay = (driver->specific.softwarePWM.total_delay > driver->specific.softwarePWM.servo_cycle)
				? 100
				:  driver->specific.softwarePWM.servo_cycle - driver->specific.softwarePWM.total_delay;

		// Re-fire the event
		compareSetThreshold(timer_compare,delay + compareGetThreshold(timer_compare));
	}


}

void servosInit(SERVO_DRIVER* driver, const Timer* timer){
	const TimerCompare* channel1 = timerGetCompare(timer,CHANNEL_ON);
	const TimerCompare* channel2 = timerGetCompare(timer,CHANNEL_OFF);

	if(timerIs16bit(timer) && timerNumberOfCompareUnits(timer) >= 2){
		int i;

		// Select a prescaler that is bigger than the biggest pause.
		// Added 5ms just in case other interrupts kick in.
		uint16_t prescale;

		// If the timer is already in use then find its prescaler
		if(timerIsInUse(timer)){
			prescale = timerGetPrescaler(timer);
			if(timerGetMode(timer) != TIMER_MODE_NORMAL){
				setError(PWM_TIMER_IN_USE);
			}
		}else{
			prescale = timerGetBestPrescaler(timer, SERVO_CYCLE + 5 );
		}

		driver->specific.softwarePWM.currentServo = driver->num_servos;

		// Find how many timer ticks there will be for SERVO_CYCLE
		uint32_t tmp32 = ticks_per_ms(SERVO_CYCLE,prescale);
		driver->specific.softwarePWM.servo_cycle = tmp32;
		// make sure it fits
		if( (tmp32 & 0xFFFF0000UL)!=0){
			setError(SERVO_TIMING);
			return;
		}


		// Make sure each servo is set as an output
		for(i= driver->num_servos-1;i>=0;i--){
			SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);

			// Connect the servo to this driver
			servo->actuator.class = &c_Servos;
			servo->driver = driver;
			servo->top = driver->specific.softwarePWM.servo_cycle;

			pin_make_output(servo->pin, FALSE);

			servoSetConfig(servo,servo->center_us,servo->range_us);

			act_setConnected(servo,TRUE);

			// Start off braking
			act_setSpeed(servo,DRIVE_SPEED_BRAKE);
		}


		// Set the threshold to start interrupt chain
		if(getError()==0){
			CRITICAL_SECTION_START;

			compareAttach(channel1, &__servoStartService,  compareGetThreshold(channel1) + driver->specific.softwarePWM.servo_cycle, driver);
			compareAttach(channel2, &__servoEndService  ,  compareGetThreshold(channel2) + driver->specific.softwarePWM.servo_cycle, driver);
			__portMaskClear(&channel2->pgm_intenable);		// disable channel2

			// Do this last as it then turns on the timer
			timerSetPrescaler(timer,prescale);


			CRITICAL_SECTION_END;
		}



	}else{
		// We need a 16 bit bit timer
		setError(SERVO_TIMER_MUST_BE_16_BIT);
	}

}

