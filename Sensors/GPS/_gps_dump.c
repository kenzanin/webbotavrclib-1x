/*
 * $Id: _gps_dump.c,v 1.1 2011/07/05 23:53:05 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _gps_dump.c,v $
 * Revision 1.1  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: _gps_dump.c
 *  Created on: 28 Apr 2011
 *      Author: Clive Webster
 */
#include "_gps_common.h"
#include "../../core.h"
#include "../../rprintf.h"
void _gpsDump(const GPS_COMMON* device){
	const GPS* data = &device->info;
	rprintf("Valid:%c\n", (data->valid) ? 'Y' : 'N');
	rprintf("Satellites:%d\n",data->numSatellites);
	rprintf("\nLongitude(degrees):"); rprintfFloat(10,data->longitude);
	rprintf("\nLatitude(degrees):"); rprintfFloat(10,data->latitude);
	rprintf("\nTrack(degrees):"); rprintfFloat(10,data->track);
	rprintf("\nAltitude(m):"); rprintfFloat(10,data->altitude);
	rprintf("\nSpeed(knots):"); rprintfFloat(10,data->speed);
	rprintf("\nTime:"); rprintfFloat(10,data->fixTime);
	rprintf("\n");
}

