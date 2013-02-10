/*
 * $Id: spiUart.c,v 1.8 2011/07/06 00:26:08 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spiUart.c,v $
 * Revision 1.8  2011/07/06 00:26:08  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.7  2010/07/01 23:56:02  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/03/07 20:17:37  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 19:05:46  clivewebster
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
 * spiUart.c
 *
 *  Created on: 26-Jun-2009
 *      Author: Clive Webster
 */
#include "spiUart.h"

// Define the implementations of the virtual classes
static void __spiUARTInit(SPI_ABSTRACT_BUS* spi, boolean master);
static void __spiUARTOff(SPI_ABSTRACT_BUS* spi);
static void __spiUARTSetClock(SPI_ABSTRACT_BUS* spi, SPI_CLOCK clock);
static void __spiUARTSetDataOrder(SPI_ABSTRACT_BUS* spi,SPI_DATA_ORDER order);
static void __spiUARTSetMode(SPI_ABSTRACT_BUS* spi,SPI_MODE mode);
static uint8_t __spiUARTSendByte(SPI_ABSTRACT_BUS* spi, uint8_t data);

SPI_CLASS PROGMEM c_uart_spi = MAKE_SPI_CLASS(&__spiUARTInit, &__spiUARTOff,&__spiUARTSetClock,&__spiUARTSetDataOrder, &__spiUARTSetMode,&__spiUARTSendByte,null,null,null);


// compatibility for the mega161
#ifndef RXCIE
	#define RXCIE	RXCIE0
	#define TXCIE	TXCIE0
	#define RXEN	RXEN0
	#define TXEN	TXEN0
#endif

//------------- Private methods - dont call directly -----
static void __spiUARTInit(SPI_ABSTRACT_BUS* _spi, boolean master){
	SPI_UART* spi=(SPI_UART*)_spi;

	const IOPin* clock = spi->uart->spiClock;
	if(master && clock!=null){
		HW_UART* uart = spi->uart;

		// Turn off the UART whilst setting up
		uartOff(uart);

		// Baud rate must be set to 0 prior to enabling the USART as SPI
		// master, to ensure proper initialization of the XCK line.
		_SFR_MEM8(uart->baudL) =  0;
		_SFR_MEM8(uart->baudH) =  0;

		// Set XCK line to output, ie. set USART in master mode.
		pin_make_output(clock, TRUE);

		// When creating the device files we have checked that the UMSEL bits use
		// mask 0xc0 in statusC and that statusC=statusB+1

		// Set USART to Master SPI mode.
		_SFR_MEM8(uart->statusB+1) =  0xc0;

		// Set clock polarity and phase to correct SPI mode.
	    __spiUARTSetDataOrder(_spi,_spi->order);
	    __spiUARTSetMode(_spi,_spi->mode);

		// Enable RX and TX.
		_SFR_MEM8(uart->statusB) =  BV(RXEN)|BV(TXEN);

		// Set baud rate. Must be set _after_ enabling the transmitter.
	    __spiUARTSetClock(_spi,_spi->clock);
	}else{
		// Error - UART doesn't support SPI mode, or the clock pin is not available
		setError(SPI_UART_ERROR);
	}
}
static void	__spiUARTOff(SPI_ABSTRACT_BUS* _spi){
	SPI_UART* spi=(SPI_UART*)_spi;
	HW_UART* uart=spi->uart;

	// Turn off SPI master mode
	_SFR_MEM8(uart->statusB+1) &=  ~(0xc0);

	// Turn off the UART
	uartOff(uart);
}
static void __spiUARTSetClock(SPI_ABSTRACT_BUS* _spi, SPI_CLOCK clock){
	SPI_UART* spi=(SPI_UART*)_spi;
	HW_UART*  uart = spi->uart;
	if(uart->spiClock!=null){
		uint16_t bauddiv = (clock>>1)-1;
		if(bauddiv==0){
			bauddiv=1;
		}
		_SFR_MEM8(((HW_UART*)uart)->baudL) =  bauddiv;
		_SFR_MEM8(((HW_UART*)uart)->baudH) = bauddiv>>8;
	}
}

static void __spiUARTSetDataOrder(SPI_ABSTRACT_BUS* _spi,SPI_DATA_ORDER order){
	SPI_UART* spi  =(SPI_UART*)_spi;
	HW_UART*  uart = spi->uart;
	if(uart->spiClock!=null){
		PORT statusC =uart->statusB+1;
		if(order==SPI_DATA_ORDER_MSB){
			_SFR_MEM8(statusC) |= BV(2);	// Clear UDORD
		}else{
			_SFR_MEM8(statusC) &= ~(BV(2));	// Set UDORD
		}
	}
}
static void __spiUARTSetMode(SPI_ABSTRACT_BUS* _spi,SPI_MODE mode){
	SPI_UART* spi  =(SPI_UART*)_spi;
	HW_UART*  uart = spi->uart;
	if(uart->spiClock!=null){
		PORT statusC =uart->statusB+1;
		_SFR_MEM8(statusC) = (_SFR_MEM8(statusC) & ~(3)) | (mode & 3);
	}
}

static uint8_t __spiUARTSendByte(SPI_ABSTRACT_BUS* _spi, uint8_t data){
#ifdef UDRE0
	SPI_UART* spi=(SPI_UART*)_spi;
	HW_UART*  uart = spi->uart;
	if(uart->spiClock!=null){
		PORT statusA =uart->statusA;

		// Wait for empty transmit buffer.
		do {} while( (_SFR_MEM8(statusA) & (1<<UDRE0)) == 0 );

		// Send data.
		_SFR_MEM8(uart->data) = data;

		// Wait for transfer to complete and return received value.
		do {} while( (_SFR_MEM8(statusA) & (1<<RXC0)) == 0 );

		return _SFR_MEM8(uart->data);
	}
#endif
	return -1;
}


