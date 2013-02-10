/*
 * $Id: int2.h,v 1.2 2011/02/21 20:27:44 clivewebster Exp $
 * Revision History
 * ================
 * $Log: int2.h,v $
 * Revision 1.2  2011/02/21 20:27:44  clivewebster
 * Added ATMega1284P
 *
 * Revision 1.1  2010/08/10 22:48:51  clivewebster
 * Created
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
 *        File: int2.h
 *  Created on: 2 Aug 2010
 *      Author: Clive Webster
 */

#ifndef INT2_H_
#define INT2_H_

#ifdef BUILDING_LIBRARY
// Include the device definitions
#include <avr/io.h>


#if defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega128RFA1__)
	// Uses PORTD2 as the INT2 pin
	#define DDR_INT2   DDRD
	#define PORT_INT2  PORTD
	#define PIN_INT2   PD2
    #define READ_INT2  PIND

	// The Interrupt Sense Control register
	#define ISC_INT2   EICRA
	// The low bit in INT_ISC
	#define ISCL_INT2  ISC20
	// The high bit in INT_ISC
	#define ISCH_INT2  ISC21

	// The interrupt end register and bit
	#define ENABLE_PORT_INT2	EIMSK
	#define ENABLE_BIT_INT2	INT2

	// The interrupt vector
	#define VECT_INT2	INT2_vect
#elif defined (__AVR_ATmega644__)  || defined (__AVR_ATmega1284P__)
	// Uses PORTB2 as the INT2 pin
	#define DDR_INT2	  DDRB
	#define PORT_INT2  PORTB
	#define PIN_INT2   PB2
    #define READ_INT2  PIND

// The Interrupt Sense Control register
	#define ISC_INT2   EICRA
	// The low bit in INT_ISC
	#define ISCL_INT2  ISC20
	// The high bit in INT_ISC
	#define ISCH_INT2  ISC21
	// The interrupt end register and bit
	#define ENABLE_PORT_INT2	EIMSK
	#define ENABLE_BIT_INT2	INT2

	// The interrupt vector
	#define VECT_INT2	INT2_vect

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega168__) || defined (__AVR_ATmega328P__)  || defined (__AVR_ATmega32__)
	// ATMega32 has INT2 - but it only supports rising edges
	// not supported
#else
#	error Processor not defined for INT2
#endif

#endif
#endif /* INT2_H_ */
