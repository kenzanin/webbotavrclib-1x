
/*
 * $Id: DualSerial.h,v 1.8 2011/07/05 23:57:05 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: DualSerial.h,v $
 * Revision 1.8  2011/07/05 23:57:05  clivewebster
 * POLOLU_DS_MOTOR_LIST now requires PROGMEM
 *
 * Revision 1.7  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/01/25 16:52:03  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/11 17:12:09  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:33:32  clivewebster
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
 * DualSerial.h
 *
 *  Created on: 25-Mar-2009
 *      Author: Clive Webster
 *
 *      Drive pololu motor controller via a UART.
 *      This send bytes: 0x80, 0x00, b0mmmmmmd, b0sssssss
 *      where 'mmmmmm' is the motor number, 'd' is the direction 1=fwd 0=rev
 *      and 'sssssss' is the speed
 *  This will work with the following:
 * 		Micro Dual Serial Motor Controller (SMC02B) See http://www.pololu.com/file/download/smc02b_guide.pdf?file_id=0J36
 *		Each motor can draw up to 1 amp
 *
 *		Low Voltage Dual Serial Motor Controller (SMC05A) See http://www.pololu.com/file/0J59/smc05a_guide.pdf
 *		Each motor can draw up to 5 amps
 *
 *		3Amp motor controller with feedback (SMC03A) See http://www.pololu.com/file/0J61/smc03a_guide.pdf
 *		Each motor can draw up to 3 amps
 *
 *		High power motor controller with feedback (SMC04B) See http://www.pololu.com/file/0J60/smc04b_guide.pdf
 *		Each motor can draw up to 30 amps
 *
 *
 #include "Motors/Pololu/DualSerial.h"

REMOTE_ACTUATOR motor1 = MAKE_REMOTE_ACTUATOR(FALSE);
REMOTE_ACTUATOR motor2 = MAKE_REMOTE_ACTUATOR(FALSE);
REMOTE_ACTUATOR_LIST motors[] = {&motor1,&motor2};
REMOTE_ACTUATOR_DRIVER driver = MAKE_REMOTE_ACTUATOR_DRIVER(motors, UART0, 19200, 2);


void appInit(void){
	pololuDualSerialInit(&driver);
}

 *
 */

#ifndef DUALSERIAL_H_
#define DUALSERIAL_H_

#include "../../uart.h"
#include "../../actuators.h"



struct s_pololu_driver;     // Fwd def

/* -------------------------------------------------------
 * Define the structure for one pololu dual serial motor
-------------------------------------------------------- */
typedef struct s_pololuDS{
	__ACTUATOR actuator;				// The common actuator stuff
	struct s_pololu_driver* driver;		// The driver it is attached to, set by init'ing the driver
} POLOLU_DS_MOTOR;

// Define the standard constructor
#define MAKE_POLOLU_DS_MOTOR(inverted)  { MAKE_ACTUATOR(inverted),null}

/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef POLOLU_DS_MOTOR* /*PROGMEM*/  POLOLU_DS_MOTOR_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
-------------------------------------------------------- */
typedef struct s_pololu_driver{
	POLOLU_DS_MOTOR_LIST* const devices;	// The devices
	uint8_t num_devices; 					// The total number of devices
	UART*  uart;							// The UART to talk to them via
	BAUD_RATE baudRate;						// The baud rate
	uint8_t firstMotorNumber;				// The first motor number in the list
} POLOLU_DS_DRIVER;

#define MAKE_POLOLU_DS_DRIVER(motors, uart, baudrate, firstMotorNumber) { \
	motors, \
	(uint8_t)(sizeof(motors)/sizeof(POLOLU_DS_MOTOR*)), \
	&((uart)->_uart_),baudrate,firstMotorNumber \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */

void pololuDualSerialInit(POLOLU_DS_DRIVER* driver);

#endif /* DUALSERIAL_H_ */
