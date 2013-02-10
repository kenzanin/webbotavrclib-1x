/*
 * $Id: _distance_common.h,v 1.4 2010/06/14 19:03:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _distance_common.h,v $
 * Revision 1.4  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 18:32:14  clivewebster
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
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Defines the common interface for all distance sensors
 *
 *
 */
#ifndef _distance_common_H_
#define _distance_common_H_

#include "../_sensor_common.h"

// Define the data type used by all distance sensors
// Changing this will mean changing distanceDump
#define DISTANCE_TYPE uint16_t

// Forward reference
struct s_distance;

// Define the api for the class
typedef struct PROGMEM c_distance{
	SENSOR_CLASS sensor;						// include parent class
} DISTANCE_CLASS;

#define MAKE_DISTANCE_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each distance sensor
typedef struct s_distance{
	SENSOR		sensor;							// Include fields from parent type
	DISTANCE_TYPE cm;						// Last read value in cm
} DISTANCE;
#define MAKE_DISTANCE_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),(DISTANCE_TYPE)0}

// Create macros to make the virtual methods easier to call
#define distanceRead(device) _sensorRead_(&device.distance.sensor)
#define distanceInit(device) _sensorInit_(&device.distance.sensor)

// Dump the value of a sensor to the rprintf destination
void _distanceDump(const DISTANCE* device);
#define distanceDump(device) _distanceDump(&device.distance);

#endif

