/*
 * $Id: _pressure_common.h,v 1.4 2011/08/13 11:30:01 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _pressure_common.h,v $
 * Revision 1.4  2011/08/13 11:30:01  clivewebster
 * Return values in Pa rather than kPa
 *
 * Revision 1.3  2010/06/14 19:09:50  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:41:41  clivewebster
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
 *  Created on: 24-Sep-2009
 *      Author: Clive Webster
 *
 *  Defines the common interface for all pressure sensors
 *  Returns a value in kPa
 *  As of Version 1.34 now returns value in Pa
 *
 */

#ifndef PRESSURE_H_
#define PRESSURE_H_
#include "../_sensor_common.h"

// Define the data type used by all sensors
// Changing this will mean changing pressureDump
#define PRESSURE_TYPE int32_t

// Define forward references
struct s_pressure;

// Define the api for the class
typedef struct PROGMEM c_pressure{
	SENSOR_CLASS sensor;						// include parent class
} PRESSURE_CLASS;

#define MAKE_PRESSURE_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each pressure sensor
typedef struct s_pressure{
	SENSOR			sensor;						// Include fields from parent type
	PRESSURE_TYPE	pa;							// Last read value
	PRESSURE_TYPE	paStart;					// The value at power on
} PRESSURE;

#define MAKE_PRESSURE_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),(PRESSURE_TYPE)0}

// Create macros to make the virtual methods easier to call
#define pressureRead(device) _sensorRead_(&device.pressure.sensor)
#define pressureInit(device) _sensorInit_(&device.pressure.sensor)

// Dump the value of a sensor to the rprintf destination
void _pressureDump(const PRESSURE* device);
#define pressureDump(device) _pressureDump(&device.pressure);

// Get the depth (in water) in 'm below start point'
double _pressureRelativeDepth(const PRESSURE* device);
#define pressureRelativeDepth(device) _pressureRelativeDepth(&device.pressure);

// Get the current altitude (in air) in 'm above sea level'
double _pressureAltitude(const PRESSURE* device);
#define pressureAltitude(device) _pressureAltitude(device.pressure);

// Get the current altitude (in air) in 'm above start point'
double _pressureRelativeAltitude(const PRESSURE* device);
#define pressureRelativeAltitude(device) _pressureRelativeAltitude(device.pressure);

#endif /* PRESSURE_H_ */

