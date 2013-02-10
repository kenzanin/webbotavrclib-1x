/*
 * $Id: L297.c,v 1.2 2010/08/11 17:20:06 clivewebster Exp $
 * Revision History
 * ================
 * $Log: L297.c,v $
 * Revision 1.2  2010/08/11 17:20:06  clivewebster
 * Version 1.24
 *
 * Revision 1.1  2010/08/10 22:51:57  clivewebster
 * Created
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
 *        File: L297.c
 *  Created on: 4 Aug 2010
 *      Author: Clive Webster
 */
#include "L297.h"

static void setSpeed(STEPPER_MOTOR* stepper, DRIVE_SPEED speed){
	L297* motor = (L297*)stepper;

	// set the direction pin
	pin_set(motor->direction, (speed<0) ? FALSE : TRUE);
}

static void setConnected(STEPPER_MOTOR* stepper, boolean connected){
	L297* motor = (L297*)stepper;

	// Set enable pin low to disconnect
	pin_set(motor->enable, connected);

}

static void step(STEPPER_MOTOR* stepper){
	L297* motor = (L297*)stepper;

	// Send a 1uS pulse to the step pin
	pin_low(motor->pulse);
	pin_high(motor->pulse);
}

static void	init(STEPPER_MOTOR* stepper){
	L297* motor = (L297*)stepper;

	// Make pins into outputs with initial values
	pin_make_output(motor->direction, FALSE);// clockwise
	pin_make_output(motor->enable, FALSE);	// disconnected
	pin_make_output(motor->pulse, TRUE);

	pin_make_output(motor->reset, FALSE);	// start reset the device
	pin_make_output(motor->reset, TRUE);	// end reset the device - will now respond to pulses

}

STEPPER_MOTOR_IMPL const c_l297 = MAKE_STEPPER_MOTOR_IMPL(&init, &setSpeed,&setConnected,&step);

