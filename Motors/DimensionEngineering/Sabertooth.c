/*
 * $Id: Sabertooth.c,v 1.6 2010/06/14 18:46:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Sabertooth.c,v $
 * Revision 1.6  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/25 17:44:47  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/12/11 17:11:41  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:33:32  clivewebster
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
 * Sabertooth.c
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 */
#include "Sabertooth.h"

#include "../../core.h"
#include "../../timer.h"

// speed is 0 to 127
static void __saberOutput(SABERTOOTH_MOTOR* remote, boolean fwd, uint8_t speed){

	UART* uart = remote->driver->uart;

	switch(remote->driver->mode){
		case PACKETIZED:{
			uint8_t checksum=0;
			checksum += _uartSendByte(uart,remote->address);

			uint8_t cmd;
			if(remote->motorNumber==1){
				// motor 1
				cmd = (fwd) ? 0 : 1;
			}else{
				// motor 2
				cmd = (fwd) ? 4 : 5;
			}
			checksum += _uartSendByte(uart,cmd);

			checksum += _uartSendByte(uart,speed);

			_uartSendByte(uart, (checksum &0x7FU));
		}
		break;

		case SIMPLE:{
			if(remote->motorNumber==1){
				// 1 is full reverse, 64 is stop, 127 is full fwd
				speed = interpolate(speed, 0, 127, 64 , (fwd) ? 127 : 1);
			}else{
				// 128 is full reverse, 192 is stop, 255 is full fwd
				speed = interpolate(speed, 0, 127, 192 , (fwd) ? 255 : 128);
			}
			_uartSendByte(uart, speed);
		}
		break;
	}
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	SABERTOOTH_MOTOR* remote = (SABERTOOTH_MOTOR*)actuator;
	if(connected){
		DRIVE_SPEED speed = act_getSpeed(remote);
		actuator->required_speed=-128;
		act_setSpeed(remote, speed);
	}else{
		// Set speed = 0 in forwards to coast
		__saberOutput(remote, TRUE, 0);
	}
}

static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SABERTOOTH_MOTOR* remote = (SABERTOOTH_MOTOR*)actuator;
	if(act_isConnected(remote) && act_getSpeed(remote)!=speed){
		// controller speed is 0 to 127
		int8_t s = interpolate(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, -127 , 127);

		// If speed is 0 then set reverse to brake
		boolean fwd=TRUE;
		if(s <= 0){
			s = -s;
			fwd = FALSE;
		}

		__saberOutput(remote, fwd,s);
	}
}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_Sabertooth = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);


void sabertoothInit(SABERTOOTH_DRIVER* driver){


	switch(driver->mode){
		case PACKETIZED:{
			// Pause for 2s from power on
			delay_ms(2000);
			// Set baud rate
			_uartInit(driver->uart,driver->baudRate);
			// send bauding character
			_uartSendByte(driver->uart,0xaa);
			delay_ms(20);
			_uartSendByte(driver->uart,0xaa);
			delay_ms(20);
			_uartSendByte(driver->uart,0xaa);
			delay_ms(20);
			_uartSendByte(driver->uart,0xaa);
			delay_ms(20);
			_uartSendByte(driver->uart,0xaa);
			delay_ms(20);
		}
		break;
		case SIMPLE:{
			_uartInit(driver->uart,driver->baudRate);
		}
		break;
	}

	// set each motor to stop
	for(int i=0;i<driver->numMotors;i++){
		SABERTOOTH_MOTOR* motor = (SABERTOOTH_MOTOR*)pgm_read_word(&driver->motors[i]);
		motor->actuator.class=&c_Sabertooth;
		motor->driver = driver;
		act_setSpeed(motor,0);
		act_setConnected(motor,TRUE);
	}
}

