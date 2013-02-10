/*
 * $Id: spi.c,v 1.14 2011/07/06 00:28:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spi.c,v $
 * Revision 1.14  2011/07/06 00:28:04  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.13  2011/02/21 20:29:54  clivewebster
 * Added ATMega1284P
 *
 * Revision 1.12  2010/07/19 19:46:27  clivewebster
 * Added ATMega644
 *
 * Revision 1.11  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.10  2010/05/09 22:09:35  clivewebster
 * Add ATMega128
 *
 * Revision 1.9  2010/04/12 23:15:34  clivewebster
 * Add pullup on MISO and add support for ATMega128rfa1
 *
 * Revision 1.8  2010/03/07 20:17:47  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2010/02/17 23:32:33  clivewebster
 * Add support for ATMega1280
 *
 * Revision 1.6  2010/02/09 16:45:29  clivewebster
 * Added ATMega2561
 *
 * Revision 1.5  2009/11/02 19:05:46  clivewebster
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
 * spi.c
 *
 *  Created on: 23-Jun-2009
 *      Author: Clive Webster
 *
 *  Implement an SPI interface in hardware
 */
#include "spi.h"
#include <avr/io.h>

// Define the implementations of the virtual classes
static void 	__spiHWInit(SPI_ABSTRACT_BUS* spi, boolean master);
static void 	__spiHWOff(SPI_ABSTRACT_BUS* spi);
static void 	__spiHWSetClock(SPI_ABSTRACT_BUS* spi, SPI_CLOCK clock);
static void 	__spiHWSetDataOrder(SPI_ABSTRACT_BUS* spi,SPI_DATA_ORDER order);
static void 	__spiHWSetMode(SPI_ABSTRACT_BUS* spi,SPI_MODE mode);
static uint8_t 	__spiHWSendByte(SPI_ABSTRACT_BUS* spi, uint8_t data);

SPI_CLASS const PROGMEM c_hw_spi = MAKE_SPI_CLASS(&__spiHWInit, &__spiHWOff,&__spiHWSetClock,&__spiHWSetDataOrder, &__spiHWSetMode,&__spiHWSendByte, null, null,null);


#if defined (__AVR_ATmega640__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__) || defined (__AVR_ATmega128RFA1__)
// Define the SPI pins for ATMega640/ATMega128/ATMega1280/ATMega2560/ATMega2561/ATMega128RFA1
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PB0

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PB1

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PB2

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PB3
#elif defined (__AVR_ATmega168__)
// Define the SPI pins for ATMega168
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PB2

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PB5

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PB3

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PB4

#elif defined (__AVR_ATmega8__)
// Define the SPI pins for ATMega8
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PB2

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PB5

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PB3

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PB4

#elif defined (__AVR_ATmega32__) || defined (__AVR_ATmega644__)
// Define the SPI pins for ATMega168
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PB4

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PB7

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PB5

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PB6

#elif defined (__AVR_ATmega328P__)
// Define the SPI pins for ATMega328P
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PORTB2

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PORTB5

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PORTB3

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PORTB4

#elif defined (__AVR_ATmega1284P__)
// Define the SPI pins for ATMega1284P
#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_PIN  PORTB4

#define SCK_PORT PORTB
#define SCK_DDR  DDRB
#define SCK_PIN  PORTB7

#define MOSI_PORT PORTB
#define MOSI_DDR  DDRB
#define MOSI_PIN  PORTB5

#define MISO_PORT PORTB
#define MISO_DDR  DDRB
#define MISO_PIN  PORTB6

#else
#error SPI pins not defined
#endif




//------------- Private methods - dont call directly -----
static void __spiHWInit(SPI_ABSTRACT_BUS* _spi, boolean master){
	//SPI* spi = (SPI*)_spi;
	CRITICAL_SECTION_START;
    volatile char IOReg;
    if(master){
    	sbi(SCK_DDR, SCK_PIN);		// set SCK as output
    	sbi(MOSI_DDR, MOSI_PIN);	// set MOSI as output
    	cbi(MISO_DDR, MISO_PIN);	// set MISO as an input
    	sbi(MISO_PORT,MISO_PIN);	// enable pullup on MISO
    	sbi(SS_DDR, SS_PIN);		// set SS as output for Master mode to work
        // enable SPI in Master Mode, Data order=MSB first, Mode=0, with SCK = CK/4
        SPCR    = (1<<SPE)|(1<<MSTR);
    }else{
    	sbi(MISO_DDR, MISO_PIN);	// set MISO as output
        SPCR    = (1<<SPE);
    }

    __spiHWSetClock(_spi,_spi->clock);
    __spiHWSetDataOrder(_spi,_spi->order);
    __spiHWSetMode(_spi,_spi->mode);

    // clear SPIF bit in SPSR
    IOReg   = SPSR;
    IOReg   = SPDR;
    CRITICAL_SECTION_END;
}

// Turn off the SPI hardware
static void __spiHWOff(SPI_ABSTRACT_BUS* spi){
	SPCR &= ~(1<<SPE);
}

static void __spiHWSetDataOrder(SPI_ABSTRACT_BUS* spi,SPI_DATA_ORDER order){
	if(order == SPI_DATA_ORDER_MSB){
		cbi(SPCR,DORD);
	}else{
		sbi(SPCR,DORD);
	}
}

static void __spiHWSetMode(SPI_ABSTRACT_BUS* spi,SPI_MODE mode){
	switch(mode){
	case SPI_MODE_0:
		cbi(SPCR,CPOL);
		cbi(SPCR,CPHA);
		break;
	case SPI_MODE_1:
		cbi(SPCR,CPOL);
		sbi(SPCR,CPHA);
		break;
	case SPI_MODE_2:
		sbi(SPCR,CPOL);
		cbi(SPCR,CPHA);
		break;
	case SPI_MODE_3:
		sbi(SPCR,CPOL);
		sbi(SPCR,CPHA);
		break;
	}
}

static void __spiHWSetClock(SPI_ABSTRACT_BUS* spi, SPI_CLOCK clock){
	cbi(SPCR,SPR1);
	cbi(SPCR,SPR0);
#ifdef SPI2X
	cbi(SPSR,SPI2X);
#endif
	switch(clock){
	case SPI_CLOCK_DIV2:
#ifdef SPI2X
		sbi(SPSR,SPI2X);
#endif
	case SPI_CLOCK_DIV4:
		break;

	case SPI_CLOCK_DIV8:
#ifdef SPI2X
		sbi(SPSR,SPI2X);
#endif
	case SPI_CLOCK_DIV16:
		sbi(SPCR,SPR0);
		break;

	case SPI_CLOCK_DIV32:
#ifdef SPI2X
		sbi(SPSR,SPI2X);
#endif
	case SPI_CLOCK_DIV64:
		sbi(SPCR,SPR1);
		break;

	case SPI_CLOCK_DIV128:
		sbi(SPCR,SPR0);
		sbi(SPCR,SPR1);
		break;
	}
}

static uint8_t __spiHWSendByte(SPI_ABSTRACT_BUS* spi, uint8_t data){
    SPDR  = data;          			// send Character
    while (!(SPSR & (1<<SPIF)));    // wait until Char is sent
    return SPDR;					// return the received byte
}
