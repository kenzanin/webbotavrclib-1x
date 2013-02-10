#ifndef _SERVOS_H_
#define _SERVOS_H_
#include "actuators.h"
#include "_uart_common.h"
#include "i2cBus.h"
/* -------------------------------------------------
 * $Id: servos.h,v 1.13 2011/07/06 00:28:45 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: servos.h,v $
 * Revision 1.13  2011/07/06 00:28:45  clivewebster
 * Add PROGMEM to SERVO_LIST
 *
 * Revision 1.12  2011/02/21 20:29:31  clivewebster
 * Added muxServo
 *
 * Revision 1.11  2010/09/30 17:00:05  clivewebster
 * Refactored so that specialist info is now in a union to reduce memory. Also added an I2C_SERVO_DRIVER
 *
 * Revision 1.10  2010/07/15 14:46:45  clivewebster
 * *** empty log message ***
 *
 * Revision 1.9  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/05/29 15:51:03  clivewebster
 * Added servosCenter
 *
 * Revision 1.7  2010/05/17 19:36:01  clivewebster
 * Update comments
 *
 * Revision 1.6  2010/03/19 01:50:11  clivewebster
 * Improve software PWM handling
 *
 * Revision 1.5  2010/03/07 19:30:04  clivewebster
 * Updated comments
 *
 * Revision 1.4  2010/02/06 13:26:04  clivewebster
 * Allow setServoConfig to change the settings for a servo
 *
 * Revision 1.3  2009/10/09 17:41:29  clivewebster
 * *** empty log message ***
 *
 * ===========
 *
 * Copyright (C) 2010 Clive Webster (webbot@webbot.org.uk)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
	A servo controller that uses interrupt driven routines
	to control the speed of servos in the background.

	Each driver can cope with up to about 10 servos but you
	can have multiple controllers

#include "servos"

// Define two servos in bank 1
SERVO left  = MAKE_SERVO(FALSE, D1,1500, 500);
SERVO right = MAKE_SERVO(TRUE , D2,1500, 50);
SERVO* PROGMEM servos1[] = {&left,&right};
SERVO_DRIVER bank1 = MAKE_SERVO_DRIVER(servos1);

// Define two servos in bank 2
SERVO top    = MAKE_SERVO(FALSE, D1,1500, 500);
SERVO bottom = MAKE_SERVO(TRUE , D2,1500, 50);
SERVO* PROGMEM servos2[] = {&top,&bottom};
SERVO_DRIVER bank2 = MAKE_SERVO_DRIVER(servos2);

void appInit(void){
	// Initialise the servo controllers
	servosInit(&bank1,TIMER1);
	servosInit(&bank2,TIMER3);
}



   ------------------------------------------------- */

/*----------------------------------------------------------------------------------------
*
*	Servo
*
*	Holds all data required by a servo
*
-----------------------------------------------------------------------------------------*/
// Servos need updating every 20ms
#define SERVO_CYCLE 20

// Fwd defs
struct s_servo_driver;

/* -------------------------------------------------------
 * Define the structure for one servo
-------------------------------------------------------- */
typedef struct s_servo{
	__ACTUATOR 			actuator;	// has all the common stuff
	struct s_servo_driver* driver;	// The driver it is attached to, set by init'ing the driver
	const IOPin * 		pin;		// The IO pin it is connected to

	// If you need to change these settings at runtime then use servoSetConfig
	uint16_t			center_us;	// The number of microseconds pulse length to center the servo. On ideal servos it will be 1500. ie 1,5ms
	uint16_t			range_us;	// The number of microseconds either side of center for full range. On ideal servos it be 500. Range = 1ms to 2ms.

	// These are scratch variables used by the driver - DO NOT modify by hand
	uint16_t			min_ticks;	// The no of timer ticks for minimum rotation
	uint16_t			max_ticks;  // The no of timer ticks for maximum rotation
	uint16_t			delay;		// The current delay for the timer
	uint16_t			top;		// The value of TOP for the timer it uses
} SERVO;

// Define the standard constructor for a servo connected to an IOPin
#define MAKE_SERVO(inverted, iopin, center, range)  { MAKE_ACTUATOR(inverted),null,iopin,center, range, 0,0,0 }

// Define the standard constructor for a servo connected to a controller
#define MAKE_REMOTE_SERVO(inverted, center, range)  { MAKE_ACTUATOR(inverted),null,null,center, range, 0,0,0 }

void servoSetConfig(SERVO* servo, uint16_t center, uint16_t range);

/* -------------------------------------------------------
 * Define software protocols for serial servo controllers
-------------------------------------------------------- */
typedef enum e_servo_protocol {
	MINI_SSC, POLOLU_COMPACT
} SERVO_PROTOCOL;


/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef SERVO* /*PROGMEM*/  SERVO_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
-------------------------------------------------------- */
#define NUM_MUX_PINS 3

typedef struct s_servo_driver{
	SERVO_LIST* const servos;		// The servos
	uint8_t num_servos; 			// The total number of servos
	boolean servos_init;			// Have the servos been initialised

	// This should be in the union but lots of problems...don't ask
	I2C_DEVICE i2cInfo;				// The i2c details for the driver

	// Make this the last thing
	union {

		// Extra Info for a serial servo driver
		struct {
			UART*   uart;					// The uart it uses if serial servo
			SERVO_PROTOCOL protocol;		// The protocol it uses if serial servo
		} serial;

		// Extra Info for a software PWM servo driver
		struct {
			uint8_t	currentServo;			// The current servo being worked on
			uint16_t total_delay;			// The total delay so far
			uint16_t servo_cycle;			// The number of timer ticks that represent SERVO_CYCLE
		} softwarePWM;

		// Extra Info when using a multiplexer
		struct {
			uint8_t	 currentServo;			// The current servo being worked on
			uint16_t servo_cycle;			// The value of TOP that represents 2.5ms
			const IOPin* muxPins[NUM_MUX_PINS];	// The multiplexer pins
			PORT 	pwmPort;					// Port for setting the duty cycle
		} softwareMUX;
	} specific;

} SERVO_DRIVER;

#define MAKE_I2C_SERVO_DRIVER(i2cAddr, servolst) { \
	servolst, \
	(uint8_t)(sizeof(servolst)/sizeof(SERVO*)), \
	FALSE, \
	MAKE_I2C_DEVICE((i2cAddr) & 0xfe) \
}

#define MAKE_SERVO_DRIVER(servolst) { \
	servolst, \
	(uint8_t)(sizeof(servolst)/sizeof(SERVO*)), \
	FALSE, \
	MAKE_I2C_DEVICE(0) \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */


// Pass the list of servos, the list should be in PROGMEM space to save Flash RAM
void servosInit(SERVO_DRIVER* driver, const Timer* timer);

// Initialise the servos using PWM. Each servo needs to be connected to a PWM output pin
void servoPWMInit(SERVO_DRIVER* driver);

// Initialise the servos using PWM via a de-multiplexer.
void servosMuxInit(SERVO_DRIVER* driver, const IOPin* pwm16, const IOPin* mux1, const IOPin* mux2, const IOPin* mux3);

// Initialise the serial servo driver
#define servoSerialInit(driver, uart, baud, protocol) _servoSerialInit(driver,&((uart)->_uart_),baud,protocol)
void _servoSerialInit(SERVO_DRIVER* driver, UART* uart, BAUD_RATE baud, SERVO_PROTOCOL protocol);

// Set all of the servo to be connected/disconnected for this bank of servos
void servosSetConnected(const SERVO_DRIVER* driver, boolean connect);
#define servosConnect(driver) servosSetConnected(driver,TRUE)
#define servosDisconnect(driver) servosSetConnected(driver,FALSE)

// Set all of the servo to the same speed for this bank of servos
void servosSetSpeed(const SERVO_DRIVER* driver, DRIVE_SPEED speed);


// A terminal program to help you find the center and range positions
void _servosCenter(SERVO_LIST* const servos, uint8_t numServos, UART* uart);
#define servosCenter(servos,uart) _servosCenter(servos, (uint8_t)(sizeof(servos)/sizeof(SERVO*)), &((uart)->_uart_))

#endif


