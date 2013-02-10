#ifndef _AXON2_H_
#define _AXON2_H_

/* ------------------------------------------------------
 * $Id: axon2.h,v 1.17 2011/07/06 00:10:47 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: axon2.h,v $
 * Revision 1.17  2011/07/06 00:10:47  clivewebster
 * Add PROGMEM to LISTs
 *
 * Revision 1.16  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.15  2010/02/18 00:03:15  clivewebster
 * Changed marquee initialisation for Version 1.15
 *
 * Revision 1.14  2010/02/04 22:30:49  clivewebster
 * Stopped configure_ports from setting this to outputs unless they are hardwired to something on the board. Stops temporary short-circuits. WebbotLib makes things outputs when needed anyway.
 *
 * Revision 1.13  2010/01/24 17:30:37  clivewebster
 * *** empty log message ***
 *
 * Revision 1.12  2010/01/17 02:43:11  clivewebster
 * *** empty log message ***
 *
 * Revision 1.11  2009/12/27 18:22:39  clivewebster
 * *** empty log message ***
 *
 * Revision 1.10  2009/12/11 17:18:29  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.9  2009/11/16 03:44:05  clivewebster
 * *** empty log message ***
 *
 * Revision 1.8  2009/11/01 00:53:52  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/10/26 19:02:12  clivewebster
 * *** empty log message ***
 *
 * Revision 1.6  2009/10/11 23:36:57  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/10/10 20:15:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========

	AxonII board

	Runs at 16Mhz ATMega640

	20 June 09 - in 'configure_ports' changed the UART transmit
				 pins so that they are NOT set as output pins as this
				 causes a spurious character to sent at power on when
				 using level shifters etc.
-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif


#define THE_SYSTEM AXON2

/*
	We are compiling for the Axon2
	So we can make certain assumptions: ATMega640 running at 16MHz

	Hardware specific calls:
		void LED_on()  - turn on  the status LED - the decimal point on the LED display
		void LED_off() - turn off the status LED - the decimal point on the LED display

		boolean button_pressed(void) - has the surface mounted button on the board been pressed?

		void led_put_digit(uint8_t byte) - put a digit to the AxonII display.
*/


#ifndef F_CPU
#warning No CPU speed specified - assuming Axon is running at 16MHz
#define F_CPU 16000000
#endif

#if F_CPU != 16000000
#warning "AxonII normally runs at 16MHz but you have defined a different value"
#endif

#if defined (__AVR_ATmega640__)
#else
#	error When using the AxonII you must set the device to the ATMega640
#endif



#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;


// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>
// Now #undef any pins that are not connected...

// PORTB is used for programmer
// Remove SPI pins - may need to put back as they are available on the ISP programmer header
//#undef PB0
//#undef PB1
//#undef PB2
//#undef PB3

// Remove pins used by UART0
//#undef PE0
//#undef PE1

// Remove pins used by UART1
//#undef PD2
//#undef PD3

// Remove pins used by UART2
//#undef PH0
//#undef PH1

// Remove pins used by UART3
//#undef  PJ0
//#undef PJ1



/**

	Each system must define this method. It is called
	by the main in core.h just before entering the
	applications own 'control' method

	Set up the I/O ports with their standard settings

	Original code by Admin @ www.societyofrobots.com
*/
void __axon2_ports(void){

	//ANALOG PORTS
	//useful for sensors, SharpIR, photoresistors, etc.
	//analog ports can be configured as digital ports if desired
	DDRF = 0b00000000;  //configure all F ports for input				0x00
	PORTF = 0b00000000; //make sure pull-up resistors are turned off	0x00
	DDRK = 0b00000000;  //configure all K ports for input				0x00
	PORTK = 0b00000000; //make sure pull-up resistors are turned off	0x00

	//DIGITAL PORTS
	//useful for servos, PWM, LED's, UART, interrupts, timers
	DDRA = 0b00000000;  //configure ports for input
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
	DDRB = 0b00000000;  //configure ports for input
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	DDRC = 0b00111111;  //configure ports for output
	//       ||||||||
	//       |||||||\___0:  LED Bottom right
	//       ||||||\____1:  Status LED
	//       |||||\_____2:  LED top right
	//       ||||\______3:	LED Top
	//       |||\_______4:	LED Top left
	//       ||\________5:  LED middle
	//       |\_________6:
	//       \__________7:
	DDRD = 0b11010000;  //configure ports for output, except Rx and button
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2: serial RXD1, input
	//       ||||\______3: serial TXD1, input
	//       |||\_______4:
	//       ||\________5: button
	//       |\_________6: LED bottom
	//       \__________7: LED bottom left
	sbi(PORTD, PD5);  // enable pull-up resistor for button

	DDRE = 0b00000000;  //configure ports for inpu
	//       ||||||||
	//       |||||||\___0: serial RXD0, input
	//       ||||||\____1: serial TXD0, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:

	DDRG = 0b00000000;  //configure ports for input
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:

	DDRH = 0b00000000;  //configure ports for input
	//       ||||||||
	//       |||||||\___0: serial RXD2, input
	//       ||||||\____1: serial TXD2, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	DDRJ = 0b00000000;  //configure ports for input
	//       ||||||||
	//       |||||||\___0: serial RXD3, input
	//       ||||||\____1: serial TXD3, input
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:
	DDRL = 0b00000000;  //configure ports for input
	//       ||||||||
	//       |||||||\___0:
	//       ||||||\____1:
	//       |||||\_____2:
	//       ||||\______3:
	//       |||\_______4:
	//       ||\________5:
	//       |\_________6:
	//       \__________7:

};






// ... before calling
 #include "../device.h"

void led_off();
void configure_ports(void){
 	__axon2_ports();
 	led_off();
 }

// ---------------   End: define IO pins ---------------------



#include "../timer.h"

// ---------------   Start: define Status LED on the board --------------
void registerLED(void){
	// Register the built in LED
	statusLEDregister(C1, FALSE);
}
// ---------------   End: define Status LED on the board ----------------


// -----------   Start: define the 7 seg LED ------
/*
#include "../led.h"

// give them names based on their position
LED led_top       		= MAKE_LED(C3,FALSE);
LED led_top_left  		= MAKE_LED(C4,FALSE);
LED led_top_right 		= MAKE_LED(C2,FALSE);
LED led_middle 			= MAKE_LED(C5,FALSE);
LED led_bottom_left		= MAKE_LED(D7,FALSE);
LED led_bottom_right	= MAKE_LED(C0,FALSE);
LED led_bottom			= MAKE_LED(D6,FALSE);

// give them synonyms according to Admins diagram
#define led_a &led_top
#define led_b &led_top_right
#define led_c &led_bottom_right
#define led_d &led_bottom
#define led_e &led_bottom_left
#define led_f &led_top_left
#define led_g &led_middle
*/

#include "../segled.h"
SEGLED led_display = MAKE_SEGLED(C3,C2,C0,D6,D7,C4,C5,null,FALSE);

MAKE_WRITER(led_put_char){
	segled_put_char(&led_display,byte);
	return byte;
}

void led_off(){
	led_put_char(' ');
}

// Create list of 7 segment leds. There is only 1 on the Axon II
SEGLED_LIST PROGMEM _leds_[] = {&led_display};

// Create a marquee to scroll text along these leds
MAKE_WRITER(marquee_put_char);
MARQUEE marquee = MAKE_MARQUEE(_leds_, (TICK_COUNT)600000, (TICK_COUNT)2000000, &marquee_put_char);
MAKE_WRITER(marquee_put_char){
	return marqueeSendByte(&marquee,byte);
}



// -----------     End: define each LED that makes up the 7 seg LED ------



// ---------------   Start: define Button on the board ---------------------
// 	Original code by Admin @ www.societyofrobots.com

#define button_pressed() pin_is_low(D5)
// ---------------   End: define Button on the board ---------------------


// Initialise any other hardware, like motor drivers etc, built onto the board
#include "../rprintf.h"
void sysInitHardware(void){
	Writer writer = marqueeGetWriter(&marquee);
	// Set rprintf ...
	rprintfInit(writer);
	// .. and the error log to go to the marquee
	setErrorLog(writer);
}


#endif
