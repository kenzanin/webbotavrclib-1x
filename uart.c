/*
 * $Id: uart.c,v 1.15 2011/03/07 01:35:18 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: uart.c,v $
 * Revision 1.15  2011/03/07 01:35:18  clivewebster
 * Only enable RX or TX interrupts if they are used
 *
 * Revision 1.14  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.13  2010/03/24 19:46:08  clivewebster
 * Allow polling mode for hardware UARTs
 *
 * Revision 1.12  2010/03/07 19:31:13  clivewebster
 * *** empty log message ***
 *
 * Revision 1.11  2010/01/24 17:39:55  clivewebster
 * Moved ISRs out to device,h
 *
 * Revision 1.10  2009/12/11 17:19:29  clivewebster
 * *** empty log message ***
 *
 * Revision 1.9  2009/11/16 03:36:32  clivewebster
 * Fix ATmega8 uart not being included
 *
 * Revision 1.8  2009/11/02 19:07:54  clivewebster
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
 * uart.c
 *
 *  Created on: 17-Mar-2009
 *      Author: Clive Webster
 */
#include "uart.h"

// compatibility for the mega161
#ifndef RXCIE
	#define RXCIE	RXCIE0
	#define TXCIE	TXCIE0
	#define RXEN	RXEN0
	#define TXEN	TXEN0
	#define RXC		RXC0
#endif


// Sets the uart baud rate.
// Argument should be in bits-per-second, like uartSetBaudRate(9600);
// or may be BAUD_RATE_MAX to use the highest possible(if inaccurate) baud rate
void __uartHWSetBaudRate(UART* uart, BAUD_RATE baudrate){
	HW_UART* hwUart = (HW_UART*)uart;
	CRITICAL_SECTION_START;
	// calculate division factor for requested baud rate, and set it
	uint16_t bauddiv = (baudrate==BAUD_RATE_MAX) ? 1 : ((cpu_speed + (baudrate*8L))/(baudrate*16L)-1);
	if(hwUart->u2xMask != 0){
		_SFR_MEM8(hwUart->statusA) &=  ~(hwUart->u2xMask);	// turn off u2x
		uint16_t bauddiv2 = (baudrate==BAUD_RATE_MAX) ? 1 : ((cpu_speed + (baudrate*4L))/(baudrate*8L)-1);
		if(bauddiv2 >= bauddiv){
			_SFR_MEM8(hwUart->statusA) |=  hwUart->u2xMask;	// turn on u2x
			bauddiv = bauddiv2;
		}
	}
	_SFR_MEM8(hwUart->baudL) =  bauddiv;
	_SFR_MEM8(hwUart->baudH) = bauddiv>>8;
	CRITICAL_SECTION_END;
}


// Initializes one UART to the default baud rate
// After running this init function, the processor
// I/O pins that used for uart communications (RXD, TXD)
// are no long available for general purpose I/O.
void __uartHWInit(UART* _uart, BAUD_RATE baud){
	HW_UART* uart = (HW_UART*) _uart;

	// set default baud rate
	uartSetBaudRate(uart, baud);


	uint8_t statB = 0;
	if(_uartIsOneWire(_uart)){
		statB =  BV(RXCIE) | BV(RXEN);		// Rx on
	}else{
		// enable RxD/TxD and interrupts
		if(uart->_uart_.rx_pin){
			statB = BV(RXCIE) | BV(RXEN);
		}
		if(uart->_uart_.tx_pin){
			statB |= BV(TXCIE) | BV(TXEN);
		}

//		_SFR_MEM8(uart->statusB) =  BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN);
	}

	_SFR_MEM8(uart->statusB) = statB;
}

// Disable the uart
static void __uartHWOff(UART * _uart){
	HW_UART* uart = (HW_UART*) _uart;

	_SFR_MEM8(uart->statusB) &=  ~(BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));

	if(uart->u2xMask != 0){
		_SFR_MEM8(uart->statusA) &=  ~(uart->u2xMask);	// turn off u2x
	}

}


// If one wire - then go into receive mode
static void oneWireReceiveMode(UART* _uart){
	if(_uartIsOneWire(_uart)){
		HW_UART* uart = (HW_UART*)_uart;
		// Place into receive mode and turn transmitter off
		_SFR_MEM8(uart->statusB) &=  ~(BV(TXCIE)|BV(TXEN));		// Tx Off
		_SFR_MEM8(uart->statusB) |=  (BV(RXCIE)|BV(RXEN));		// Rx on
	}
}

// Sends a single byte over the uart.
void __uartHWStartXmit(UART* _uart, uint8_t txData){
	HW_UART* uart = (HW_UART*)_uart;
	_SFR_MEM8(uart->data) = txData;		// Start sending the character
}


// Put the uart into xmit mode
static void	startXmitMode(UART* _uart){
	if(_uartIsOneWire(_uart)){
		HW_UART* uart = (HW_UART*)_uart;
		// Place into transmit mode and turn receiver off
		_SFR_MEM8(uart->statusB) &=  ~(BV(RXCIE)|BV(RXEN));		// Rx Off
		_SFR_MEM8(uart->statusB) |=  (BV(TXCIE)|BV(TXEN));		// Tx on
	}
}


// Create a class with the method overrides for this type of UART
UART_CLASS const c_hw_uart = MAKE_UART_CLASS( \
		&__uartHWSetBaudRate, 	\
		&__uartHWStartXmit, 	\
		&__uartHWInit, 			\
		&__uartHWOff,			\
		&startXmitMode, \
		&oneWireReceiveMode );

// Put the receiver in polling mode (TRUE) or interrupt mode (FALSE)
void uartReceivePollingMode(const HW_UART* uart,boolean polling){
	if(polling){
		// turn off receive interrupts
		_SFR_MEM8(uart->statusB) &=  ~(BV(RXCIE));		// Rx Off
	}else{
		// turn on receive interrupts
		_SFR_MEM8(uart->statusB) |=  BV(RXCIE);			// Rx On
	}
}


// Poll the hardware for the next receive character
int uartPollByte(const HW_UART* uart){
	int ch=-1;
	if( _SFR_MEM8(uart->statusA) & BV(RXC)){
		// A character is waiting
		ch = _SFR_MEM8(uart->data);

	}
	return ch;
}


