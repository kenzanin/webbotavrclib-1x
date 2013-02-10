/*
 * $Id: atmega1284P.h,v 1.1 2011/02/21 20:28:19 clivewebster Exp $
 * Revision History
 * ================
 * $Log: atmega1284P.h,v $
 * Revision 1.1  2011/02/21 20:28:19  clivewebster
 * Added ATMega1284P
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: atmega1284P.h
 *  Created on: 14 Feb 2011
 *      Author: Clive Webster
 */

#ifndef ATMEGA1284P_H_
#define ATMEGA1284P_H_
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA1284P

/*
	We are compiling for the ATMEGA1284P
	So we can make certain assumptions: ATMega1284P but dot know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 16MHz
#define F_CPU 16000000
#endif


#if !defined (__AVR_ATmega1284P__)
#error If including ATMega1284P.h you must set the compiler to use the device ATMega1284P
#endif

#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;


// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>
// Now #undef any pins that are not connected...


/**

	Each system must define this method. It is called
	by the main in core.h just before entering the
	applications own 'control' method

	Set up the I/O ports with their standard settings

*/
void configure_ports(void){
};



// ... before calling
#include "../device.h"
// ---------------   End: define IO pins ---------------------





// Define common info for timers
#include "../timer.h"





void registerLED(void){
}

// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
}



#endif /* ATMEGA1284P_H_ */
