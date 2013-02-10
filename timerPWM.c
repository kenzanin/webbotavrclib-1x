
/*
 * $Id: timerPWM.c,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timerPWM.c,v $
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2009/10/26 18:53:52  clivewebster
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
 * timerPWM.c
 *
 *  Created on: 23-Oct-2009
 *      Author: Clive Webster
 *
 *  Routines to help work out PWM settings
 *
 */
#include "timer.h"


// Calc the frequency of a PWM given various settings
// Note that no checks are performed as to whether timers are 16 bit or not
// Return the value in 10'ths of a hertz ie deciHertz
static uint32_t __timerPWMGetDeciHertz(TIMER_MODE mode,uint16_t prescaler,uint16_t icr){
	uint32_t deciHertz=0;
	uint16_t top = icr;
	uint32_t cpuDivPrescale = cpu_speed * 10 / prescaler;

	switch(mode){
	case TIMER_MODE_PWM8_PHASE_CORRECT:
		top=0xffU;
		goto PhaseCorrect;
	case TIMER_MODE_PWM9_PHASE_CORRECT:
		top=0x1ffU;
		goto PhaseCorrect;
	case TIMER_MODE_PWM10_PHASE_CORRECT:
		top=0x3ffU;
		goto PhaseCorrect;
	case TIMER_MODE_PWM_PHASE_FREQ_ICR:
	case TIMER_MODE_PWM_PHASE_CORRECT_ICR:
		PhaseCorrect:
		// freq      = clock / (prescaler * 2 * top);
		deciHertz = cpuDivPrescale;
		deciHertz /= top;
		deciHertz /= 2;
		break;


	case TIMER_MODE_PWM8_FAST:
		top=0xffU;
		goto Fast;
	case TIMER_MODE_PWM9_FAST:
		top=0x1ffU;
		goto Fast;
	case TIMER_MODE_PWM10_FAST:
		top=0x3ffU;
		goto Fast;
	case TIMER_MODE_PWM_FAST_ICR:
		Fast:
		// freq = clock / ( prescaler * (1+top) );
		deciHertz = cpuDivPrescale;
		deciHertz /= (((uint32_t)(top))+1UL);
	default:
		break;

	}
	return deciHertz;
}

uint32_t TimerGetPwmDeciHertz(const Timer* timer){
	uint32_t rtn = 0;
	if(timerIsInUse(timer)){
		TIMER_MODE mode = timerGetMode(timer);
		uint16_t prescale = timerGetPrescaler(timer);
		uint16_t icr;
		if(modeIsICR(mode)){
			PORT icrPort = pgm_read_word(&timer->pgm_icr);
			icr = _SFR_MEM16(icrPort);
		}else{
			icr = 0;
		}
		rtn =  __timerPWMGetDeciHertz(mode,prescale,icr);
	}
	return rtn;
}

boolean timerCalcPwm(const Timer* timer, uint32_t deciHertz, uint16_t steps, TIMER_MODE *modeRtn, uint16_t* icrRtn, uint16_t* prescaleRtn){
	TIMER_MODE min = 0;
	TIMER_MODE max = MAX_MODES-1;
	boolean rtn = FALSE;	// Default return is that it cannot be set

	// Only try the current setting if the timer is in use
	if(timerIsInUse(timer)){
		min = max = timerGetMode(timer);
	}

	uint32_t clockDiv2 = (cpu_speed >> 1);

	uint32_t bestError=0;
	TIMER_MODE bestMode=0;
	uint16_t bestPrescaler=0;
	uint16_t bestICR=0;

	for(TIMER_MODE mode = min ; mode <= max; mode++){
		// Try the next mode
		if(modeIsPWM(mode) && timerIsModeSupported(timer,mode)){
			// The timer supports this mode
			uint16_t top;
			uint32_t exactPrescaler;
			boolean usable=FALSE;

			switch(mode){
			case TIMER_MODE_PWM8_PHASE_CORRECT:
				top=0xffU;
				goto PhaseCorrect;
			case TIMER_MODE_PWM9_PHASE_CORRECT:
				top=0x1ffU;
				goto PhaseCorrect;
			case TIMER_MODE_PWM10_PHASE_CORRECT:
				top=0x3ffU;
				goto PhaseCorrect;
			case TIMER_MODE_PWM_PHASE_FREQ_ICR:
			case TIMER_MODE_PWM_PHASE_CORRECT_ICR:
				// freq      = clock / (prescaler * 2 * top);
				// prescaler = clock / (frequency * 2 * top);
				top = 0xffffU;
				PhaseCorrect:
				exactPrescaler = clockDiv2 * 10;
				if(deciHertz){
					exactPrescaler /= deciHertz;
				}
				exactPrescaler /= top;
				goto Usable;

			case TIMER_MODE_PWM8_FAST:
				top=0xffU;
				goto Fast;
			case TIMER_MODE_PWM9_FAST:
				top=0x1ffU;
				goto Fast;
			case TIMER_MODE_PWM10_FAST:
				top=0x3ffU;
				goto Fast;
			case TIMER_MODE_PWM_FAST_ICR:
				top = 0xffffU;
				Fast:
				// freq      = clock / ( prescaler * (1+top) );
				// prescaler = clock / ( freq * (1+top) )
				exactPrescaler = cpu_speed * 10; // convert deciHertz to Hertz
				// avoid div by 0
				if(deciHertz){
					exactPrescaler /= deciHertz;
				}
				exactPrescaler /= ( ((uint32_t)(top)) + 1UL );
				Usable:
				usable = TRUE;
				break;

			default:
				break;
			}

			if(usable){
				// Make sure the prescaler is in valid bounds - even tho value may not be available
				uint16_t prescaler = CLAMP(exactPrescaler, 1, MAX_PRESCALE);
				uint16_t icr = 0;
				uint32_t tempTop=0;

				// Make the prescaler a valid value
				prescaler = timerGetClosestPrescale(timer, prescaler);

				if(deciHertz==PWM_SLOWEST){
					prescaler = MAX_PRESCALE;
					icr = 0xffffU;
				}else if(deciHertz==PWM_FASTEST){
					prescaler = 1;
					icr = steps;		// At least the required number of steps
				}else{
					// Work out the value for top
					prescaler--;
					do{
						prescaler = timerGetClosestPrescale(timer, ++prescaler);

						// Find value for TOP
						switch(mode){
						case TIMER_MODE_PWM_PHASE_FREQ_ICR:
						case TIMER_MODE_PWM_PHASE_CORRECT_ICR:
							// freq = clock / (prescaler * 2 * top);
							// top  = clock / (prescaler * 2 * freq);
							tempTop = clockDiv2 * 10;
							tempTop /= deciHertz;
							tempTop /= prescaler;
							break;
						case TIMER_MODE_PWM_FAST_ICR:
							// freq = clock / ( prescaler * (1+top) );
							// top  = (clock / ( prescaler * freq)) - 1
							tempTop = cpu_speed * 10;
							tempTop /= deciHertz;
							tempTop /= prescaler;
							tempTop--;
							break;
						default:
							break;
						}

					}while(prescaler<MAX_PRESCALE && (tempTop &0xffff0000UL)!=0);
					icr = CLAMP(tempTop, steps, 0xffffu);
				}

				// Now calc the frequency it would achieve
				uint32_t actual = __timerPWMGetDeciHertz(mode,prescaler,icr);

				// Calc the absolute error
				uint32_t error = (actual > deciHertz) ? actual - deciHertz : deciHertz - actual;

				// Keep the one with the least error
				if(rtn==FALSE || error < bestError){
					bestError = error;
					rtn = TRUE;
					bestMode = mode;
					bestPrescaler = prescaler;
					bestICR = icr;
				}
			} // End if usable
		} // this mode cannot be used
	} // next mode

	// Return values
	if(rtn){
		if(modeRtn){
			*modeRtn = bestMode;
		}
		if(icrRtn){
			*icrRtn = bestICR;
		}
		if(prescaleRtn){
			*prescaleRtn = bestPrescaler;
		}
	}

	return rtn;
}
