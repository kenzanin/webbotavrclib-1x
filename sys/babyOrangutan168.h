#ifndef _BABY_ORANG_168_H_
#define _BABY_ORANG_168_H_

/* ------------------------------------------------------
 * $Id: babyOrangutan168.h,v 1.8 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: babyOrangutan168.h,v $
 * Revision 1.8  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.7  2011/07/06 00:10:47  clivewebster
 * Add PROGMEM to LISTs
 *
 * Revision 1.6  2010/07/15 14:57:21  clivewebster
 * Added AVcc_MV
 *
 * Revision 1.5  2009/10/09 18:39:35  clivewebster
 * Update comments
 *
 * Revision 1.4  2009/10/09 18:37:21  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/10/09 17:23:22  clivewebster
 * Add sysInitHardware
 *
 * ===========

	Baby Orangutan board

	Runs at 20Mhz ATMega168
	This device is no longer manufactured but you can tell the different as this board is
	green whereas the new ones are blue.

	Has integral LB1836M small motor driver.
	Motor 1 uses PB1 for PWM and PD5 for direction
	Motor 2 uses PB2 for PWM and PD6 for direction

	In order to keep code as small as possible then these motors, by default, are not available (some projects may not need them!).
	If you want to use the motor drivers then add:
		#define USE_ON_BOARD_MOTORS
	to your program before including this file

-------------------------------------------------------*/
#ifdef THE_SYSTEM
#error You must only include one system file
#endif

#define THE_SYSTEM BABYORANGUTAN168

#ifndef F_CPU
#warning No CPU speed specified - assuming Baby Orangutan is running at 20MHz
#define F_CPU 20000000
#endif

#if F_CPU != 20000000
#warning Baby Orangutan 168 normally runs at 20MHz but you have defined a different value
#endif

#if defined (__AVR_ATmega168__)
#else
#	error When using the Baby Orangutan 168 you must set the device to the ATMega168
#endif



#include "../core.h"

// The ADC uses 5v
const uint16_t PROGMEM AVcc_MV = 5000;

// ---------------   Start: define IO pins ---------------------
#include <avr/io.h>



void __ports(void){
};


// Now #undef any pins that are not connected...
//	#undef PB1	// Used by motor 1
//	#undef PB2	// Used by motor 2
	#undef PB6	// Used by the 20MHz crystal
	#undef PB7	// Used by the 20MHz crystal
	#undef PC6	// Used by reset
	#undef PC7	// Attached to trimmer pot
//	#undef PD5	// Used by motor 1
//	#undef PD6	// Used by motor 2




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
// 	motorPWMInit(&driver);
 }


// ---------------   End: define IO pins ---------------------






// Define common info for timers
#include "../timer.h"

void registerLED(void){
	// Register the built in LED
	statusLEDregister(D1, TRUE);
}

// Has an on-board LB1836M motor controller
#ifdef USE_ON_BOARD_MOTORS
#include "../Motors/Sanyo/LB1836M.h"
SANYO_LB1836M_MOTOR Motor1 = MAKE_SANYO_LB1836M_MOTOR(FALSE, B1, D5);
SANYO_LB1836M_MOTOR Motor2 = MAKE_SANYO_LB1836M_MOTOR(FALSE, B2, D6);
SANYO_LB1836M_MOTOR_LIST PROGMEM onboard_motors[] = {&Motor1, &Motor2};
SANYO_LB1836M_MOTOR_DRIVER onboard_driver = MAKE_SANYO_LB1836M_MOTOR_DRIVER(onboard_motors);
	#undef B1
	#undef B2
	#undef D5
	#undef D6
#endif


// Initialise any other hardware, like motor drivers etc, built onto the board
void sysInitHardware(void){
#ifdef USE_ON_BOARD_MOTORS
	// Set PWM frequency to 10kHz
	sanyoLB1836M_Init(&onboard_driver,10000UL);
#endif
}


#endif
