/*
 * $Id: timerCompare.c,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timerCompare.c,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/06/07 18:34:37  clivewebster
 * Move nullTimerCompareCallback to timer.c
 *
 * Revision 1.4  2010/03/07 19:24:29  clivewebster
 * compareFromIOPin gives preference to 16 bit timers if it is used by multiple timer output channels
 *
 * Revision 1.3  2009/11/02 19:07:05  clivewebster
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
 * timerCompare.c
 *
 *  Created on: 25-Aug-2009
 *      Author: Clive Webster
 */
#include "timer.h"


boolean compareIsInUse(const TimerCompare* channel){
	TimerDataCompare* data = compareGetData(channel);

	// if it has a callback then it is in use
	if(data->compare_callback != null)
		return TRUE;

	// if it is channel A and the Timer is is a mode where TOP is set channel A then it is also
	// in use (for defining the Timer TOP value)
	if(timerGetCompare(compareGetTimer(channel),0)==channel){
		TIMER_MODE mode = timerGetMode(compareGetTimer(channel));
		if(mode==TIMER_MODE_CTC_OCR
				|| mode==TIMER_MODE_PWM_PHASE_FREQ_OCR
				|| mode==TIMER_MODE_PWM_PHASE_CORRECT_OCR
				|| mode==TIMER_MODE_PWM_FAST_OCR){
			return TRUE;
		}
	}

	return FALSE;
}

/* -------------------------------------------------------
//
//  Get the compare channel used by a given IOPin
//  May return null if this pin is not to do with a timer
//  or it does not exist on this system board
//  An IOPin may actually support more than one compare
// channel - in which case give priority to 16 bit timers
------------------------------------------------------- */
const TimerCompare* compareFromIOPin(const IOPin* pin){
	uint8_t t;
	uint8_t c;
	const TimerCompare* rtn=null;

	// Iterate over the timers
	for(t=0; t < NUMBER_OF_TIMERS; t++){
		const Timer * timer = &pgm_Timers[t];

		// Iterate over the compare units
		uint8_t numCompare = timerNumberOfCompareUnits(timer);
		for(c=0;c<numCompare;c++){
			const TimerCompare* channel = timerGetCompare(timer,c);
			const IOPin* thisPin = compareGetPin(channel);
			if(thisPin != null && thisPin==pin){
				rtn = channel;					// remember the channel
				if(timerIs16bit(timer)){
					return rtn;
				}
			}
		}
	}

	return rtn;
}



