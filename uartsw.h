/*
 * $Id: uartsw.h,v 1.9 2010/12/02 19:11:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: uartsw.h,v $
 * Revision 1.9  2010/12/02 19:11:04  clivewebster
 * Try to achieve higher and more reliable baud rates
 *
 * Revision 1.8  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.7  2010/02/17 23:57:58  clivewebster
 * Specify timer number in MAKE command rather than a point to the TIMER
 *
 * Revision 1.6  2010/02/09 17:12:50  clivewebster
 * Add uartGetReader and uartGetWriter for each uart
 *
 * Revision 1.5  2010/02/09 16:46:01  clivewebster
 * Added ATMega2561
 *
 * Revision 1.4  2009/11/02 19:07:54  clivewebster
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
 * uartsw.h
 *
 *  Created on: 30-Apr-2009
 *      Author: Clive Webster
 *
 *  This requires a timer which supports input capture
 *  The receive pin must be the Input Capture pin for the timer
 *  The xmit pin can be any IO pin
 *  The first compare channel on the timer is used to schedule transmit bits
 *  The second compare channel on the timer is used to schedule receive bit sampling
 *
 *  Either the receive pin or the transmit can be 'null' if only talking in one direction
 *  If the transmit and receive pins are the same then it uses RS485 half-duplex mode ie the device
 *  talks and then listens.
 *
 *  Device			Timer		Pin
 *  ATMega168		1			B0
 *
 *  ATMega2560		1			D4
 *  				3			E7
 *					4			L0
 *					5			L1
 *
 *  ATMega2561		1			D4
 *  				3			E7
 *
 *  ATMega640		1			D4
 *  				3			E7
 *					4			L0
 *					5			L1
 *
 *  ATMega32		1			D6
 *
 *  ATMega328P		1			B0
 *
 *  ATMega8			1			B0
 */

#ifndef UARTSW_H_
#define UARTSW_H_

#include "_uart_common.h"
#include "iopin.h"

// constants/macros/typdefs
typedef struct s_uart_sw {
	UART				 _uart_;
	const uint8_t		timer;		// The 16 bit timer to be used
	volatile uint8_t 	rxData;		// The byte currently being received
	volatile uint8_t 	rxBitNum;	// The bit number currently being received
	volatile uint8_t 	txData;		// The byte currently being transmitted
	volatile uint8_t 	txBitNum;	// The bit number currently being transmitted
	volatile uint16_t 	startBitLength;// The time for the start bit
	volatile uint16_t 	dataBitLength;// The time between data bits
	boolean				inverted;	// Is it inverted
} SW_UART;

// Class definition for a software UART
extern const UART_CLASS c_sw_uart;

// Constructor for a buffered software UART
#define MAKE_SW_UART_BUFFERED(rxBuf,txBuf,timer,rxpin,txpin,inv,reader,writer) \
	{ MAKE_UART_COMMON(rxBuf,txBuf,&c_sw_uart,rxpin,txpin,reader,writer) , \
	timer,0,0,0,0,0,0,inv }


// functions

#endif /* UARTSW_H_ */
