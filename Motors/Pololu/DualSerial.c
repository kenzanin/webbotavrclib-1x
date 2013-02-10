/*
 * $Id: DualSerial.c,v 1.10 2010/06/14 18:46:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: DualSerial.c,v $
 * Revision 1.10  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.9  2010/01/25 16:52:03  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/12/11 17:12:09  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.6  2009/11/02 18:33:32  clivewebster
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
 * DualSerial.c
 *
 *  Created on: 25-Mar-2009
 *      Author: Clive Webster
 *
 * Many of Pololu motor controllers use the same serial protocol to control the motors but the hardware
 * differs in the amount of voltage/current it can supply to the motors. From our end - it all looks the
 * same
 *
 *
 * Each device can control two motors. Devices can be daisy chained to allow
 * control of up to 64 motors.
 * Serial baud rate is auto-detected between 1200 and 19200 baud, 8 data, no parity, 1 stop bit using TTL levels
 * so may be connected directly the mcu UART.
 *
 * Since the motors are connected to the external device we will just define them as a basic ACTUATOR.
 *
 * A speed of 0 in reverse will brake, speed of 0 in forwards will cause the motor to coast
 *
 */

#include "DualSerial.h"
#include "../../core.h"
#include "../../timer.h"


static void __pololuOutput(POLOLU_DS_MOTOR* remote, uint8_t fwd, uint8_t speed){
	POLOLU_DS_DRIVER* driver = remote->driver;
	POLOLU_DS_MOTOR* first = (POLOLU_DS_MOTOR*)pgm_read_word(&driver->devices[0]);

	uint8_t motorNo =  remote - first;
	motorNo += driver->firstMotorNumber;
	UART* __pololuUART = driver->uart;

	_uartSendByte(__pololuUART,0x80);
	_uartSendByte(__pololuUART,0x0);
	_uartSendByte(__pololuUART,((motorNo<<1) & 0x7fU)|fwd);
	_uartSendByte(__pololuUART,(uint8_t)speed);
}

static void setConnected(__ACTUATOR *actuator, boolean connected){
	POLOLU_DS_MOTOR* remote = (POLOLU_DS_MOTOR*)actuator;
	if(connected){
		DRIVE_SPEED speed = act_getSpeed(remote);
		actuator->required_speed=-128;
		act_setSpeed(remote, speed);
	}else{
		// Set speed = 0 in forwards to coast
		__pololuOutput(remote, 1, 0);
	}
}

static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	POLOLU_DS_MOTOR* remote = (POLOLU_DS_MOTOR*)actuator;
	if(act_isConnected(remote) && act_getSpeed(remote)!=speed){
		// controller speed is 0 to 127
		int8_t s = interpolate(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, -127 , 127);

		// If speed is 0 then set reverse to brake
		uint8_t fwd=1;
		if(s <= 0){
			s = -s;
			fwd = 0;
		}

		__pololuOutput(remote, fwd,s);
	}
}

// Define the class
static __ACTUATOR_DRIVER_CLASS c_Pololu_DualSerial = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,&setConnected);

void pololuDualSerialInit(POLOLU_DS_DRIVER* driver){
	_uartInit(driver->uart,driver->baudRate);
	for(int i=0;i<driver->num_devices;i++){
		POLOLU_DS_MOTOR* act = (POLOLU_DS_MOTOR*)pgm_read_word(&driver->devices[i]);
		act->actuator.class = &c_Pololu_DualSerial;
		act->driver = driver;
		act_setSpeed(act,0);
		act_setConnected(act,TRUE);
	}
	// Pause for 100ms
	delay_ms(100);
}

