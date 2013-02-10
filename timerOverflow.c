/*
 * $Id: timerOverflow.c,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timerOverflow.c,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 19:07:05  clivewebster
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
 * timerOverflow.c
 *
 *  Created on: 25-Aug-2009
 *      Author: Clive Webster
 */
#include "timer.h"
/* -------------------------------------------------------
//
//  Attach a callback function to a timer when it overflows
//
//  This function is called during an interrupt so should
//  be quick.
//
------------------------------------------------------- */
void timerOverflowAttach(const Timer* timer, TimerCallback callback, void* user_data )
{
	// Turn off interrupts whilst changing
	// in case an interrupt happens half way through
	CRITICAL_SECTION_START;
	if(callback!=null && timerGetData(timer)->overflow_callback != null){
		setError(TIMER_OVERFLOW_CALLBACK_EXISTS);
	}

	// Set callback
	TimerData* data = timerGetData(timer);
	data->overflow_callback = callback;
	data->overflow_data = user_data;

	// enable interrupt if there is callback
	if(callback!=null){
		// enable overflow interrupts
		__portMaskSet(&timer->pgm_overflowint);
	}else{
		// Disable overflow interrupts
		__portMaskClear(&timer->pgm_overflowint);
	}

	CRITICAL_SECTION_END;
}

