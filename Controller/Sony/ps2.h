/*
 * $Id: ps2.h,v 1.3 2010/06/14 18:43:18 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ps2.h,v $
 * Revision 1.3  2010/06/14 18:43:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/06/13 15:09:55  clivewebster
 * Add rumble, button pressures and additional button test routines.
 *
 * Revision 1.1  2010/04/12 23:18:37  clivewebster
 * *** empty log message ***
 *
 * ===========
 * 
 *
 * Copyright (C) 2010 Clive Webster (Webbot)
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
 * ps2.h
 *
 *  Created on: 10 Apr 2010
 *      Author: Clive Webster
 *
 *   Sony PS2 controller support
 */

#ifndef PS2_H_
#define PS2_H_

#include "../../_spi_common.h"

// Define constants for buttons. These can be ORed together
#define	PS2_BTN_SELECT 		0x0001u
#define	PS2_BTN_L3     		0x0002u
#define	PS2_BTN_R3     		0x0004u
#define	PS2_BTN_START  		0x0008u
#define	PS2_DPAD_UP    		0x0010u
#define	PS2_DPAD_RIGHT 		0x0020u
#define	PS2_DPAD_DOWN  		0x0040u
#define	PS2_DPAD_LEFT  		0x0080u
#define	PS2_BTN_L2     		0x0100u
#define	PS2_BTN_R2     		0x0200u
#define	PS2_BTN_L1     		0x0400u
#define	PS2_BTN_R1     		0x0800u
#define	PS2_BTN_TRIANGLE	0x1000u
#define	PS2_BTN_CIRCLE  	0x2000u
#define	PS2_BTN_X     		0x4000u
#define	PS2_BTN_SQUARE  	0x8000u

// Define the analogue joystick numbers
typedef enum {
	// analogue joysticks
	PS2_STICK_RIGHT_X=2,
	PS2_STICK_RIGHT_Y,
	PS2_STICK_LEFT_X,
	PS2_STICK_LEFT_Y,
	// virtual joysticks
	PS2_STICK_DPAD_X,
	PS2_STICK_DPAD_Y,
	PS2_STICK_SHAPE_X,
	PS2_STICK_SHAPE_Y
} PS2_STICK;


// Create the datatype for the controller and define a constructor
typedef struct s_sony_ps2{
	SPI_ABSTRACT_DEVICE	_device_;		// Its an SPI device
	uint8_t mode;
	uint8_t reply;
	uint8_t	buffer[18];					// 2 for buttons, 4 for joysticks, 12 for pressures
	uint8_t calibration[4];				// The joystick values that represent 'center'
	uint8_t	deadzone;					// The size of the deadzone around the mid point
	boolean rumble;						// Has rumble been turned on
	uint8_t rumbleLeft;					// Rumble on the left 0...0xff
	boolean	rumbleRight;				// Rumble on the right true/false
	uint16_t previous;					// Previous button state
} SONY_PS2;

#define MAKE_SONY_PS2(select) { \
	MAKE_SPI_DEVICE(SPI_MODE_3,SPI_DATA_ORDER_LSB,select,0x00), \
	255,255, \
	{255,255,128,128,128,128}, \
	{128,128,128,128}, \
	27,	\
	FALSE, 0, FALSE,\
	0}

// Calibrate the joysticks so that current positions = center
boolean sonyPS2_calibrate(SONY_PS2* controller, uint8_t deadzone);


// Is the controller in analog mode
boolean sonyPS2_isAnalogMode(const SONY_PS2* controller);

// Set the controller to analogue mode - return TRUE if success
boolean sonyPS2_setAnalogMode(SONY_PS2* controller);

// Read the controller and return TRUE if it was read successfully
boolean sonyPS2_read(SONY_PS2* controller);

// Returns a bitwise representation of the pressed buttons
// A bit value of 1 indicates the button is pressed
uint16_t sonyPS2_buttonsRaw(const SONY_PS2* controller);

// return TRUE if the specified PS2_BTN_??? button is pressed
boolean sonyPS2_buttonPressed(const SONY_PS2* controller, uint16_t button);

// Get the pressure for a button 0=not pressed, up to 255=pressed hard)
uint8_t sonyPS2_buttonPressure(const SONY_PS2* controller, uint16_t button);

// Return the value for a given joystick axis without any calibration or deadzone
// Left/Up = 0, Right/Down=255
uint8_t sonyPS2_joystickRaw(const SONY_PS2* controller, PS2_STICK stick);

// Read the joystick and return a +/- value around the midpoint
int8_t sonyPS2_joystick(const SONY_PS2* controller, PS2_STICK stick);

// Set the rumble motors. One is just on or off, and the other is 0x00....0xff
void sonyPS_setRumble(SONY_PS2* controller, uint8_t left, boolean right);

// Return OR bitmask of the buttons that have changed since the previous call, or 0
// if no buttons have changed
uint16_t sonyPS2_buttonsChanged(const SONY_PS2* controller);

// Has button just been pressed?
boolean sonyPS2_buttonDown(const SONY_PS2* controller, uint16_t button);
// Has button just been released?
boolean sonyPS2_buttonUp(const SONY_PS2* controller, uint16_t button);
// Has button been held down?
boolean sonyPS2_buttonHeld(const SONY_PS2* controller, uint16_t button);

#endif /* PS2_H_ */
