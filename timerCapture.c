/*
 * $Id: timerCapture.c,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timerCapture.c,v $
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
 *
 * timerCapture.c
 *
 *  Created on: 25-Aug-2009
 *      Author: Clive Webster
 */

#include "timer.h"

void timerCaptureAttach(const Timer* timer, TimerCallback callback, void* user_data, boolean risingEdge )
{
	TimerData* data = timerGetData(timer);
	if(data->capture_callback != null){
		setError(TIMER_CAPTURE_CALLBACK_EXISTS);
	}

	// Turn off interrupts whilst changing
	// in case an interrupt happens half way through
	CRITICAL_SECTION_START;

	// Set callback
	data->capture_callback = callback;
	data->capture_data = user_data;

	// Set the rising falling edge
	if(risingEdge){
		__portMaskSet(&timer->pgm_captureedge);
	}else{
		__portMaskClear(&timer->pgm_captureedge);
	}

	// Clear any pending capture interrupt flag
	timerCaptureClearInterruptPending(timer);

	// enable interrupt
	__portMaskSet(&timer->pgm_captureint);

	CRITICAL_SECTION_END;
}

void timerCaptureDetach(const Timer* timer){
	TimerData* data = timerGetData(timer);

	// stop interrupt
	__portMaskClear(&timer->pgm_captureint);

	// Clear any pending capture interrupt flag
	timerCaptureClearInterruptPending(timer);

	// Set callback
	data->capture_callback = null;
}

