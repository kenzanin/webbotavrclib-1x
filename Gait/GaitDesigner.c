
/*
 * $Id: GaitDesigner.c,v 1.3 2010/06/14 18:38:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: GaitDesigner.c,v $
 * Revision 1.3  2010/06/14 18:38:51  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/05/28 18:06:54  clivewebster
 * Add the C command to get the config of a servo
 *
 * Revision 1.1  2010/04/25 03:32:26  clivewebster
 * Add Gait behaviour
 *
 * ===========
 * 
 * Copyright (C) 2010 Clive Webster (Webbot)
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
 * GaitDesigner.c
 *
 *  Created on: 21 Apr 2010
 *      Author: Clive Webster
 */

#include "GaitDesigner.h"
#include <stdlib.h>
#include "../rprintf.h"
#include "../servos.h"


static __ACTUATOR* getEntry(const GAIT_DESIGNER* gait, uint8_t inx){
	return (__ACTUATOR*)pgm_read_word(&gait->actuators[inx]);

}

void gaitDesignerInit(GAIT_DESIGNER* gait){
	// Initialise the uart
	_uartInit(gait->uart,gait->baudRate);
	if(gait->buffer == null){
		gait->buffer = malloc(gait->num_actuators * 2 + 6); // Max is 2 bytes per actuator + some header/trailer
	}
	gait->msgInx = 0;
}

static int8_t hexDigit(const uint8_t* buf, uint8_t inx){
	uint8_t rtn = buf[inx];
	if(rtn>='A'){
		rtn = rtn - 'A' + 10;
	}else{
		rtn &= 15;
	}
	return rtn;
}

static void setSpeed(const GAIT_DESIGNER* gait, uint8_t servo, int8_t percent){
	int16_t temp = (int16_t)percent * DRIVE_SPEED_MAX;
	DRIVE_SPEED speed = temp / 100;
	__ACTUATOR* act = getEntry(gait,servo);
	__act_setSpeed(act,speed);
}

// Process incoming characters
void gaitDesignerProcess(GAIT_DESIGNER* gait){
	int b = __uartGetByte(gait->uart);
	if(b==-1) return;	// No characters at all
	
	uint8_t inx = gait->msgInx;
	uint8_t* buffer = gait->buffer;

	Writer old = rprintfInit(gait->uart->writer);

	// Process all received characters
	while(b!=-1){
		uint8_t servo;
		int8_t percent;

		uint8_t c = b & 0xff;;
//		_uartSendByte(gait->uart,c);
		if(c == '\r'){
			// ignore it
		}else if(c=='\n'){
			buffer[inx] = '\0';
			// now process msg
			if(buffer[0]=='G' && inx>=3 && inx%2 ==1){
				// A group message,
				inx=0;
				for(servo=0; servo < gait->num_actuators && buffer[inx+1];servo++){
					int8_t percent = (hexDigit(buffer,++inx) << 4) | hexDigit(buffer,++inx);
					setSpeed(gait,servo,percent);
				}
			}else if(buffer[0]=='N' && inx==1){
				// Reply with number of servos
				rprintf("#n%d\r\n",gait->num_actuators);
			}else if(buffer[0]=='C'){
				// Get config ie: C0
				// Reply with: c0,center,range
				servo=0;
				inx=1;
				while(buffer[inx]>='0' && buffer[inx]<='9'){
					servo*=10;
					servo+= (buffer[inx++] & 15);
				}
				SERVO* theServo = (SERVO*)getEntry(gait, servo);

				rprintf("#c%d,%d,%d\r\n",servo,theServo->center_us,theServo->range_us);
			}else if(buffer[0]=='S'){
				// Single servo cmd  <inx>,<speed>
				boolean neg = FALSE;
				servo=0;
				percent=0;
				inx=1;
				while(buffer[inx]>='0' && buffer[inx]<='9'){
					servo*=10;
					servo+= (buffer[inx++] & 15);
				}
				if(buffer[inx++]==','){
					if(buffer[inx]=='-'){
						inx++;
						neg=TRUE;
					}
					while(buffer[inx]){
						percent*=10;
						percent += (buffer[inx++] & 15);
					}
					if(neg){
						percent *= -1;
					}
					setSpeed(gait,servo,percent);
				}
			}
			inx=0;
		}else if (c=='#'){
			inx = 0;
		}else{
			buffer[inx++] = b;
		}
		b = __uartGetByte(gait->uart);
	}
	gait->msgInx = inx;
	rprintfInit(old);
}
