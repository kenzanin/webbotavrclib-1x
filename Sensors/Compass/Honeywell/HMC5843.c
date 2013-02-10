/*
 * $Id: HMC5843.c,v 1.4 2010/10/01 13:16:26 clivewebster Exp $
 * Revision History
 * ================
 * $Log: HMC5843.c,v $
 * Revision 1.4  2010/10/01 13:16:26  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:46:30  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/07/02 00:23:40  clivewebster
 * Return raw magetometer values and also calculate the roll and pitch angles
 *
 * Revision 1.1  2010/06/20 19:31:07  clivewebster
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
 *        File: HMC5843.c
 *  Created on: 20 Jun 2010
 *      Author: Clive Webster
 */

#include "HMC5843.h"

// Requires the floating point library
#include <math.h>

static void _init(SENSOR* sensor){
	// Put into continuous measurement mode
	if(sensor){
		HMC5843* device = (HMC5843*)sensor;
		const I2C_DEVICE* i2c = &(device->i2cInfo);
		if(i2cMasterWriteRegister(i2c, 2, 0)){
			device->compass.sensor.initialised = TRUE;
		}
	}
}

static void _read(SENSOR* sensor){
	HMC5843* device = (HMC5843*)sensor;
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	uint8_t response[6];

	// Read the x,y,z registers (MSB first, then LSB) registers 3....8
	if(i2cMasterReadRegisters(i2c,3,sizeof(response),response)){
		int16_t x = (response[0] << 8) | response[1];
		int16_t y = (response[2] << 8) | response[3];
//		int16_t z = (response[4] << 8) | response[4];
		int16_t z = (response[4] << 8) | response[5];
		device->rawX = x;
		device->rawY = y;
		device->rawZ = z;

		double heading = atan2((double)(x), (double)(-1 * y)) * (180.0 / M_PI);
		if(heading < 0)	heading += 360;
		device->compass.bearingDegrees = (COMPASS_TYPE)heading;

		heading = atan2((double)(x), (double)(-1 * z)) * (180.0 / M_PI);
		if(heading < 0)	heading += 360;
		device->compass.rollDegrees = (COMPASS_TYPE)heading;

		heading = atan2((double)(y), (double)(-1 * z)) * (180.0 / M_PI);
		if(heading < 0)	heading += 360;
		device->compass.pitchDegrees = (COMPASS_TYPE)heading;
	}
}

// Set the speed in bits 4,3,2 of configuration register A (register 00) (
static void _speed(HMC5843* compass, uint8_t speed){
	if(compass){
		uint8_t reg;
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		if(i2cMasterReadRegisters(i2c,0x00,1,&reg)){
			reg &= 0xE3;	// mask out other bits
			reg |= (speed << 2);
			i2cMasterWriteRegister(i2c,0x00,reg);

			// Initialise
			_init(&compass->compass.sensor);
		}
	}
}

void HMC5843_1Hz(HMC5843* compass){
	_speed(compass,1);
}
void HMC5843_2Hz(HMC5843* compass){
	_speed(compass,2);
}
void HMC5843_5Hz(HMC5843* compass){
	_speed(compass,3);
}
void HMC5843_10Hz(HMC5843* compass){
	_speed(compass,4);
}
void HMC5843_20Hz(HMC5843* compass){
	_speed(compass,5);
}
void HMC5843_50Hz(HMC5843* compass){
	_speed(compass,6);
}


// Requires a 100ms delay before being read for the first time
// The fastest speed is 50Hz ie every 20ms
COMPASS_CLASS const c_HMC5843 = MAKE_COMPASS_CLASS(&_init,&_read, 100 , 20 );
