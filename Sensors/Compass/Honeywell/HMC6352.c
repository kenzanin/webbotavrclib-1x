
/*
 * $Id: HMC6352.c,v 1.6 2010/10/01 13:16:26 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: HMC6352.c,v $
 * Revision 1.6  2010/10/01 13:16:26  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.5  2010/09/30 16:46:30  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.4  2010/06/14 18:53:23  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/02/04 22:59:38  clivewebster
 * Fixed init problem - cast of mismatched pointer
 *
 * Revision 1.2  2010/02/04 19:57:12  clivewebster
 * Add functions to change the refresh rate
 *
 * Revision 1.1  2010/01/30 21:21:18  clivewebster
 * Added in Version 1.14
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
 *
 * HMC6352.c
 *
 *  Created on: 29-Jan-2010
 *      Author: Clive Webster
 */
#include "HMC6352.h"
#include "../../../timer.h"

#define CMD_GET_HEADING 0x41
#define CMD_WRITE_RAM   0x47

typedef struct s_cmd_50{
	uint8_t heading_msb;		// 0
	uint8_t heading_lsb;		// 1
} HEADING_REPLY;

void HMC6352_1Hz(HMC6352* compass){
	// Put into continuous mode 1Hz

	// Post heading data
	if(compass){
		uint8_t cmd[] = {CMD_WRITE_RAM, 0x74, 0b00010010};
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		i2cMasterSend(i2c,sizeof(cmd),cmd);
	}
}

void HMC6352_5Hz(HMC6352* compass){
	// Put into continuous mode 5Hz

	// Post heading data
	if(compass){
		uint8_t cmd[] = {CMD_WRITE_RAM, 0x74, 0b00110010};
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		i2cMasterSend(i2c,sizeof(cmd),cmd);
	}
}
void HMC6352_10Hz(HMC6352* compass){
	// Put into continuous mode 10Hz

	// Post heading data
	if(compass){
		uint8_t cmd[] = {CMD_WRITE_RAM, 0x74, 0b01010010};
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		i2cMasterSend(i2c,sizeof(cmd),cmd);
	}
}
void HMC6352_20Hz(HMC6352* compass){
	// Put into continuous mode 20Hz

	// Post heading data
	if(compass){
		uint8_t cmd[] = {CMD_WRITE_RAM, 0x74, 0b01110010};
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		i2cMasterSend(i2c,sizeof(cmd),cmd);
	}
}

static void __HMC6352_init(SENSOR* sensor){
	HMC6352* compass = (HMC6352*)sensor;
	HMC6352_20Hz(compass);
}

static void __HMC6352_read(SENSOR* sensor){
	HMC6352* compass = (HMC6352*)sensor;
	const I2C_DEVICE* i2c = &(compass->i2cInfo);
	HEADING_REPLY reply;

	// Post heading data
// Not need in continuous mode
//	uint8_t  cmd = CMD_GET_HEADING;			// post heading and pitch
//	i2cMasterSend(compass->i2cAddress,1,&cmd);
//	delay_ms(6);							// Datasheet says to wait for 6ms before reading response

	// receive the response
	if(i2cMasterReceive(i2c, sizeof(HEADING_REPLY), (uint8_t*)(&reply))){
		// Get heading in 10ths of a degree 0->3600 and round to degrees
		uint16_t heading10ths = reply.heading_msb;
		heading10ths <<= 8;
		heading10ths |= reply.heading_lsb;
		compass->compass.bearingDegrees = (heading10ths + 5)/10;
	}
}

// Requires a 500ms delay before being read for the first time
// The fastest it can go is 20Hz ie every 50ms
COMPASS_CLASS const c_HMC6352 = MAKE_COMPASS_CLASS(&__HMC6352_init,&__HMC6352_read, 500 , 50 );

