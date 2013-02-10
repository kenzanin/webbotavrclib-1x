/*
 * $Id: razor.c,v 1.2 2010/07/19 19:55:31 clivewebster Exp $
 * Revision History
 * ================
 * $Log: razor.c,v $
 * Revision 1.2  2010/07/19 19:55:31  clivewebster
 * Fixed parsing error
 *
 * Revision 1.1  2010/07/02 00:43:12  clivewebster
 * Added. But requires firmware to be uploaded to the Razor board.
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
 *        File: razor.c
 *  Created on: 24 Jun 2010
 *      Author: Clive Webster
 */

#include "razor.h"
#include <stdlib.h>

#define STARTUP_DELAY 	0
#define READ_DELAY 		10
#define NUM_VALUES		9

static void _init(SENSOR* sensor){
	RAZOR* razor = (RAZOR*)sensor;
	_uartInit(razor->uart,razor->baud);
}

static void _read(SENSOR* sensor){
	int chIn = -1;
	char response[16];
	uint8_t responseIndex;
	int16_t values[NUM_VALUES];
	uint8_t fieldCount;
	RAZOR* razor = (RAZOR*)sensor;
	uint16_t timeout;

	// Send a '1' and await a response
	_uartSendByte(razor->uart, '1');

start:
	responseIndex = fieldCount = timeout = 0;

	while( chIn != ';'){
		chIn = __uartGetByte(razor->uart);
		if(chIn == -1){
			// No character available
			if(--timeout == 0){
				// We have timed out - so keep values as before
				return;
			}
		}else{
			// Get the character
			char c = chIn & 0xff;

			// Reset the timeout detector
			timeout = 0;

			// Ignore CR and LFs
			if(c == '\r' || c == '\n'){
				goto start;
			}

			// Append character to the response
			if(responseIndex < sizeof(response)-1){
				response[responseIndex++] = c;
			}

			// Check for end of field
			if(c ==',' || c==';'){
				response[responseIndex] = 0; 		// Terminate the string
				int16_t value = atoi(response);	 		// Convert number to an integer
				if(fieldCount < NUM_VALUES){
					values[fieldCount] = value; 	// And store it if we have room
				}
				fieldCount ++;

				responseIndex = 0;			 // Reset for next field
			}

		}
	}

	if(fieldCount == NUM_VALUES ){
		// We have a complete message with 9 values
		razor->imu.x_axis_degrees_per_second = values[0];
		razor->imu.y_axis_degrees_per_second = values[1];
		razor->imu.z_axis_degrees_per_second = values[2];
		razor->imu.x_axis_mG = values[3];
		razor->imu.y_axis_mG = values[4];
		razor->imu.z_axis_mG = values[5];
		razor->imu.bearingDegrees = values[6];
		razor->imu.pitchDegrees = values[7];
		razor->imu.rollDegrees = values[8];
	}
}

IMU_CLASS c_razor = MAKE_IMU_CLASS(&_init, &_read, STARTUP_DELAY , READ_DELAY );
