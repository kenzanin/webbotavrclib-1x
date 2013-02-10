#ifndef _ATMEGA2560_H_
#define _ATMEGA2560_H_

/* ------------------------------------------------------
 * $Id: atmega2560.h,v 1.4 2010/07/15 14:57:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: atmega2560.h,v $
 * Revision 1.4  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.3  2009/11/01 00:52:25  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========

	A generic ATMega 2560 configuration

	NB This leaves the direction of each port in their
	initial state - which shoud be considered as unknown

	If using iopin.h then it is assumed that ALL io pins
	will have headers - ie they are all available to the program

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA2560

/*
	We are compiling for the ATMEGA2560
	So we can make certain assumptions: ATMega2560 but dot know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 8MHz
#define F_CPU 8000000
#endif


#if !defined (__AVR_ATmega2560__)
#error If including ATMega2560.h you must set the compiler to use the device ATMega2560
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
