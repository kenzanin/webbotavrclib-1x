/*
 * $Id: SRF02_Sonar.c,v 1.4 2010/10/01 13:16:26 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: SRF02_Sonar.c,v $
 * Revision 1.4  2010/10/01 13:16:26  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:46:30  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 19:00:14  clivewebster
 * Add copyright license info
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
 *
 * SRF02_Sonar.c
 *
 *  Created on: 15-May-2010
 *      Author: Clive Webster
 *
 *
 */
#include "SRF02_Sonar.h"


// Start a new reading
static boolean startReading(const Devantech_SRF02* device){
	boolean rtn = false;
	if(device){
		const I2C_DEVICE* i2c = &(device->i2cInfo);
		rtn = i2cMasterWriteRegister(i2c, 0, 0x51); // Send command to register 0
	}
	return rtn;
}

static void getReading(const Devantech_SRF02* device, DISTANCE_TYPE* rtn){
	if(device){
		uint8_t response[4];
		const I2C_DEVICE* i2c = &(device->i2cInfo);

		// Read registers 0,1,2,3
		while(i2cMasterReadRegisters(i2c, 0, sizeof(response), response)){
			if(response[0]!=255){
				// Ranging has finished
				DISTANCE_TYPE val = response[2];	// get high byte
				val = (val << 8) | response[3];		// put in low byte
				*rtn = val;
				break;
			}
		}
	}
}

// Read all the values and store into the device
static void __srf02_read(SENSOR* sensor){
	Devantech_SRF02* device = (Devantech_SRF02*)sensor;

	if(startReading(device)){
		getReading(device,&device->distance.cm);
	}
}

// Requires a 70ms delay between readings
DISTANCE_CLASS const c_Devantech_SRF02 = MAKE_DISTANCE_CLASS(null,&__srf02_read,0,70);

