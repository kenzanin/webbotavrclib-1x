#ifndef _SYS_ATMEGA8_H_
#define _SYS_ATMEGA8_H_

/* ------------------------------------------------------
 * $Id: atmega8.h,v 1.4 2010/07/15 14:57:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: atmega8.h,v $
 * Revision 1.4  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.3  2009/11/01 00:53:26  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========

	A generic ATMega 168 configuration

	NB This leaves the direction of each port in their
	initial state - which shoud be considered as unknown

	If using iopin.h then it is assumed that ALL io pins
	will have headers - ie they are all available to the program

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM ATMEGA168

/*
	We are compiling for the ATMEGA168
	So we can make certain assumptions: ATMega168 but dot know the frequency


*/


#ifndef F_CPU
#warning No F_CPU speed specified - assuming running at 8MHz
#define F_CPU 8000000
#endif


#if !defined (__AVR_ATmega8__)
#error "If including ATMega8.h you must set the compiler to use the device ATMega8"
#endif

#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;


// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>
// Now #undef any pins that are not connected...




// ... before calling
#include "../device.h"
// ---------------   End: define IO pins ---------------------





// Define common info for timers
#include "../timer.h"




/**

	Each system must define this method. It is called
	by the main in core.h just before entering the
	applications own 'control' method

	Set up the I/O ports with their standard settings

*/
void configure_ports(void){
	};


void registerLED(void){
}
// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
}

#endif
