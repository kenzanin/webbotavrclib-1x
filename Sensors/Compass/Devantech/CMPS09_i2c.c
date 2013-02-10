/*
 * $Id: CMPS09_i2c.c,v 1.4 2010/10/01 13:16:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: CMPS09_i2c.c,v $
 * Revision 1.4  2010/10/01 13:16:25  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:46:26  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 18:51:59  clivewebster
 * Add copyright license info
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
 * CMPS09_i2c.c
 *
 *  Created on: 16-May-2010
 *      Author: Clive Webster
 *
 *
 */
#include "CMPS09.h"


static void __CMPS09_read(SENSOR* sensor){
	CMPS09_I2C* compass = (CMPS09_I2C*)sensor;
	const I2C_DEVICE* i2c = &(compass->i2cInfo);

	// Read register 2 (high) and 3 (low) = bearing
	// register 4 = Pitch in degrees
	// register 5 = Roll in degrees
	uint8_t response[4];
	if(i2cMasterReadRegisters(i2c, 2, sizeof(response), response)){
		uint16_t bearing = response[0];
		bearing <<= 8;
		bearing |= response[1];
		bearing = (bearing + 5) / 10;	// round to nearest degree
		while(bearing>=360){
			bearing -= 360;
		}

		compass->compass.bearingDegrees = (COMPASS_TYPE)bearing;
		compass->compass.pitchDegrees = (COMPASS_TYPE)response[2];
		compass->compass.rollDegrees = (COMPASS_TYPE)response[3];
	}
}

// Maximum refresh rate is 33ms
COMPASS_CLASS c_CMPS09_i2c = MAKE_COMPASS_CLASS(null,&__CMPS09_read, 0 ,33 );
