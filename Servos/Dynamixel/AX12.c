
/*
 * $Id: AX12.c,v 1.6 2010/07/15 14:39:53 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: AX12.c,v $
 * Revision 1.6  2010/07/15 14:39:53  clivewebster
 * Reduce uart activity if servo is inverted
 *
 * Revision 1.5  2010/07/01 23:49:50  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.4  2010/06/14 19:14:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/03/25 16:48:08  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2010/03/24 19:49:27  clivewebster
 * Alpha release
 *
 * Revision 1.1  2010/03/08 16:12:27  clivewebster
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
 * AX12.c
 *
 *  Created on: 24 Feb 2010
 *      Author: Clive Webster
 */

#include "AX12.h"
#include "../../uart.h"
#include "../../core.h"


static const __ACTUATOR_DRIVER_CLASS c_ax12;

void ax12Send(const DYNAMIXEL_AX12_DRIVER* driver, uint8_t id, size_t len, uint8_t* data){
	HW_UART* uart = driver->ax12uart;
	uartSendByte(uart, 0xff);
	uartSendByte(uart, 0xff);
	uint8_t chksum = uartSendByte(uart, id);
	chksum += uartSendByte(uart, len+1);

	while(len--){
		chksum += uartSendByte(uart, *data++);
	}
	uartSendByte(uart, ~chksum);

}

// Send a command expecting no response
void ax12Write8(const DYNAMIXEL_AX12* servo, uint8_t cmd, uint8_t data){
	if(act_isConnected(servo)){
		uint8_t str[] = {servo->driver->writeCmd,cmd,data};
		ax12Send(servo->driver, servo->id, sizeof(str), str);
	}
}

// Send a command expecting no response
void ax12Write16(const DYNAMIXEL_AX12* servo, uint8_t cmd, uint16_t data){
	if(act_isConnected(servo)){
		uint8_t str[] = {servo->driver->writeCmd,cmd,data & 0xff, (data>>8) & 0xff };
		ax12Send(servo->driver, servo->id, sizeof(str), str);
	}
}

// Callback to change the hardware to xmit or receive
// Called with interrupts disabled
static void xmitBlock(void* param, boolean xmit){
	DYNAMIXEL_AX12_DRIVER* driver = (DYNAMIXEL_AX12_DRIVER*)param;

	if(!xmit){
		// We are entering recv mode - turn off receive hardware interrupts
		// as they are just too slow for 1,000,000 baud. We will receive the
		// characters by polling the uart hardware directly instead
		uartReceivePollingMode(driver->ax12uart,TRUE);
	}

	// Change the hardware state
	pin_set(driver->directionPin, xmit);
}

void ax12Begin(DYNAMIXEL_AX12_DRIVER* driver){
	driver->writeCmd = ax12_REG_WRITE; // queue cmd
}

void ax12End(DYNAMIXEL_AX12_DRIVER* driver){
	driver->writeCmd = ax12_WRITE;    // don't queue cmds

	// Broadcast an ACTION cmd
	uint8_t status[] = {ax12_ACTION};
	ax12Send(driver, ax12_BROADCAST_ID, sizeof(status), status);
}


void ax12Init(DYNAMIXEL_AX12_DRIVER* driver, BAUD_RATE baud){
	// Set the callback if there is a direction pin
	if(driver->directionPin){
		pin_make_output(driver->directionPin, FALSE);// put into receive mode
		driver->ax12uart->_uart_.xmitCallback = &xmitBlock;
		driver->ax12uart->_uart_.callbackParam = driver;
	}else{
		if(driver->ax12uart->_uart_.class == &c_hw_uart){
			// Its a hardware UART - so make it one wire before initialising
			driver->ax12uart->_uart_.oneWire = TRUE;
		}
	}


	// Set the baud rate
	uartInit(driver->ax12uart, baud);


	// Tell all servos to only respond to READ commands
	uint8_t status[] = {ax12_WRITE, ax12_STATUS_RTN_LEVEL,1};
	ax12Send(driver, ax12_BROADCAST_ID, sizeof(status), status);


	// Initialuse each servo
	for(int i= driver->num_servos-1;i>=0;i--){
		DYNAMIXEL_AX12* servo = (DYNAMIXEL_AX12*)pgm_read_word(&driver->servos[i]);

		// Connect the servo to this driver
		servo->actuator.class = &c_ax12;
		servo->driver = driver;

		// Set it as connected
		act_setConnected(servo,TRUE);


		if(servo->continuous){
			// Put into continuous rotation mode
			ax12SetCW_ANGLE_LIMIT(servo,0);
			ax12SetCCW_ANGLE_LIMIT(servo,0);
		}else{
			// Put into servo mode
			ax12SetCW_ANGLE_LIMIT(servo,0);
			ax12SetCCW_ANGLE_LIMIT(servo,1023);
		}

		// Start off braking
		act_setSpeed(servo,DRIVE_SPEED_BRAKE);
	}

}

// Callback - for when the speed has been set
static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	DYNAMIXEL_AX12* servo = (DYNAMIXEL_AX12*)actuator;

	DRIVE_SPEED current = servo->actuator.required_speed;
	if(servo->actuator.inverted){
		current *= -1;
	}


	// Ignore if it hasn't been changed
	if(speed != current){

		// Send command to set speed
		if(servo->continuous){
			// Interpolate the values to 0 to 1023

			uint16_t position=interpolateU(ABS(speed), DRIVE_SPEED_BRAKE, DRIVE_SPEED_MAX, 0, 0x3ff);
			if(speed<0){
				position |= 0x400;
			}
			ax12SetMOVING_SPEED(servo,position);
		}else{
			// Interpolate the values to 0 to 1023
			uint16_t position=interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, 0, 1023);
			ax12SetGOAL_POSITION(servo,position);
		}
	}

}



static const __ACTUATOR_DRIVER_CLASS c_ax12 = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,null);
