
/*
 * $Id: TB6612FNG_3pin.c,v 1.7 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: TB6612FNG_3pin.c,v $
 * Revision 1.7  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.6  2010/10/06 12:13:53  clivewebster
 * Only set driver class if there are no errors
 *
 * Revision 1.5  2010/07/01 23:47:15  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.4  2010/06/14 18:46:56  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/10/27 20:56:44  clivewebster
 * Use new PWM initialisation
 *
 * Revision 1.2  2009/10/26 19:01:37  clivewebster
 * interpolateU
 *
 * Revision 1.1  2009/10/09 18:22:27  clivewebster
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
 *
 * TB6612FNG.c
 *
 *
 *  Created on: 03-Oct-2009
 *      Author: Clive Webster
 *
 *  A driver that uses 3 pins per motor - 1 pwm output and 2 IO pins
 */

#include "TB6612FNG.h"
#include "../../errors.h"
#include "../../iopin.h"
#include "../../timer.h"

// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	TOSHIBA_TB6612FNG_3pin_MOTOR* motor = (TOSHIBA_TB6612FNG_3pin_MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pwm);
	const Timer* timer = compareGetTimer(channel);
	uint16_t top = timerGetTOP(timer);

	// New compare threshold
	uint16_t delay=0;

	if( speed > 0 ){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MAX, 0 , top);
		// Set pins high and low
		pin_high(motor->enable1);
		pin_low(motor->enable2);
	}else if(speed < 0){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MIN,  0 , top);
		// Set pins low and high
		pin_low(motor->enable1);
		pin_high(motor->enable2);
	}else{
		// brake
		// Set both pins high, and the PWM to anything
		delay = 0;
		pin_high(motor->enable1);
		pin_high(motor->enable2);
	}

	// Change the duty cycle
	compareSetThreshold(channel,delay);
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	TOSHIBA_TB6612FNG_3pin_MOTOR* motor = (TOSHIBA_TB6612FNG_3pin_MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pwm);

	if(connected){
		// connect
		// turn PWM back on
		compareSetOutputMode(channel, CHANNEL_MODE_NON_INVERTING);
		// restore previous speed
		setSpeed(actuator, act_getSpeed(motor));
	}else{
		// coast Set PWM output always high
		compareSetOutputMode(channel, CHANNEL_MODE_DISCONNECT);
		pin_high(motor->pwm);

		// Set both input pins low
		pin_low(motor->enable1);
		pin_low(motor->enable2);
	}

}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_motors = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);

// Pass the list of servos, the list should be in PROGMEM space to save Flash RAM
// The specified Timer must implement timer compare interrupts and, if so, it will
// ise the timer compare channel A (if there is more than one)
void toshibaTB6612FNG_3pin_Init(TOSHIBA_TB6612FNG_3pin_MOTOR_DRIVER* driver, uint32_t freq){
	uint32_t deciHertz = 10 * freq;
	// Make sure each servo is set as an output
	for(int i= driver->num_motors-1;i>=0;i--){
		TOSHIBA_TB6612FNG_3pin_MOTOR* motor = (TOSHIBA_TB6612FNG_3pin_MOTOR*)pgm_read_word(&driver->motors[i]);

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
			motor->actuator.class = &c_motors;

			// Use non-inverting PWM
			compareSetOutputMode(channel,CHANNEL_MODE_NON_INVERTING);

			// Make sure the pwm pin is an output pin
			pin_make_output(motor->pwm, FALSE);

			// Make sure the direction pin are set as an output pin and select 'brake'
			pin_make_output(motor->enable1, TRUE);
			pin_make_output(motor->enable2, TRUE);

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
