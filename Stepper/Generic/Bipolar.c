/*
 * $Id: Bipolar.c,v 1.1 2010/08/14 21:03:32 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Bipolar.c,v $
 * Revision 1.1  2010/08/14 21:03:32  clivewebster
 * Added
 *
 * ================
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
 *        File: Bipolar.c
 *  Created on: 12 Aug 2010
 *      Author: Clive Webster
 */

#include "Bipolar.h"

static uint8_t PROGMEM phases[] = {
		0b11000001,	// Coil 1A
		0b00011100, // Coil 1B
		0b00000111, // Coil 1C
		0b01110000  // Coil 1D
};

static void setConnected(STEPPER_MOTOR* stepper, boolean connected){
	BIPOLAR* motor = (BIPOLAR*)stepper;

	// Set enable pin low to disconnect
	pin_set(motor->enable, connected);

}

static void step(STEPPER_MOTOR* stepper){
	BIPOLAR* motor = (BIPOLAR*)stepper;

	// Move to the next/previous phase
	if(motor->stepper.actualSpeed < 0){
		motor->phase = (motor->phase - motor->mode) & 7;
	}else{
		motor->phase = (motor->phase + motor->mode) & 7;
	}
	uint8_t mask = 1 << motor->phase;

	// Find next state of coils
	boolean coil1A = (mask & pgm_read_byte(&phases[0])) ? TRUE : FALSE;
	boolean coil1B = (mask & pgm_read_byte(&phases[1])) ? TRUE : FALSE;
	boolean coil2A = (mask & pgm_read_byte(&phases[2])) ? TRUE : FALSE;
	boolean coil2B = (mask & pgm_read_byte(&phases[3])) ? TRUE : FALSE;

	pin_set(motor->coil1A, coil1A);
	pin_set(motor->coil1B, coil1B);
	pin_set(motor->coil2A, coil2A);
	pin_set(motor->coil2B, coil2B);

}

static void	init(STEPPER_MOTOR* stepper){
	BIPOLAR* motor = (BIPOLAR*)stepper;

	// Make pins into outputs with initial values
	pin_make_output(motor->enable, FALSE);	// disconnected

	// Turn off all coils
	pin_make_output(motor->coil1A, FALSE);
	pin_make_output(motor->coil1B, FALSE);
	pin_make_output(motor->coil2A, FALSE);
	pin_make_output(motor->coil2B, FALSE);
	step(stepper);						// Set to first valid state

}

STEPPER_MOTOR_IMPL c_bipolar = MAKE_STEPPER_MOTOR_IMPL(&init,null,&setConnected,&step);


