/*
 * $Id: tone.c,v 1.1 2011/07/05 23:40:46 clivewebster Exp $
 * Revision History
 * ================
 * $Log: tone.c,v $
 * Revision 1.1  2011/07/05 23:40:46  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.1  2010/10/09 17:19:32  clivewebster
 * *** empty log message ***
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
 *        File: tone.c
 *  Created on: 8 Oct 2010
 *      Author: Clive Webster
 */

#include "tone.h"
#include "../errors.h"
#include "../timer.h"

// Initialise the tone library
void toneInit(TONE_PLAYER* player){
	if(!player->initialised){
		const Timer* timer = player->timer;

		pin_make_output(player->pin, FALSE);

		// Make sure the timer is not in use and can support CTC OCR mode
		if(!timerIsInUse(timer)){
			if(timerIsModeSupported(timer,TIMER_MODE_CTC_OCR)){
				const TimerCompare* compare = timerGetCompare(timer,0);
				if(compare != null && !compareIsInUse(compare)){
					// It is ok to use

			        // Turn off the PWM pin output
					compareSetOutputMode(compare, CHANNEL_MODE_DISCONNECT);

					// Mark the timer as in use for now
					timerSetMode(timer, TIMER_MODE_CTC_OCR);
			        timerSetPrescaler(timer, 1);

					player->initialised = TRUE;
				}else{
					setError(TIMER_COMPARE_CALLBACK_EXISTS);
				}
			}else{
				setError(TIMER_MODE_NOT_SUPPORTED);
			}
		}else{
			setError(PWM_TIMER_IN_USE);
		}
	}
}

static void pauser(const TimerCompare *timer_compare, void* data){
	TONE_PLAYER* player = data;

	// Reduce the duration unless its forever
	if(player->duration){
		player->duration--;
		if(player->duration==0){
			INTERRUPTABLE_SECTION{
				toneStop(player);
			}
		}
	}
}

static void toggle(const TimerCompare *timer_compare, void* data){
	TONE_PLAYER* player = data;

	pin_toggle(player->pin);

	pauser(timer_compare,data);
}

// Start playing a note
void tonePlay(TONE_PLAYER* player, uint16_t frequency, uint32_t durationMS ){
	if(player && player->initialised){

		// Find maximum value for the timer counter
		uint32_t maxval;
		if(timerIs16bit(player->timer)){
			maxval = 0xffffU;
		}else{
			maxval = 0xffU;
		}

		boolean pause = FALSE;
		if(frequency == 0){
			pause = TRUE;
			frequency = 440;
		}

		for(uint16_t prescale=1; prescale<=MAX_PRESCALE; prescale<<=1){
			int inx = __timerPrescalerIndex(player->timer, prescale);
			if(inx!=-1){

				uint32_t ocr = cpu_speed / prescale;
				ocr /= frequency;
				ocr /= 2;
				ocr--;

	            if(ocr <= maxval){
	            	// Found the best value

	            	// Calculate the toggle count
	                if (durationMS > 0){
	                  player->duration = 2 * frequency * durationMS / 1000;
	                  if(player->duration==0){
	                	  player->duration = 1;
	                  }
	                }else{
	                  player->duration = 0;
	                }

	                // Start the sound playing
	        		player->playing = TRUE;
	                timerSetPrescaler(player->timer,prescale);
//	                timerOverflowAttach(timer,toggle,player);
					const TimerCompare* compare = timerGetCompare(player->timer,0);
					if(pause){
						compareAttach(compare,&pauser,ocr,player);
					}else{
						compareAttach(compare,&toggle,ocr,player);
					}
	                return;
	            }
			}
		}
	}
}

// Stop playing a tone
void toneStop(TONE_PLAYER* player){
	if(player->initialised){
		const TimerCompare* compare = timerGetCompare(player->timer,0);
		compareDetach(compare);
//		timerOverflowDetach(player->timer);
		pin_low(player->pin);
		player->playing = FALSE;
		if(player->callback){
			ToneCallback cb = player->callback;
			cb(player);
		}
	}
}

