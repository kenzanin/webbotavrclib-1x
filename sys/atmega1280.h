#ifndef _ATMEGA1280_H_
#define _ATMEGA1280_H_

/* ------------------------------------------------------
 * $Id:
 *
 * Revision History
 * ================
 * $Log:
 * ===========

	A generic ATMega 1280 configuration

	NB This leaves the direction of each port in their
	initial state - which shoud be considered as unknown

	If using iopin.h then it is assumed that ALL io pins
	will have headers - ie they are all available to the program

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA1280

/*
	We are compiling for the ATMEGA1280
	So we can make certain assumptions: ATMega1280 but dot know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 8MHz
#define F_CPU 8000000
#endif


#if !defined (__AVR_ATmega1280__)
#error If including ATMega1280.h you must set the compiler to use the device ATMega1280
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

#endif
