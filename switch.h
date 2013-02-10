/*
 * $Id: switch.h,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: switch.h,v $
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/01/24 18:02:22  clivewebster
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
 *
 * switch.h
 *
 *  Created on: 11-Dec-2009
 *      Author: Clive Webster
 *
 *  A simple class to wrap an input pin as a manual switch.
 *
 *  The switch should be connected from the input pin to ground.
 *  We turn on the internal pullup resistor so that the default input value is
 *  high and goes low when pressed. Note that contact bounce may make it alternate
 *  high/low for a short period.
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include "iopin.h"

typedef struct s_switch {
	const IOPin* pin;
} SWITCH;

#define MAKE_SWITCH(pin) {pin}

static __inline__ boolean SWITCH_pressed(SWITCH* sw){
	return pin_is_low(sw->pin);
}

static __inline__ boolean SWITCH_released(SWITCH* sw){
	return pin_is_high(sw->pin);
}

static __inline__ void SWITCH_init(SWITCH *sw){
	pin_make_input(sw->pin,TRUE);
}

#endif /* SWITCH_H_ */
