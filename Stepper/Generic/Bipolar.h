/*
 * $Id: Bipolar.h,v 1.1 2010/08/14 21:03:32 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Bipolar.h,v $
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
 *        File: Bipolar.h
 *  Created on: 12 Aug 2010
 *      Author: Clive Webster
 *
 *  A generic method for bipolar stepper motors driven via half h-bridges like the L293
 *  Requires 4 pins (2 per coil) and an optional Enable pin
 */

#ifndef BIPOLAR_H_
#define BIPOLAR_H_

#include "../_stepper_common.h"
#include "../../iopin.h"

extern STEPPER_MOTOR_IMPL c_bipolar;

// The increment in the phase table
enum {
	HALF_STEP = 1,
	FULL_STEP = 2
};

// A bipolar motor
typedef struct s_bipolar {
	STEPPER_MOTOR 	stepper;		// has all the common stuff
	const IOPin*	coil1A;
	const IOPin*	coil1B;
	const IOPin*	coil2A;
	const IOPin*	coil2B;
	const IOPin*	enable;			// the optional ENABLE pin
	uint8_t			phase;			// The current phase 0...7
	const uint8_t	mode;			// full or half step mode
} BIPOLAR;

#define MAKE_BIPOLAR(inverted,steps, accel, every, coil1A,coil1B, coil2A,coil2B, enable, mode) { \
	MAKE_STEPPER_MOTOR(&c_bipolar,inverted,(mode==FULL_STEP) ? steps : (steps)*2,accel,every), \
	coil1A,coil1B,coil2A,coil2B, enable,0, mode }



#endif /* BIPOLAR_H_ */
