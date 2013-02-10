/*
 * $Id: _gps_common.h,v 1.1 2011/07/05 23:53:05 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _gps_common.h,v $
 * Revision 1.1  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
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
 *  Created on: 28-Apr-2011
 *      Author: Clive Webster
 *
 *  Defines the common interface for all gps sensors
 *
 */

#ifndef GPS_COMMON_H_
#define GPS_COMMON_H_
#include "../_sensor_common.h"
#include "gps.h"
#include <math.h>

#ifdef __cplusplus
/* ===================== C Code ===============================================*/
extern "C" {
#endif

// Define forward references
struct s_gps_common;

// Define the api for the class
typedef struct /*PROGMEM*/ c_gps{
	SENSOR_CLASS sensor;						// include parent class
} GPS_CLASS;

#define MAKE_GPS_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each gps sensor
typedef struct s_gps_common{
	SENSOR	sensor;				// Include fields from parent type
	GPS		info;				// The values returned by the last read of the sensor
	GPS 	work;				// The values being changed under interrupts
} GPS_COMMON;

#define MAKE_GPS_SENSOR(class) {MAKE_SENSOR(&(class.sensor)),MAKE_GPS(), MAKE_GPS()}

// Create macros to make the virtual methods easier to call
#define gpsRead(device) _sensorRead_(&device.gps.sensor)
#define gpsInit(device) _sensorInit_(&device.gps.sensor)

// Dump the value of a sensor to the rprintf destination
void _gpsDump(const GPS_COMMON* device);
#define gpsDump(device) _gpsDump(&device.gps);


#if defined(BUILDING_LIBRARY)
void __gps_read(SENSOR* sensor);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GPS_COMMON_H_ */

