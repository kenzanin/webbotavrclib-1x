/*
 * $Id: HMC6343.c,v 1.7 2010/10/01 13:16:26 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: HMC6343.c,v $
 * Revision 1.7  2010/10/01 13:16:26  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.6  2010/09/30 16:46:30  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.5  2010/06/14 18:53:23  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/02/04 19:52:14  clivewebster
 * Added functions to set the refresh rate
 *
 * Revision 1.3  2009/11/02 18:20:31  clivewebster
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
 * HMC6343.c
 *
 *  Created on: 21-Sep-2009
 *      Author: Clive Webster
 */
#include "HMC6343.h"
#include "../../../timer.h"

#define CMD_GET_HEADING 0x50
#define CMD_WRITE_EEPROM 0xf1
typedef struct s_cmd_50{
	uint8_t heading_msb;		// 0
	uint8_t heading_lsb;		// 1
	uint8_t pitch_msb;			// 2
	uint8_t pitch_lsb;			// 3
	uint8_t roll_msb;			// 4
	uint8_t roll_lsb;			// 5
} HEADING_REPLY;

static void speed(HMC6343* compass, uint8_t val){
	if(compass){
		uint8_t cmd[] = {CMD_WRITE_EEPROM, 5, val};

		// Post heading data
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		i2cMasterSend(i2c,sizeof(cmd),cmd);
	}
}

// Set a 1Hz refresh period
void HMC6343_1Hz(HMC6343* compass){
	speed(compass,0);
}

// Set a 5Hz refresh period (default)
void HMC6343_5Hz(HMC6343* compass){
	speed(compass,1);
}

// Set a 10Hz refresh period
void HMC6343_10Hz(HMC6343* compass){
	speed(compass,2);
}

static void __HMC6343_read(SENSOR* sensor){
	if(sensor){
		HMC6343* compass = (HMC6343*)sensor;
		const I2C_DEVICE* i2c = &(compass->i2cInfo);
		uint8_t  cmd = CMD_GET_HEADING;			// post heading and pitch
		HEADING_REPLY reply;

		// Post heading data
		if(i2cMasterSend(i2c,1,&cmd)){
			// Datasheet says to wait for 1ms before reading response
			delay_ms(1);

			// receive the response
			if(i2cMasterReceive(i2c, sizeof(HEADING_REPLY), (uint8_t*)(&reply))){
				// Get heading in 10ths of a degree 0->3600 and round to degrees
				uint16_t heading10ths = reply.heading_msb;
				heading10ths <<= 8;
				heading10ths |= reply.heading_lsb;
				compass->compass.bearingDegrees = (heading10ths + 5)/10;

				// Get roll +- 900 tenths of a degree
				int16_t roll10ths = reply.roll_msb;
				roll10ths <<= 8;
				roll10ths |= reply.roll_lsb;
				compass->compass.rollDegrees = (roll10ths + 5)/10;

				// Get pitch +- 900 tenths of a degree
				int16_t pitch10ths = reply.pitch_msb;
				pitch10ths <<= 8;
				pitch10ths |= reply.pitch_lsb;
				compass->compass.pitchDegrees = (pitch10ths + 5)/10;
			}
		}
	}
}

// Requires a 500ms delay before being read for the first time
// The fastest it can go is 10Hz ie every 100ms
COMPASS_CLASS c_HMC6343 = MAKE_COMPASS_CLASS(null,&__HMC6343_read, 500 , 100 );
