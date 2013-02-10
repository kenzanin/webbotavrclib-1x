/*
 * $Id: Sabertooth.h,v 1.7 2011/07/05 23:56:14 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Sabertooth.h,v $
 * Revision 1.7  2011/07/05 23:56:14  clivewebster
 * SABERTOOTH_MOTOR_LIST now requires PROGMEM
 *
 * Revision 1.6  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/25 17:44:48  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/01/24 18:18:28  clivewebster
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
 * Sabertooth.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Dimension Engineering Sabertooth controller 2x5Amp and also the SyRen10 and SyRen25
 *
 *  This uses the packetised serial link (which can support multiple boards)
 *  or the simple serial link (which can only support one board)
 *
 *
 */

#ifndef SABERTOOTH_H_
#define SABERTOOTH_H_
#include "../../uart.h"
#include "../../actuators.h"


typedef enum {
	PACKETIZED,
	SIMPLE
} SABERTOOTH_MODE;

struct s_sabertooth_driver;     // Fwd def

/* -------------------------------------------------------
 * Define the structure for one sabertooth motor
-------------------------------------------------------- */
typedef struct s_sabertooth{
	__ACTUATOR actuator;		// The common actuator stuff
	uint8_t	   address;			// The address of the card this motor uses.
								// Set via DIP switches on the card
	uint8_t	   motorNumber;		// eg motor 1 or motor 2
	struct s_sabertooth_driver* driver;
} SABERTOOTH_MOTOR;

// Define the standard constructor
#define MAKE_SABERTOOTH_MOTOR(inverted, address, motornumber)  { MAKE_ACTUATOR(inverted),address,motornumber,null}

/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef SABERTOOTH_MOTOR* /*PROGMEM*/  SABERTOOTH_MOTOR_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
-------------------------------------------------------- */
typedef struct s_sabertooth_driver{
	SABERTOOTH_MOTOR_LIST* const motors;// The list of SABERTOOTH_MOTOR
	UART* uart;							// The uart to use to control them all
	BAUD_RATE  baudRate;				// The baud rate to use
	const SABERTOOTH_MODE mode;			// The operating mode
	const uint8_t numMotors;			// The number of motors in the list
} SABERTOOTH_DRIVER;


#define MAKE_SABERTOOTH_DRIVER(motors, uart, baudrate, mode) { \
	motors, \
	&((uart)->_uart_), \
	baudrate, mode, \
	(uint8_t)(sizeof(motors)/sizeof(SABERTOOTH_MOTOR*)) \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */
void sabertoothInit(SABERTOOTH_DRIVER* driver);

#endif /* SABERTOOTH_H_ */
