 /* ----------------------------------------------------------------------
 * $Id: uart.h,v 1.12 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: uart.h,v $
 * Revision 1.12  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.11  2010/03/24 19:46:09  clivewebster
 * Allow polling mode for hardware UARTs
 *
 * Revision 1.10  2010/02/09 15:04:27  clivewebster
 * Add uartGetReader and uartGetWriter for each uart
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
 *
 *  Cope with hardware UARTs
 *
 -------------------------------------------------------------------------*/
#ifndef _UART4_H_
#define _UART4_H_

#include "_uart_common.h"

typedef struct s_uart {
	UART				 _uart_;
	const PORT			 statusA;
	const PORT			 statusB;
	const PORT			 baudL;
	const PORT			 baudH;
	const PORT			 data;
	const IOPin*		 spiClock;    // The pin used for SPI clock. eg XCK0 or null if no spi support
	const uint8_t		 u2xMask;
} HW_UART;
extern HW_UART Uarts[];

// Class definition for a hardware UART
extern const UART_CLASS c_hw_uart;

// Number of uarts
extern const uint8_t NUM_UARTS;

// Constructor for a buffered Hardware UART
#define MAKE_UART_BUFFERED(rxBuf,txBuf,statusA,statusB,baudL,baudH,data,spi,u2x,rx,tx,reader,writer) \
	{ MAKE_UART_COMMON(rxBuf,txBuf,&c_hw_uart,rx,tx,reader,writer) , \
	_SFR_MEM_ADDR(statusA),_SFR_MEM_ADDR(statusB),_SFR_MEM_ADDR(baudL),_SFR_MEM_ADDR(baudH),_SFR_MEM_ADDR(data),spi,u2x}

// Constructor for a non-buffered Hardware UART
#define MAKE_UART(statusA,statusB,baudL,baudH,data,spi,u2x,rx,tx,reader,writer) \
	MAKE_UART_BUFFERED(null,null,statusA,statusB,baudL,baudH,data,spi,u2x,rx,tx,reader,writer)


// Poll the uart for the next byte
int uartPollByte(const HW_UART* uart);

// Put the receiver in polling mode (TRUE) or interrupt mode (FALSE)
void uartReceivePollingMode(const HW_UART* uart,boolean polling);

//#if !defined(BUILDING_LIBRARY)
// Initialise all of the hardware uarts
// NB This is called automatically at start up
//void __attribute__ ((constructor)) initUarts(void)   {
//	int i;
//	for(i=0;i<NUM_UARTS;i++){
//		uartInit(&(Uarts[i]) ,UART_DEFAULT_BAUD_RATE);
//	}
//}
//#endif


// ----------------- Create some simpler functions for each uart --------------

#if defined(UBRR0) || defined(UBRRL) || defined(UBRR0L)
MAKE_WRITER(uart0SendByte);
MAKE_READER(uart0GetByte);
#endif

#if defined(UBRR1)  || defined(UBRR1L)
MAKE_WRITER(uart1SendByte);
MAKE_READER(uart1GetByte);
#endif

#ifdef UBRR2
MAKE_WRITER(uart2SendByte);
MAKE_READER(uart2GetByte);
#endif

#ifdef UBRR3
MAKE_WRITER(uart3SendByte);
MAKE_READER(uart3GetByte);
#endif

#endif

