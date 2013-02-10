/*
 * $Id: A4983.c,v 1.2 2010/08/11 17:20:15 clivewebster Exp $
 * Revision History
 * ================
 * $Log: A4983.c,v $
 * Revision 1.2  2010/08/11 17:20:15  clivewebster
 * Version 1.24
 *
 * Revision 1.1  2010/08/10 22:51:58  clivewebster
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
 *        File: A4983.c
 *  Created on: 4 Aug 2010
 *      Author: Clive Webster
 */
#include "A4983.h"

static void setSpeed(STEPPER_MOTOR* stepper, DRIVE_SPEED speed){
	POLOLU_A4983* motor = (POLOLU_A4983*)stepper;

	// set the direction pin
	pin_set(motor->direction, (speed<0) ? FALSE : TRUE);
}

static void setConnected(STEPPER_MOTOR* stepper, boolean connected){
	POLOLU_A4983* motor = (POLOLU_A4983*)stepper;

	// Set enable pin high to disconnect
	pin_set(motor->enable, (connected) ? FALSE : TRUE );

}

static void step(STEPPER_MOTOR* stepper){
	POLOLU_A4983* motor = (POLOLU_A4983*)stepper;

	// Send a 1uS pulse to the step pin
	pin_high(motor->pulse);
	pin_low(motor->pulse);
}

static void	init(STEPPER_MOTOR* stepper){
	POLOLU_A4983* motor = (POLOLU_A4983*)stepper;

	// Make pins into outputs with initial values
	pin_make_output(motor->direction, FALSE);// clockwise
	pin_make_output(motor->enable, TRUE);	// disconnected
	pin_make_output(motor->pulse, FALSE);

	pin_make_output(motor->reset, FALSE);	// start reset the device
	pin_make_output(motor->reset, TRUE);	// end reset the device - will now respond to pulses

}

STEPPER_MOTOR_IMPL c_a4983 = MAKE_STEPPER_MOTOR_IMPL(&init, &setSpeed,&setConnected,&step);


