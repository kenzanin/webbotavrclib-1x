/*
 * $Id: atmega2561.h,v 1.2 2010/07/15 14:57:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: atmega2561.h,v $
 * Revision 1.2  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.1  2010/02/09 16:44:04  clivewebster
 * Added ATMega2561
 *
 * ===========
 *
 *
 * atmega2561.h
 *
 *  Created on: 09-Feb-2010
 *      Author: Clive Webster
 *
 	A generic ATMega 2561 configuration

	NB This leaves the direction of each port in their
	initial state - which shoud be considered as unknown

	If using iopin.h then it is assumed that ALL io pins
	will have headers - ie they are all available to the program

 *
 */

#ifndef ATMEGA2561_H_
#define ATMEGA2561_H_

#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA2561

/*
	We are compiling for the ATMEGA2561
	So we can make certain assumptions: ATMega2561 but dot know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 16MHz
#define F_CPU 16000000
#endif


#if !defined (__AVR_ATmega2561__)
#error If including ATMega2561.h you must set the compiler to use the device ATMega2561
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



#endif /* ATMEGA2561_H_ */
