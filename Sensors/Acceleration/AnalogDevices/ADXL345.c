/*
 * $Id: ADXL345.c,v 1.4 2010/10/01 13:16:25 clivewebster Exp $
 * Revision History
 * ================
 * $Log: ADXL345.c,v $
 * Revision 1.4  2010/10/01 13:16:25  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:46:26  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/07/19 19:51:24  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.1  2010/06/20 19:30:56  clivewebster
 * Added in Version 1.21
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
 *
 *        File: ADXL345.c
 *  Created on: 15 Jun 2010
 *      Author: Clive Webster
 *
 *  This uses FULL_RESOLUTION mode so that the values are consistent across g ranges.
 *  Multiple by 3.9 to convert values to 'mg'
 */
#include "ADXL345.h"

#include "../../../fraction.h"

// 3.9
static const uint16_t PROGMEM _frac[]     = {3, 2, 3, 15, 0};

static void _init(SENSOR* sensor){
	const ADXL345* accel = (const ADXL345*)sensor;
	const I2C_DEVICE* i2c = &(accel->i2cInfo);
	// Put into measurement mode
	i2cMasterWriteRegister(i2c, 0x2d, 0);
	i2cMasterWriteRegister(i2c, 0x2d, 8);

	// Put into Full Resolution mode
	i2cMasterWriteRegister(i2c, 0x31, 8);
}

static ACCEL_TYPE getMG(uint8_t lsb, uint8_t msb){
	int16_t val = (msb << 8) | lsb;
	ACCEL_TYPE rtn = fraction32(val, _frac);
	return rtn;
}

static void _read(SENSOR* sensor){
	uint8_t response[6];
	ADXL345* accel = (ADXL345*)sensor;
	const I2C_DEVICE* i2c = &(accel->i2cInfo);

	// Read the x,y,z registers
	if(i2cMasterReadRegisters(i2c,0x32,sizeof(response),response)){
		ACCEL_TYPE x,y,z;
		x = getMG(response[0],response[1]);
		y = getMG(response[2],response[3]);
		z = getMG(response[4],response[5]);
		__accelerometerSetX(&accel->accelerometer,x);
		__accelerometerSetY(&accel->accelerometer,y);
		__accelerometerSetZ(&accel->accelerometer,z);

	}

}

// Needs 11.1 ms to start up
ACCELEROMETER_CLASS const c_ADXL345 = MAKE_ACCELEROMETER_CLASS(&_init,&_read,12,0);
