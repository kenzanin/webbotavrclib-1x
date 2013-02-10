
/*
 * $Id: TPA81.c,v 1.4 2010/10/01 13:16:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: TPA81.c,v $
 * Revision 1.4  2010/10/01 13:16:25  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:46:29  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 19:10:52  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/03/20 00:46:48  clivewebster
 * *** empty log message ***
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
 * TPA81.c
 *
 *  Created on: 19 Mar 2010
 *      Author: Clive Webster
 */
#include "TPA81.h"


static void _read(SENSOR* sensor){
	Devantech_TPA81* device = (Devantech_TPA81*)sensor;
	uint8_t response[10];

	const I2C_DEVICE* i2c = &(device->i2cInfo);

	// Get registers 0 to 9
	if(i2cMasterReadRegisters(i2c,0,sizeof(response),response)){
		device->version = response[0];
		device->temperature.celsius = response[1];
		device->sensor[0] = response[2];
		device->sensor[1] = response[3];
		device->sensor[2] = response[4];
		device->sensor[3] = response[5];
		device->sensor[4] = response[6];
		device->sensor[5] = response[7];
		device->sensor[6] = response[8];
		device->sensor[7] = response[9];
	}
}

// 40ms between reads
TEMPERATURE_CLASS c_Devantech_TPA81 = MAKE_TEMPERATURE_CLASS(null,&_read, 0 ,40 );

