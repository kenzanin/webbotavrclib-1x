/*
 * $Id: _acceleration_common.h,v 1.6 2010/06/14 18:50:17 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _acceleration_common.h,v $
 * Revision 1.6  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/11/02 18:18:28  clivewebster
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
 * acceleration.h
 *
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Defines the common interface for all acelerometers
 *
 *  In order to avoid the use of floating point maths then
 *  the values are in 'thousands of a G' ie 1000 = 1G
 *
 */

#ifndef ACCELERATION_H_
#define ACCELERATION_H_
#include "../_sensor_common.h"

// Define the data type used by all accelerators
// Changing this value will require changes to accelerometerDump
#define ACCEL_TYPE int16_t

// Define forward references
struct s_accelerometer;

// Define the api for the class
typedef struct PROGMEM c_accelerometer{
	SENSOR_CLASS sensor;						// include parent class
} ACCELEROMETER_CLASS;

#define MAKE_ACCELEROMETER_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each accelerometer
typedef struct s_accelerometer{
	SENSOR		sensor;							// Include fields from parent type
	ACCEL_TYPE	x_axis_mG;						// Last read value for the x axis
	ACCEL_TYPE	y_axis_mG; 						// Last read value for the y axis
	ACCEL_TYPE	z_axis_mG; 						// Last read value for the z axis
	ACCEL_TYPE	x_calib_min,x_calib_max;		// x calibration
	ACCEL_TYPE	y_calib_min,y_calib_max;		// y calibration
	ACCEL_TYPE	z_calib_min,z_calib_max;		// z calibration
} ACCELEROMETER;

#define MAKE_ACCELEROMETER_SENSOR(class) { \
	MAKE_SENSOR(&(class.sensor)),\
	(ACCEL_TYPE)0, (ACCEL_TYPE)0,(ACCEL_TYPE)0, \
	(ACCEL_TYPE)-1000, (ACCEL_TYPE)1000,	\
	(ACCEL_TYPE)-1000, (ACCEL_TYPE)1000,	\
	(ACCEL_TYPE)-1000, (ACCEL_TYPE)1000 	\
	}

// Create macros to make the virtual methods easier to call
#define accelerometerRead(device) _sensorRead_(&device.accelerometer.sensor)
#define accelerometerInit(device) _sensorInit_(&device.accelerometer.sensor)

// Dump the value of a sensor to the rprintf destination
void _accelerometerDump(const ACCELEROMETER* device);
#define accelerometerDump(device) _accelerometerDump(&device.accelerometer);

// Calibration routines
void __accelerometerCalibrateX(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue);
void __accelerometerCalibrateY(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue);
void __accelerometerCalibrateZ(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue);
#define accelerometerCalibrateX(device, min,max) __accelerometerCalibrateX(&device.accelerometer,min,max)
#define accelerometerCalibrateY(device, min,max) __accelerometerCalibrateY(&device.accelerometer,min,max)
#define accelerometerCalibrateZ(device, min,max) __accelerometerCalibrateZ(&device.accelerometer,min,max)

#if defined(BUILDING_LIBRARY)
// private routines
void	__accelerometerSetX(ACCELEROMETER* device, ACCEL_TYPE x);
void	__accelerometerSetY(ACCELEROMETER* device, ACCEL_TYPE y);
void	__accelerometerSetZ(ACCELEROMETER* device, ACCEL_TYPE z);
#endif

#endif /* ACCELERATION_H_ */

