/*
 * $Id: A4983.h,v 1.2 2010/08/11 17:20:15 clivewebster Exp $
 * Revision History
 * ================
 * $Log: A4983.h,v $
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
 *        File: A4983.h
 *  Created on: 4 Aug 2010
 *      Author: Clive Webster
 *
 *  Add support for a Pololu A4983 stepper motor controller
 */

#ifndef A4983_H_
#define A4983_H_
#include "../_stepper_common.h"
#include "../../timer.h"
#include "../../iopin.h"

extern const STEPPER_MOTOR_IMPL c_a4983;

// A board controls an individual motor
typedef struct s_a4983_board {
	STEPPER_MOTOR 	stepper;		// has all the common stuff
	const IOPin*	direction;		// the pin used to control the direction
	const IOPin*	pulse;			// the pin used to pulse
	const IOPin*	reset;			// the optional RESET pin
	const IOPin*	enable;			// the optional ENABLE pin
} POLOLU_A4983;

#define MAKE_POLOLU_A4983(inverted,steps, accel, every, direction,pulse,reset,enable) { \
	MAKE_STEPPER_MOTOR(&c_a4983,inverted,steps,accel,every), \
	direction, pulse, reset, enable }


#endif /* A4983_H_ */
