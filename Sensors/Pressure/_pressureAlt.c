/*
 * $Id: _pressureAlt.c,v 1.1 2011/08/13 11:29:35 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _pressureAlt.c,v $
 * Revision 1.1  2011/08/13 11:29:35  clivewebster
 * Added
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
 *        File: _pressureAlt.c
 *  Created on: 11 Aug 2011
 *      Author: Clive Webster
 */
#include "_pressure_common.h"
#include <math.h>

// Convert Pa to altitude in meters
static double aboveSeaLevel(PRESSURE_TYPE val){
	double temp = (double) val / 101325.0;
	temp = 1 - pow(temp, 1.0/5.255);
	return 44330 * temp;
}
// Get the current altitude (in air) in 'm above sea level'
double _pressureAltitude(const PRESSURE* device){
	return aboveSeaLevel(device->pa);
}

// Get the current altitude (in air) in 'm above start point'
double _pressureRelativeAltitude(const PRESSURE* device){
	return aboveSeaLevel(device->pa) - aboveSeaLevel(device->paStart);
}
