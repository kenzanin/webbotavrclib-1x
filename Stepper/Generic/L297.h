/*
 * $Id: L297.h,v 1.2 2010/08/11 17:20:06 clivewebster Exp $
 * Revision History
 * ================
 * $Log: L297.h,v $
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
 *        File: L297.h
 *  Created on: 4 Aug 2010
 *      Author: Clive Webster
 *
 *  Add support for a generic L297 stepper motor controller
 */

#ifndef L297_H_
#define L297_H_
#include "../_stepper_common.h"
#include "../../timer.h"
#include "../../iopin.h"

extern STEPPER_MOTOR_IMPL c_l297;

// An L297 controls an individual motor
typedef struct s_l297 {
	STEPPER_MOTOR 	stepper;		// has all the common stuff
	const IOPin*	direction;		// the pin used to control the direction
	const IOPin*	pulse;			// the pin used to pulse
	const IOPin*	reset;			// the optional RESET pin
	const IOPin*	enable;			// the optional ENABLE pin
} L297;

#define MAKE_L297(inverted,steps, accel, every, direction,pulse,reset,enable) { \
	MAKE_STEPPER_MOTOR(&c_l297,inverted,steps,accel,every), \
	direction, pulse, reset, enable }


#endif /* L297_H_ */
