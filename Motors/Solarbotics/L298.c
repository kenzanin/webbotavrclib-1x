
/*
 * $Id: L298.c,v 1.8 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: L298.c,v $
 * Revision 1.8  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.7  2010/10/06 12:13:53  clivewebster
 * Only set driver class if there are no errors
 *
 * Revision 1.6  2010/07/01 23:46:37  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.5  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/05/28 18:01:32  clivewebster
 * Fixed bug where motor only turned one way
 *
 * Revision 1.3  2009/10/27 20:57:23  clivewebster
 * Use new PWM initialisation
 *
 * Revision 1.2  2009/10/26 19:01:37  clivewebster
 * interpolateU
 *
 * Revision 1.1  2009/10/09 18:26:54  clivewebster
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
 * L298.c
 *
 *  Created on: 03-Oct-2009
 *      Author: Clive Webster
 */

#include "L298.h"
#include "../../errors.h"
#include "../../iopin.h"
#include "../../timer.h"

// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SOLAR_L298_MOTOR* motor = (SOLAR_L298_MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pin);
	const Timer* timer = compareGetTimer(channel);
	uint16_t top = timerGetTOP(timer);

	// New compare threshold
	uint16_t delay=0;

	if( speed > 0 ){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MAX, 0 , top);
		// Set pins low and high
		pin_low(motor->enable1);
		pin_high(motor->enable2);

	}else if(speed < 0){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MIN,  0 , top);
		// Set pins high and low
		pin_high(motor->enable1);
		pin_low(motor->enable2);
	}else{
		// brake
		// Force output to stay high
		delay=top;
		// Set both pins low
		pin_low(motor->enable1);
		pin_low(motor->enable2);
	}

	// Change the duty cycle
	compareSetThreshold(channel,delay);
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	SOLAR_L298_MOTOR* motor = (SOLAR_L298_MOTOR*)actuator;
	const TimerCompare* channel = compareFromIOPin(motor->pin);

	// Turn on/off the pin to start/stop sending PWM
	compareSetOutputMode(channel, (connected) ? CHANNEL_MODE_NON_INVERTING : CHANNEL_MODE_DISCONNECT);
}

// Define the class
static const __ACTUATOR_DRIVER_CLASS c_motors = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);

// Pass the list of servos, the list should be in PROGMEM space to save Flash RAM
// The specified Timer must implement timer compare interrupts and, if so, it will
// ise the timer compare channel A (if there is more than one)
void solarL298Init(SOLAR_L298_MOTOR_DRIVER* driver, uint32_t freq){
	uint32_t deciHertz = 10 * freq;
	// Make sure each servo is set as an output
	for(int i= driver->num_motors-1;i>=0;i--){
		SOLAR_L298_MOTOR* motor = (SOLAR_L298_MOTOR*)pgm_read_word(&driver->motors[i]);

		// Find the PWM timer compare channel for the output pin
		const TimerCompare* channel = compareFromIOPin(motor->pin);
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

			// Make sure the motor pin is an output pin
			pin_make_output(motor->pin, FALSE);

			// Make sure the direction pin is set as an output pin
			pin_make_output(motor->enable1, FALSE);
			pin_make_output(motor->enable2, FALSE);

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
