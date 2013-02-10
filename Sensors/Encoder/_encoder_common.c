/*
 * $Id: _encoder_common.c,v 1.7 2010/10/06 12:14:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _encoder_common.c,v $
 * Revision 1.7  2010/10/06 12:14:48  clivewebster
 * Optimise encoder_read if there is no interpolation
 *
 * Revision 1.6  2010/09/08 18:25:32  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.5  2010/08/14 21:04:02  clivewebster
 * Store encoder timings
 *
 * Revision 1.4  2010/06/14 19:05:35  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/06/13 18:14:51  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/11/02 18:36:19  clivewebster
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
 * _encoder_common.c
 *
 *  Created on: 25-Sep-2009
 *      Author: Clive Webster
 */
#include "_encoder_common.h"
#include "../../timer.h"


void _encoderSubtract(ENCODER* encoder, ENCODER_TYPE count){
	if(encoder->inverted){
		count*=-1;
	}
	CRITICAL_SECTION{
		encoder->counter -= count;
	}
}

void __encoder_read(SENSOR* sensor){
	ENCODER_TYPE ticks;
	ENCODER* encoder = (ENCODER*)sensor;
	TIMER_SNAPSHOT lastTime;
	TIMER_SNAPSHOT prevTime;

	// Disable interrupts whilst reading counter.
	CRITICAL_SECTION{
		ticks = encoder->counter;
		if(encoder->interpolate){
			lastTime = encoder->snapshot[encoder->tickIndex];
			prevTime = encoder->snapshot[encoder->tickIndex ^ 1];
		}
	}

	if(encoder->interpolate){
		TICK_COUNT lastTick,prevTick,now;
		now = clockGetus();
		lastTick = timerSnapshotToTicks(&lastTime);
		prevTick = timerSnapshotToTicks(&prevTime);
		encoder->timeSinceLastTick  = now - lastTick;       // us since last tick
		encoder->durationOfLastTick = lastTick - prevTick;  // us between last two ticks (ie speed)
	}


	if(encoder->inverted){
		ticks *= -1;
	}

	// Store the last read value
	encoder->value=ticks;
}

// Another tick has happened
// so store the time of day
void _encoder_tick(ENCODER* encoder){
	// Toggle between 0 and 1
	encoder->tickIndex ^= 1;
	clockGetSnapshot(&encoder->snapshot[encoder->tickIndex]);
}

// Return how many ticks there are per revolution
uint16_t _encoderTicksPerRevolution(const ENCODER* encoder){
	uint16_t rtn = encoder->numStripes;
	return rtn * 2;
}
