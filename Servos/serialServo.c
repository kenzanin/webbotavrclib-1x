/*
 * $Id: serialServo.c,v 1.2 2010/09/30 16:54:59 clivewebster Exp $
 * Revision History
 * ================
 * $Log: serialServo.c,v $
 * Revision 1.2  2010/09/30 16:54:59  clivewebster
 * refactored to use union in servos.h
 *
 * Revision 1.1  2010/07/15 14:55:57  clivewebster
 * *** empty log message ***
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
 *        File: serialServo.c
 *  Created on: 5 Jul 2010
 *      Author: Clive Webster
 */

#include "../servos.h"
#include "../core.h"

static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SERVO* servo = (SERVO*)actuator;

	DRIVE_SPEED current = servo->actuator.required_speed;
	if(servo->actuator.inverted){
		current *= -1;
	}


	if(speed != current){
		// Speed has changed
		UART* uart = servo->driver->specific.serial.uart;
		if(uart){
			SERVO_PROTOCOL protocol = servo->driver->specific.serial.protocol;
			if(protocol==MINI_SSC){
				uint8_t speedByte;

				// Send 0xff, servo#, speed
				// where speed 0x7f=CENTER, 0x00=Left, 0xfe=Right
				if(DRIVE_SPEED_MIN == -127 && DRIVE_SPEED_MAX == 127){
					speedByte = speed + 127;
				}else{
					// Need to interpolate
					speedByte = interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, 0, 254);
				}

				_uartSendByte(uart, 0xff);
				_uartSendByte(uart, servo->delay & 0x7f);
				_uartSendByte(uart, speedByte);
			}else if(protocol==POLOLU_COMPACT){
				// Send 0x84, servo#, lsb, msb
				uint16_t speedWord;
				speedWord = interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX,
						4*(servo->center_us - servo->range_us),
						4*(servo->center_us + servo->range_us) );
				_uartSendByte(uart, 0x84);
				_uartSendByte(uart, servo->delay & 0x7f);
				_uartSendByte(uart, speedWord & 0x7f);
				_uartSendByte(uart, (speedWord>>7) & 0x7f );
			}
		}
	}
}


// Define the class, note that you cannot connect/disconnect servos
static __ACTUATOR_DRIVER_CLASS c_miniSSC_servos = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,null);

void _servoSerialInit(SERVO_DRIVER* driver, UART* uart, BAUD_RATE baud, SERVO_PROTOCOL protocol){
	driver->specific.serial.uart = uart;
	driver->specific.serial.protocol = protocol;
	// Set the baud rate
	_uartInit(uart, baud);

	// Make sure each servo is initialised
	for(int8_t i= driver->num_servos-1;i>=0;i--){
		SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);
		if(servo){
			// Connect the servo to this driver
			servo->actuator.class = &c_miniSSC_servos;
			servo->driver = driver;
			servo->top = 0;
			servo->delay = i; // The servo number in the list
			servoSetConfig(servo,servo->center_us,servo->range_us);

			act_setConnected(servo,TRUE);

			// Start off braking
			act_setSpeed(servo,DRIVE_SPEED_BRAKE);
		}
	}
}
