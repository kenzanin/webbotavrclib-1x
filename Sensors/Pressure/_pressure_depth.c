/*
 * $Id: _pressure_depth.c,v 1.1 2011/08/13 11:29:35 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _pressure_depth.c,v $
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
 *        File: _pressure_depth.h
 *  Created on: 11 Aug 2011
 *      Author: Clive Webster
 */

#include "_pressure_common.h"

// Get the depth (in water) in 'm below start point'
// an increase of 1 decibar occurs for every 1.019716 metre
// i decibar = 10kPa = 10,000 Pa
double _pressureRelativeDepth(const PRESSURE* device){
	PRESSURE_TYPE diff = device->pa - device->paStart;
	double dbar = (double)diff / 10000.0;
	return dbar * 1.019716;
}
