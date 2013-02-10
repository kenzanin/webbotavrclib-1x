
/*
 * $Id: AX12info.c,v 1.2 2010/06/14 19:14:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: AX12info.c,v $
 * Revision 1.2  2010/06/14 19:14:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/03/24 19:49:27  clivewebster
 * Alpha release
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
 * AX12info.c
 *
 *  Created on: 23 Mar 2010
 *      Author: Clive Webster
 *
 * Return info about a servo
 *
 * Split into a seperate file as its not needed if you are only issuing commands to the servo
 */

#include "AX12.h"
#include "../../core.h"

// The number of bytes to read from the ax12
#define INFO_LENGTH (ax12_LOCK - ax12_PRESENT_POSITION)

uint16_t ax12GetInfo(DYNAMIXEL_AX12* servo){
	uint16_t rtn = 0;
	if(act_isConnected(servo)){
		HW_UART* uart = servo->driver->ax12uart;
		uint8_t str[] = {ax12_READ,			// Read
				ax12_PRESENT_POSITION,		// Start address
				INFO_LENGTH					// Length = up to but excluding ax12_LOCK
		};


		// Flush any received data
// Doesn't use an input buffer
//		uartFlushReceiveBuffer(uart);

		// Listen for the response
		uint8_t reply[6+INFO_LENGTH];		// 0xff,0xff,id,length,error,INFO_LENGTH bytes,checksum
		uint8_t replyPos = 0;
		uint16_t now=0;
		boolean timeOut = FALSE;

		// Send the request
		ax12Send(servo->driver, servo->id, sizeof(str), str);

		// Wait till xmit is done
		while(uartIsBusy(uart)){
			breathe();
		}

		CRITICAL_SECTION_START;						// At 1Mb baud we have to poll for chars or they get missed
		while(replyPos < sizeof(reply)){
			int ch = uartPollByte(uart);

			if(ch != -1){
				reply[replyPos++] = (uint8_t)(ch & 0xff);
				now = 0;
			}else if(--now == 0){					// Timeout after a while
				timeOut = TRUE;
				break;
			}
		}
		CRITICAL_SECTION_END;

		// Check to see if we timed out
		if(timeOut){
			return AX12_RECV_TIMEOUT;
		}

		// Now validate the response
		if(reply[0]==0xff && reply[1]==0xff){
			// Got a valid header
			if(reply[2]==servo->id){
				// From the correct servo
				if(reply[3]==sizeof(reply)-4){
					// it is the correct length
					uint8_t chksum = 0;
					for(int i=2; i<sizeof(reply);i++){
						chksum+=reply[i];
					}
					if(chksum==0xff){
						// The checksum is correct
						rtn = reply[4];
						if(rtn == 0){
							// Copy data out
							uint16_t pos = reply[5] | (reply[6]<<8); // 0...1023
							servo->info.position=interpolate(pos, 0, 1023, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX);


							servo->info.speed = reply[7] | (reply[8]<<8);
							if(servo->info.speed & 0x400){
								servo->info.speed = - (servo->info.speed & 0x3ff);
							}

							servo->info.load = reply[9] | (reply[10]<<8);
							if(servo->info.load & 0x400){
								servo->info.load = - (servo->info.load & 0x3ff);
							}

							if(servo->actuator.inverted){
								servo->info.position *= -1;
								servo->info.load *= -1;
								servo->info.speed *= -1;
							}
							servo->info.voltage = reply[11];
							servo->info.temperature = reply[12];
							servo->info.batch = (reply[13]) ? TRUE : FALSE;
							// byte14 is the reserved value
							servo->info.moving = (reply[15]) ? TRUE : FALSE;
						}
					}else{
						rtn = AX12_RECV_CHECKSUM;
					}
				}else{
					rtn = AX12_RECV_LEN;
				}
			}else{
				rtn = AX12_RECV_ID;
			}
		}else{
			rtn = AX12_RECV_HEADER;
		}
	}
	return rtn;
}

