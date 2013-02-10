/*
 * $Id: int6.h,v 1.2 2011/02/21 20:27:44 clivewebster Exp $
 * Revision History
 * ================
 * $Log: int6.h,v $
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
 *        File: int6.h
 *  Created on: 2 Aug 2010
 *      Author: Clive Webster
 */

#ifndef INT6_H_
#define INT6_H_
#ifdef BUILDING_LIBRARY
// Include the device definitions
#include <avr/io.h>


#if defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega128RFA1__)
	// Uses PORTE6 as the INT6 pin
	#define DDR_INT6	  DDRE
	#define PORT_INT6  PORTE
	#define PIN_INT6   PE6
    #define READ_INT6  PINE

	// The Interrupt Sense Control register
	#define ISC_INT6   EICRB
	// The low bit in INT_ISC
	#define ISCL_INT6  ISC60
	// The high bit in INT_ISC
	#define ISCH_INT6  ISC61

	// The interrupt end register and bit
	#define ENABLE_PORT_INT6	EIMSK
	#define ENABLE_BIT_INT6	INT6

	// The interrupt vector
	#define VECT_INT6	INT6_vect
#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega168__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega1284P__)
	// not supported
#else
#	error Processor not defined for INT6
#endif

#endif
#endif /* INT6_H_ */
