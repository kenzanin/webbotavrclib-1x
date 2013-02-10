/*
 * $Id: BMP085.h,v 1.1 2011/08/13 11:28:35 clivewebster Exp $
 * Revision History
 * ================
 * $Log: BMP085.h,v $
 * Revision 1.1  2011/08/13 11:28:35  clivewebster
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
 *        File: BMP085.h
 *  Created on: 12 Aug 2011
 *      Author: Clive Webster
 */

#ifndef BMP085_H_
#define BMP085_H_

#include "../_pressure_common.h"
#include "../../../i2cBus.h"
#include "../../Temperature/_temperature_common.h"

// The class used to read a raw value
extern const PRESSURE_CLASS c_BMP085;

typedef struct {
   short ac1;
   short ac2;
   short ac3;
   unsigned short ac4;
   unsigned short ac5;
   unsigned short ac6;
   short b1;
   short b2;
   short mb;
   short mc;
   short md;
} bmp085_calibration_t;

typedef struct s_BMP085{
	PRESSURE	 		pressure;
	I2C_DEVICE	  		i2cInfo;
	uint8_t  			oss;		// Over sampling setting
	TEMPERATURE_TYPE	celsius;	// Last read temperature
	boolean				present;	// Is the device responding

	bmp085_calibration_t cal_param; // Calibration values
	long 				param_b5;	// Used during calculations
} BMP085;

#define MAKE_BMP085(oss) { \
	MAKE_PRESSURE_SENSOR(c_BMP085), \
	MAKE_I2C_DEVICE(0xee), \
	oss & 3, \
	0,	\
	FALSE \
	}

#endif /* BMP085_H_ */
