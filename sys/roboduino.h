/*
 * $Id: roboduino.h,v 1.5 2010/07/15 14:57:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: roboduino.h,v $
 * Revision 1.5  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.4  2010/02/04 22:30:45  clivewebster
 * Stopped configure_ports from setting this to outputs unless they are hardwired to something on the board. Stops temporary short-circuits. WebbotLib makes things outputs when needed anyway.
 *
 * Revision 1.3  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========
 * roboduino.h
 *
 *  Created on: 02-Apr-2009
 *      Author: Clive Webster
 */

#ifndef ROBODUINO_H_
#define ROBODUINO_H_

/* ------------------------------------------------------

	Roboduino board

	Runs at 16Mhz ATMega168

	20 June 09 - in 'configure_ports' changed the UART transmit
				 pins so that they are NOT set as output pins as this
				 causes a spurious character to sent at power on when
				 using level shifters etc.

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif


#define THE_SYSTEM ROBODUINO

#ifndef F_CPU
#warning No CPU speed specified - assuming Roboduino is running at 16MHz
#define F_CPU 16000000
#endif

#if F_CPU != 16000000
#warning "Roboduino normally runs at 16MHz but you have defined a different value"
#endif

#if defined (__AVR_ATmega168__)
#else
#	error When using the Roboduino you must set the device to the ATMega168
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
	DDRC  = 0b00000000;  //configure all C ports for input
	PORTC = 0b00000000;  //make sure pull-up resistors are turned off

	DDRD  = 0b00000000;  //configure all D ports for output, except D0, D1 which is uart Rx,Tx
	PORTD = 0b00000000;  //turn off all pins or pull ups

	DDRB  = 0b00000000;  //configure all B ports for input
	PORTB = 0b00000000;  //turn off all pins
};



// ... before calling
#include "../device.h"
// ---------------   End: define IO pins ---------------------





// Define common info for timers
#include "../timer.h"



void registerLED(void){
	// Register the built in LED
	statusLEDregister(B5, TRUE);
}

// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
}

#endif /* ROBODUINO_H_ */
