
/*
 * $Id: LB1836M.c,v 1.7 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: LB1836M.c,v $
 * Revision 1.7  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.6  2010/10/06 12:13:53  clivewebster
 * Only set driver class if there are no errors
 *
 * Revision 1.5  2010/07/01 23:46:03  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.4  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/10/27 20:57:22  clivewebster
 * Use new PWM initialisation
 *
 * Revision 1.2  2009/10/26 19:01:37  clivewebster
 * interpolateU
 *
 * Revision 1.1  2009/10/09 18:37:07  clivewebster
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
 * LB1836M.c
 *
 *
 *  Created on: 03-Oct-2009
 *      Author: Clive Webster
 *
 *  A driver that uses 2 pwm pins per motor
 */

#include "LB1836M.h"
#include "../../errors.h"
#include "../../iopin.h"
#include "../../timer.h"

// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SANYO_LB1836M_MOTOR* motor = (SANYO_LB1836M_MOTOR*)actuator;
	const TimerCompare* channel1 = compareFromIOPin(motor->pwm1);
	const TimerCompare* channel2 = compareFromIOPin(motor->pwm2);

	// New compare threshold
	uint16_t delay=0;

	if( speed > 0 ){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MAX, 0, timerGetTOP(compareGetTimer(channel2)));
		compareSetThreshold(channel1,0);	    // Keep permanently high
		compareSetThreshold(channel2,delay);	// pwm channel 2
	}else if(speed < 0){
		delay = interpolateU(speed, 0, DRIVE_SPEED_MIN,  0 , timerGetTOP(compareGetTimer(channel1)));
		compareSetThreshold(channel2,0);	    // Keep permanently high
		compareSetThreshold(channel1,delay);	// pwm channel 1
	}else{
		// brake
		// Set both pins high
		compareSetThreshold(channel1,0);	    // Keep permanently high
		compareSetThreshold(channel2,0);	    // Keep permanently high
	}
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	SANYO_LB1836M_MOTOR* motor = (SANYO_LB1836M_MOTOR*)actuator;
	const TimerCompare* channel1 = compareFromIOPin(motor->pwm1);
	const TimerCompare* channel2 = compareFromIOPin(motor->pwm2);

	if(connected){
		// connect
		// restore previous speed
		setSpeed(actuator, act_getSpeed(motor));
	}else{
		// Set both outputs to low to coast - by setting duty cycle to TOP
		compareSetThreshold(channel1, timerGetTOP(compareGetTimer(channel1)));
		compareSetThreshold(channel2, timerGetTOP(compareGetTimer(channel2)));
	}

}

// Define the class
static const __ACTUATOR_DRIVER_CLASS c_motors = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);

// Return true if valid, false if there is an error
static boolean initPWM(const IOPin* pin, uint32_t deciHertz){
	const TimerCompare* channel = compareFromIOPin(pin);

	if(channel==null){
		setError(PWM_PIN_NOT_AVAILABLE);
		return FALSE;
	}
	if(compareIsInUse(channel)){
		setError(PWM_PIN_IN_USE);
		return FALSE;
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

		// Make it an output pin and set high for brake
		pin_make_output(pin,TRUE);


		// Use inverting PWM
		compareSetOutputMode(channel,CHANNEL_MODE_INVERTING);


		// Mark the channels as in use
		compareAttach(channel,&nullTimerCompareCallback,0,null);

		// Do this last as it then turns on the timer
		timerSetPrescaler(timer,prescaler);
	}
	return valid;
}

// Pass the list of servos, the list should be in PROGMEM space to save Flash RAM
// The specified Timer must implement timer compare interrupts and, if so, it will
// use the timer compare channel A (if there is more than one)
void sanyoLB1836M_Init(SANYO_LB1836M_MOTOR_DRIVER* driver, uint32_t freq){
	uint32_t deciHertz = 10 * freq;
	// Make sure each servo is set as an output
	for(int i= driver->num_motors-1;i>=0;i--){
		SANYO_LB1836M_MOTOR* motor = (SANYO_LB1836M_MOTOR*)pgm_read_word(&driver->motors[i]);


		if( initPWM(motor->pwm1, deciHertz) ){
			if(initPWM(motor->pwm2, deciHertz)){
				// Connect motor to driver
				motor->actuator.class = &c_motors;
			}
		}

		// Start off braking
		act_setSpeed(motor,DRIVE_SPEED_BRAKE);

		// Indicate the motor is connected
		act_setConnected(motor,TRUE);
	}
}
