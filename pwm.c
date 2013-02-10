
/*
 * $Id: pwm.c,v 1.5 2010/07/01 23:53:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: pwm.c,v $
 * Revision 1.5  2010/07/01 23:53:21  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.4  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/02/09 14:41:20  clivewebster
 * Fix bug that changed prescaler on an 'in use' timer
 *
 * Revision 1.2  2009/10/27 20:52:46  clivewebster
 * Use shared null compare interrupt routine
 *
 * Revision 1.1  2009/10/26 18:49:24  clivewebster
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
 * pwm.c
 *
 *  Created on: 22-Oct-2009
 *      Author: Clive Webster
 *
 *
 */
#include "pwm.h"
#include "timer.h"
//#include "core.h"

// Set the duty cycle
void pwmSetDutyCycle(const IOPin* pin, DUTY_CYCLE duty){
	const TimerCompare* channel = compareFromIOPin(pin);
	if(channel){
		const Timer* timer = compareGetTimer(channel);
		uint32_t top = timerGetTOP(timer);

		// Limit the duty cycle
		if(duty>100)
			duty=100;

		// 100 => top
		// duty => x
		// x = (top * duty) / 100
//		top *= duty;
//		top /= 100;
//		uint16_t delay =  top;
		uint16_t delay = interpolateU(duty, 0,100, 0,top);

		// Change the duty cycle
		compareSetThreshold(channel,delay);
	}
}

DUTY_CYCLE pwmGetDutyCycle(const IOPin* pin){
	DUTY_CYCLE rtn = 0;

	const TimerCompare* channel = compareFromIOPin(pin);
	if(channel){
		const Timer* timer = compareGetTimer(channel);
		uint32_t top = timerGetTOP(timer);
		uint32_t duty = compareGetThreshold(channel);
		// top => 100
		// duty => x
		// x = (100 * duty) / top
		duty *= 100;
		duty /= top;
		rtn = duty;
	}

	return rtn;
}

// Turn PWM off for the given pin
void pwmOff(const IOPin* pin){
	const TimerCompare* channel = compareFromIOPin(pin);
	if(channel){
		compareDetach(channel);
		compareSetOutputMode(channel, CHANNEL_MODE_DISCONNECT);
	}
}

boolean pwmInitHertz(const IOPin* pin, uint32_t hertz, DUTY_CYCLE duty, uint32_t* actualHertz){
	uint32_t deciHertz;
	deciHertz = hertz * 10;
	// Check for overflow
	if(deciHertz < hertz){
		deciHertz = PWM_FASTEST;
	}

	// Now do it
	boolean valid = pwmInitDeciHertz(pin, deciHertz, duty, actualHertz);

	if(valid==TRUE && actualHertz!=null){
		*actualHertz = *actualHertz /10;
	}
	return valid;
}

// Set up PWM on the given pin
boolean pwmInitDeciHertz(const IOPin* pin, uint32_t deciHertz, DUTY_CYCLE duty, uint32_t* actualDeciHertz){
	boolean rtn = FALSE;
	const TimerCompare* channel = compareFromIOPin(pin);

	if(channel==null){
		setError(PWM_PIN_NOT_AVAILABLE);
	}else{
		// The pin is valid
		if(compareIsInUse(channel)){
			setError(PWM_PIN_IN_USE);
		}else{
			// The pin is valid and available
			TIMER_MODE mode;
			uint16_t icr;
			uint16_t prescaler;

			const Timer* timer = compareGetTimer(channel);

			// Find the best PWM setting
			boolean valid = timerCalcPwm(timer, deciHertz, 100, &mode, &icr, &prescaler);

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

				// Mark the channel as in use
				compareAttach(channel,&nullTimerCompareCallback,0,null);

				// Turn the pin into an output, low
				pin_make_output(pin, FALSE);

				// Turn on the PWM pin output
				compareSetOutputMode(channel, CHANNEL_MODE_NON_INVERTING);

				// Turn on the timer
				timerSetPrescaler(timer,prescaler);

				// Set the initial duty cycle
				pwmSetDutyCycle(pin,duty);

				// Set the return value
				if(actualDeciHertz){
					*actualDeciHertz = TimerGetPwmDeciHertz(timer);
				}

				rtn = TRUE;
			}

		}
	}
	return rtn;
}



