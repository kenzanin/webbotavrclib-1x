#ifndef _AXON_H_
#define _AXON_H_

/* ------------------------------------------------------
 * $Id: axon.h,v 1.10 2010/07/15 14:57:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: axon.h,v $
 * Revision 1.10  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.9  2010/03/07 19:18:31  clivewebster
 * Make SPI pins on the programmer available for use
 *
 * Revision 1.8  2010/02/04 22:30:49  clivewebster
 * Stopped configure_ports from setting this to outputs unless they are hardwired to something on the board. Stops temporary short-circuits. WebbotLib makes things outputs when needed anyway.
 *
 * Revision 1.7  2009/11/01 00:53:42  clivewebster
 * *** empty log message ***
 *
 * Revision 1.6  2009/10/26 19:02:12  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========

	Axon board

	Runs at 16Mhz ATMega640

	20 June 09 - in 'configure_ports' changed the UART transmit
				 pins so that they are NOT set as output pins as this
				 causes a spurious character to be sent at power on when
				 using level shifters etc.

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif


#define THE_SYSTEM AXON


/*
	We are compiling for the Axon
	So we can make certain assumptions: ATMega640 running at 16MHz

	Hardware specific calls:
		void LED_on()  - turn on  the green status LED surface mounted on the board
		void LED_off() - turn off the green status LED surface mounted on the board
		boolean button_pressed(void) - has the surface mounted button on the board been pressed?

 *
 *  PWM Channels
 *
 *  		AVR
 * Channel	Pin		Axon
 * OC0A  	B7 	(not connected)
 * OC0B  	G5 	(attached to button)
 * OC1A  	B5 	(not connected)
 * OC1B  	B6 	(attached to green LED)
 * OC1C  	B7 	(not connected)
 * OC2A  	B4 	(not connected)
 * OC2B  	H6
 * OC3A  	E3
 * OC3B  	E4
 * OC3C  	E5
 * OC4A  	H3
 * OC4B  	H4
 * OC4C  	H5
 * OC5A  	L3 	(not connected)
 * OC5B  	L4 	(not connected)
 * OC5C  	L5 	(not connected)
 * So there is one 8bit channel on H6, and 6 x 16bit channels on E3,E4,E5, H3,H4,H5


*/


#ifndef F_CPU
#warning No CPU speed specified - assuming Axon is running at 16MHz
#define F_CPU 16000000
#endif

#if F_CPU != 16000000
#warning "Axon normally runs at 16MHz but you have defined a different value"
#endif

#if defined (__AVR_ATmega640__)
#else
#	error When using the Axon you must set the device to the ATMega640
#endif



#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;

// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>
// Now #undef any pins that are not connected...

// PORTL has no headers
#undef PORTL

// PORTB is used for programmer, PB4,5,7 aren't connected,PB6=LED
//#undef PB0      SS
//#undef PB1	  SPI SCK
//#undef PB2	  SPI MOSI
//#undef PB3	  SPI MISO
#undef PB4
#undef PB5
#undef PB7

// PORTG is unused except for PG5 - the input button
#undef PG0
#undef PG1
#undef PG2
#undef PG3
#undef PG4
#undef PG6
#undef PG7

#undef PD4
#undef PD5
#undef PD6
#undef PD7

// Remove pins used by UART0
// #undef PE0
// #undef PE1

// Remove pins used by UART1
// #undef PD2
// #undef PD3

// Remove pins used by UART2
// #undef PH0
// #undef PH1

// Remove pins used by UART3
// #undef PJ0
// #undef PJ1


/* --- PWM pins -------
Timer0   OC0A, OC0B ( 8 BIT) = pins    	PB7	,	PG5
Timer1   OC1A, OC1B (16 BIT)			PB5 ,	PB6
Timer2   OC2A, OC2B ( 8 BIT)			PB4	,	PH6
Timer3   OC3A, OC3B (16 BIT)			PE3,	PE4			PE5=OC3C
Timer4   OC4A, OC4B (16 BIT)			PH3,	PH4			PH5=OC4C
Timer5   OC5A, OC5B (16 BIT)			PL3,	PL4			PL5=OC5C
*/

/**

	Each system must define this method. It is called
	by the main in core.h just before entering the
	applications own 'control' method

	Set up the I/O ports with their standard settings

	Original code by Admin @ www.societyofrobots.com

*/
void __axon_ports(void){

	//examples: configure ports for input or output
	//DDRB = 0x1F;  //configure B ports 0->4 for output (google search '0b00011111 to hex')
	//DDRC = 0x7F;  //configure all C ports for output, except 7 (0b01111111 to hex)
	//DDRD = 0xFE;  //configure all D ports for output, except 0 and 1 (RX/TX) FC
    //DDRC = _BV (PC0); //PC0 is digital output
	//0b76543210 <- port # order, 1 for output, 0 for input
	//to convert from binary to hex, google search '0b00011111 to hex'
	//DDRG &= ~_BV(PG5); //make port G5 an input pin
	//cbi(DDRG, PG5);  //set G5 as input pin

	//ANALOG PORTS
	//useful for sensors, SharpIR, photoresistors, etc.
	//analog ports can be configured as digital ports if desired
	DDRF  = 0b00000000;  //configure all F ports for input				0x00
	PORTF = 0b00000000;  //make sure pull-up resistors are turned off	0x00
	DDRK  = 0b00000000;  //configure all K ports for input				0x00
	PORTK = 0b00000000;  //make sure pull-up resistors are turned off	0x00

	//DIGITAL PORTS
	//useful for servos, PWM, LED's, UART, interrupts, timers
	DDRA = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	//PORTB reserved for programmer (use programmer pins if you know what you are doing)
	DDRB = _BV (PB6);	//PB6 is LED, hold low to turn it on
	DDRC = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	DDRD = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2: serial RXD1, input
	//       ||||\______3: serial TXD1, input
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7: timer0
	DDRE = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD0, input
	//       ||||||\____1: serial TXD0, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	//cbi(PORTG, PG5);  // disable pull-up resistor for v1e and earlier
	sbi(PORTG, PG5);  // enable pull-up resistor for v1f and later
	cbi(DDRG, PG5);	//PG5 is for the button, make a digital input

	//note that button actions are inverted between both versions!!!
	DDRH = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD2, input
	//       ||||||\____1: serial TXD2, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:

	DDRJ = 0b00000000;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD3, input
	//       ||||||\____1: serial TXD3, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	//PORTL has no headers





	};






// ... before calling
 #include "../device.h"

 void configure_ports(void){
 	__axon_ports();
 }

// ---------------   End: define IO pins ---------------------



#include "../timer.h"


void registerLED(void){
	// Register the built in LED
	statusLEDregister(B6, FALSE);
}



// ---------------   Start: define Button on the board ---------------------
// Original code by Admin @ www.societyofrobots.com
#define button_pressed() pin_is_low(G5)
// ---------------   End: define LED and Button on the board ---------------------


// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
}

#endif
