#ifndef _BABY_ORGANUTAN_B328_H_
#define _BABY_ORGANUTAN_B328_H_

/* ------------------------------------------------------
 * $Id: babyOrangutanB328.h,v 1.6 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: babyOrangutanB328.h,v $
 * Revision 1.6  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.5  2011/07/06 00:10:47  clivewebster
 * Add PROGMEM to LISTs
 *
 * Revision 1.4  2010/09/22 16:09:22  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.2  2009/10/27 20:48:15  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2009/10/15 02:58:22  clivewebster
 * *** empty log message ***
 *
 * ===========

	Baby Orangutan board B-328

	Runs at 20Mhz ATMega328

	Has integral TB6612FNG motor driver.
	Motor 1 uses PB3 and PD3
	Motor 2 uses PD5 and PD6

	In order to keep code as small as possible then these motors, by default, are not available (some projects may not need them!).
	If you want to use the motor drivers then add:
		#define USE_ON_BOARD_MOTORS
	to your program before including this file

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM BABYORANGUTAN_B328

#ifndef F_CPU
#warning No CPU speed specified - assuming Baby Orangutan B328 is running at 20MHz
#define F_CPU 20000000
#endif

#if F_CPU != 20000000
#warning Baby Orangutan B328 normally runs at 20MHz but you have defined a different value
#endif

#if defined (__AVR_ATmega328P__)
#else
#	error When using the Baby Orangutan 328P you must set the device to the ATMega328P
#endif



#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;


// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>


void __ports(void){
};



#undef PB6	// Used by the 20MHz crystal
#undef PB7	// Used by the 20MHz crystal
#undef PC6	// Used by reset
#undef PC7	// Attached to trimmer pot


// ... before calling
 #include "../device.h"


/**

	Each system must define this method. It is called
	by the main in core.h just before entering the
	applications own 'control' method

	Set up the I/O ports with their standard settings

*/
void configure_ports(void){
 	__ports();
 }


// ---------------   End: define IO pins ---------------------


// Define common info for timers
#include "../timer.h"

void registerLED(void){
	// Register the built in LED
	statusLEDregister(D1, TRUE);
}

// Has an on-board TB6612FNG motor controller
#ifdef USE_ON_BOARD_MOTORS
#include "../Motors/Toshiba/TB6612FNG.h"
TOSHIBA_TB6612FNG_2pin_MOTOR Motor1 = MAKE_TOSHIBA_TB6612FNG_MOTOR_2pin(FALSE, D6, D5);
TOSHIBA_TB6612FNG_2pin_MOTOR Motor2 = MAKE_TOSHIBA_TB6612FNG_MOTOR_2pin(FALSE, B3, D3);
TOSHIBA_TB6612FNG_2pin_MOTOR_LIST PROGMEM onboard_motors[] = {&Motor1, &Motor2};
TOSHIBA_TB6612FNG_2pin_MOTOR_DRIVER onboard_driver = MAKE_TOSHIBA_TB6612FNG_2pin_MOTOR_DRIVER(onboard_motors);
	#undef B3
	#undef D3
	#undef D5
	#undef D6
#endif



// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
#ifdef USE_ON_BOARD_MOTORS
	toshibaTB6612FNG_2pin_Init(&onboard_driver, 10000);
#endif
}

#endif
