/*
 * $Id: _encoder_dump.c,v 1.2 2010/09/08 18:25:32 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _encoder_dump.c,v $
 * Revision 1.2  2010/09/08 18:25:32  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.1  2010/08/14 21:04:02  clivewebster
 * Store encoder timings
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
 *        File: _encoder_dump.c
 *  Created on: 13 Aug 2010
 *      Author: Clive Webster
 */

// Allow printing of '%lu'
#define RPRINTF_COMPLEX

#include "_encoder_common.h"
#include "../../rprintf.h"

void _encoderDump(const ENCODER	* device){
	rprintf("(ticks=%d", device->value);
	if(device->interpolate){
		rprintf(", since last=%lu, last duration=%lu",
			device->timeSinceLastTick,device->durationOfLastTick);
	}
	rprintfChar(')');

}
