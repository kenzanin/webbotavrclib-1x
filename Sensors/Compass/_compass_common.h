/*
 * $Id: _compass_common.h,v 1.4 2010/06/14 18:53:23 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _compass_common.h,v $
 * Revision 1.4  2010/06/14 18:53:23  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 18:20:31  clivewebster
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
 *
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Defines the common interface for all compasses
 *
 */

#ifndef COMPASS_H_
#define COMPASS_H_
#include "../_sensor_common.h"

// Define the data type used by all compasses
// Changing it will mean changing compassDump
#define COMPASS_TYPE int16_t

// Define forward references
struct s_compasss;

// Define the api for the class
typedef struct PROGMEM c_compass{
	SENSOR_CLASS sensor;						// include parent class
} COMPASS_CLASS;

#define MAKE_COMPASS_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each c ompass sensor
typedef struct s_compass{
	SENSOR			sensor;							// Include fields from parent type
	COMPASS_TYPE	bearingDegrees;					// Last read value for the bearing, unsigned
	COMPASS_TYPE	rollDegrees;					// Last read value for the roll, signed
	COMPASS_TYPE	pitchDegrees;					// Last read value for the pitch, signed
} COMPASS;

#define MAKE_COMPASS_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),(COMPASS_TYPE)0,(COMPASS_TYPE)0,(COMPASS_TYPE)0}

// Create macros to make the virtual methods easier to call
#define compassRead(device) _sensorRead_(&device.compass.sensor)
#define compassInit(device) _sensorInit_(&device.compass.sensor)

// Dump the value of a sensor to the rprintf destination
void _compassDump(const COMPASS* device);
#define compassDump(device) _compassDump(&device.compass);


#endif /* COMPASS_H_ */

