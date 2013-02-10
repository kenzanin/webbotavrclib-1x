/*
 * $Id: _current_common.h,v 1.4 2010/06/14 18:57:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _current_common.h,v $
 * Revision 1.4  2010/06/14 18:57:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 18:23:50  clivewebster
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
 *  Defines the common interface for all current sensors
 *
 */

#ifndef CURRENT_H_
#define CURRENT_H_
#include "../_sensor_common.h"

// Define the data type used by all sensors
// Changing it will mean changing currentDump
#define CURRENT_TYPE uint16_t

// Define forward references
struct s_current;

// Define the api for the class
typedef struct PROGMEM c_current{
	SENSOR_CLASS sensor;						// include parent class
} CURRENT_CLASS;

#define MAKE_CURRENT_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each current sensor
typedef struct s_current{
	SENSOR			sensor;						// Include fields from parent type
	CURRENT_TYPE	amps;						// Last read value for the current
} CURRENT;

#define MAKE_CURRENT_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),(CURRENT_TYPE)0}

// Create macros to make the virtual methods easier to call
#define currentRead(device) _sensorRead_(&device.current.sensor)
#define currentInit(device) _sensorInit_(&device.current.sensor)

// Dump the value of a sensor to the rprintf destination
void _currentDump(const CURRENT* device);
#define currentDump(device) _currentDump(&device.current);


#endif /* COMPASS_H_ */

