/*
 * $Id:
 *
 * Revision History
 * ================
 * $Log:
 * ===========
 *
 *
 * atmega128rfa1.h
 *
 *  Created on: 2-Apr-2010
 *      Author: Clive Webster
 *
 	A generic ATMega 128RFA1 configuration

	NB This leaves the direction of each port in their
	initial state - which shoud be considered as unknown

	If using iopin.h then it is assumed that ALL io pins
	will have headers - ie they are all available to the program

 *
 */

#ifndef ATMEGA128RFA1_H_
#define ATMEGA128RFA1_H_

#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA128RFA1

/*
	We are compiling for the ATMEGA128RFA1
	So we can make certain assumptions: ATMega128RFA1 but dont know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 16MHz
#define F_CPU 16000000
#endif


#if !defined (__AVR_ATmega128RFA1__)
#error If including atmega128rfa1.h you must set the compiler to use the device ATMega128RFA1
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



#endif /* ATMEGA128RFA1_H_ */
