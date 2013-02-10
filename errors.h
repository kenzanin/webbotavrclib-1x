/*
 * $Id: errors.h,v 1.13 2010/09/29 18:15:06 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: errors.h,v $
 * Revision 1.13  2010/09/29 18:15:06  clivewebster
 * Added new codes for an I2C bus 31/32
 *
 * Revision 1.12  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.11  2010/05/09 22:08:47  clivewebster
 * *** empty log message ***
 *
 * Revision 1.10  2009/11/02 18:56:42  clivewebster
 * Added revision log
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
 */
#ifndef _ERRORS_H
#define _ERRORS_H

#include "libdefs.h"
/* ----------------------------------------------------------------------------------------


	Error functionality.

	This is used to visually indicate a runtime error condition. This can help when
	no other display is connected to the system.

	Setting an error code will only have a result if there is no existing error code.

	An error code of '0' means no error. Negative Error codes should only be used by the
	library. Positive error codes can be used by your own program.Negative errors result
	in quick flashes, whereas positive errors result in longer flashes. The LED will blink
	n times (where n=abs(errorNo) ) followed by a short delay.

	Error codes are displayed via the LED set by the LEDregister command. If this is not
	called then this logic will do nothing.

	An error number is defined using the datatype ERROR_CODE.

	void setError(ERROR_CODE err);  // set an error code
	ERROR_CODE getError(void);		// get the current error code

---------------------------------------------------------------------------------------- */


typedef struct s_error{
	volatile ERROR_CODE errorCode;
	volatile ERROR_CODE remaining;
	volatile boolean    phase:1;
	Writer 				output;
} ERROR;



ERROR_CODE getError(void);

// forward definition
void setError(ERROR_CODE err);
void setErrorLog(Writer log);




// The scheduler has run out of job space in the queue
// Consider increasing SCHEDULER_MAX_JOBS by defining it in your own program
// before including the system file
// #define SCHEDULER_MAX_JOBS 20 // Set the max number of scheduled jobs
// #include "sys/axon.h"   		 // Include the system file for the AXON
#define SCHEDULER_EXHAUSTED -1

// The compare value set for an 8 bit timer
// is not an 8 bit value
#define TIMER_COMPARE_NOT_8_BIT -2

//
//  You have tried to use timerCompareAttach but the timer doesn't
//  support that mode
//
#define TIMER_COMPARE_NOT_SUPPORTED -3

//
// Try to call timerOverflowAttach for a timer when it already has a callback
//
#define TIMER_OVERFLOW_CALLBACK_EXISTS -4
//
// Try to call timerCompareAttach for a timer when it already has a callback
//
#define TIMER_COMPARE_CALLBACK_EXISTS -5

// Attempt to set the prescaler of a timer to an unsupported value
#define TIMER_PRESCALER_UNSUPPORTED -6

// servos.h requires a 16 bit timer
#define SERVO_TIMER_MUST_BE_16_BIT -7

// There is no appropriate prescale value for the frequency you want
#define NO_APPROPRIATE_TIMER_PRESCALE_VALUES -8

// You have called pin_high, pin_low, or pin_set on a pin that is currently an input pin.
// Solution: Use pin_make_output to make it an output pin before this call or your system
// file could make it an output in 'configure_ports'
#define PIN_SET_ON_INPUT -9

// You have called timerSetMode with a mode that is not supported by the timer
#define TIMER_MODE_NOT_SUPPORTED -10

// The receive character UART buffer has overflowed. Try increasing it.
#define UART_RECEIVE_OVERFLOW -11

// The PWM pin is not available
#define PWM_PIN_NOT_AVAILABLE -12

// The PWM pin is already in use
#define PWM_PIN_IN_USE -13

// The Timer for the PWM pin is already in use
#define PWM_TIMER_IN_USE -14

// The Timer for the PWM pin does not support PWM mode via ICR
#define PWM_TIMER_MODE_PWM_FAST_ICR_UNSUPPORTED -15

// The I2C buffer is too small to receive the message
// Increase the buffer size by using a
// #define I2C_RECEIVE_DATA_BUFFER_SIZE	64
// at the start of your program - where '64' is the length you need
#define I2C_RECEIVE_BUFFER_OVERFLOW -16

// The I2C buffer is too small to send the message
// Increase the buffer size by using a
// #define I2C_SEND_DATA_BUFFER_SIZE	64
// at the start of your program - where '64' is the length you need
#define I2C_SEND_BUFFER_OVERFLOW -17

// You have asked a2d to do a conversion on an unknown pin
#define INVALID_ADC_IO_PIN  -18

// MotorPWM requires a timer that supports phase and frequency correct PWM using ICR
#define PWM_TIMER_MODE_PWM_PHASE_FREQ_ICR_UNSUPPORTED -19

// You have called pin_change_attach with an IOPin that has no PCINT associated
#define PIN_HAS_NO_PCINT -20

// You have called pin_change_attach for a pin that already has a callback
#define PIN_HAS_CALLBACK -21

//
// Try to call timerCaptureAttach for a timer when it already has a callback
//
#define TIMER_CAPTURE_CALLBACK_EXISTS -22


// The software uart receive pin is not the input capture pin for the specific timer
#define UARTSW_NOT_CAPTURE -23

// The uart can only be used as a master, and the hardware uart must support SPI mode, and the
// additional clock pin (XCKn) must be available on a header pin
#define SPI_UART_ERROR -24

// The blackfin camera needs a receive buffer from the camera, preferably 80 bytes
// If its too small you may get error UART_RECEIVE_OVERFLOW on rebooting the camera as it sends
// version info etc.
#define BLACKFIN_RX_BUFFER -25

// There was a timing issue with the servos - please report to Webbot
#define SERVO_TIMING -26

// A sensor has not been initialised
#define SENSOR_NOT_INITIALISED -28

// The timer does not support PWM
#define TIMER_HAS_NO_PWM -29

// spiBusInit has not been called in appInitHardware
#define SPI_BUS_NOT_INITIALIZED -30

// Duplicate I2C address on the same I2C bus
#define I2C_DUP_ADDR -31

// I2C bus has not been initialised
#define I2C_BUS_NOT_INITIALIZED -32

#endif

