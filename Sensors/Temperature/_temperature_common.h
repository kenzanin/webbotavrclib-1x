/*
 * $Id: _temperature_common.h,v 1.4 2010/06/14 19:10:52 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _temperature_common.h,v $
 * Revision 1.4  2010/06/14 19:10:52  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 18:43:05  clivewebster
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
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Defines the common interface for all temperature sensors
 *
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_
#include "../_sensor_common.h"

// Define the data type used by all sensors
// Changing this will mean changing temperatureDump
#define TEMPERATURE_TYPE int16_t

// Define forward references
struct s_temperature;

// Define the api for the class
typedef struct PROGMEM c_temperature{
	SENSOR_CLASS sensor;						// include parent class
} TEMPERATURE_CLASS;

#define MAKE_TEMPERATURE_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each sensor
typedef struct s_temperature{
	SENSOR				sensor;						// Include fields from parent type
	TEMPERATURE_TYPE	celsius;					// Last read value
} TEMPERATURE;

#define MAKE_TEMPERATURE_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),(TEMPERATURE_TYPE)0}

// Create macros to make the virtual methods easier to call
#define temperatureRead(device) _sensorRead_(&device.temperature.sensor)
#define temperatureInit(device) _sensorInit_(&device.temperature.sensor)

// Dump the value of a sensor to the rprintf destination
void _temperatureDump(const TEMPERATURE* device);
#define temperatureDump(device) _temperatureDump(&device.temperature);

#endif /* TEMPERATURE_H_ */

