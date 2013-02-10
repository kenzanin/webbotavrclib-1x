/*
 * $Id: iopin.h,v 1.5 2010/07/02 00:12:41 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: iopin.h,v $
 * Revision 1.5  2010/07/02 00:12:41  clivewebster
 * pin_make_output now specifies the initial output value and added the activeHigh parameter to pin_pulseIn and pin_pulseOut
 *
 * Revision 1.4  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 18:58:48  clivewebster
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
#ifndef _IOPIN_H_
#define _IOPIN_H_

#include <avr/pgmspace.h>
#include <stddef.h>
#include "errors.h"
#include "libdefs.h"

/******************************************************

	Perform I/O

*******************************************************/



/******************************************************

	Set a port pin to high

******************************************************/
void pin_high(const IOPin* io);

/******************************************************

	Set a port pin to low

******************************************************/
void pin_low(const IOPin* io);

/******************************************************

	Read the value of an I/O pin and
	return TRUE if it is high or FALSE if low

******************************************************/
boolean pin_is_high(const IOPin* io);

#define pin_get(io) pin_is_high(io)

/******************************************************

	Read the value of an I/O pin and
	return TRUE if it is low or FALSE if high,
	eg
	if(pin_is_low(B2)){
	   .... PortB pin 2 is low ....
	}

******************************************************/
#define pin_is_low(io)  !pin_is_high(io)


/******************************************************

	Set an output pin high or low
	eg
	pin_set(B2, TRUE); // To set PortB pin 2 high

******************************************************/
void pin_set(const IOPin* io, boolean val);



/******************************************************

	Toggle an output pin ie if low then make high,
	if high then make low
	eg
	pin_toggle(B2); // Toggle PortB pin 2

******************************************************/
void pin_toggle(const IOPin* io);


/******************************************************

	Set the pin as an input, specifying if you need
	a pullup resistor (TRUE/FALSE)

	ie pin_make_input(B2, TRUE); // to  make B2 an input with pullup
******************************************************/
void pin_make_input(const IOPin* io, boolean pullup);

/******************************************************

	Set the pin as an output pin

	ie pin_make_output(B2, TRUE); // to  make B2 an output high
******************************************************/
void pin_make_output(const IOPin* io, boolean val);



/******************************************************

	Is the pin an output pin
	return TRUE if it is high or FALSE if low


******************************************************/
boolean pin_is_output(const IOPin* io);


/******************************************************

	Read the value of an I/O pin and
	return TRUE if it is low or FALSE if high,
	eg
	if(pin_is_low(B2)){
	   .... PortB pin 2 is low ....
	}

******************************************************/
#define pin_is_input(io)  !pin_is_output(io)

/******************************************************

	Output a pulse of the given duration.
	io: The pin
	us: The duration of the pulse
	activeHigh: TRUE if the pulse is low-high-low or FALSE for high-low-high

	This will set the pin as an output and it will
	remain as an output on return

******************************************************/
void pin_pulseOut(const IOPin* io, TICK_COUNT us, boolean activeHigh);

/******************************************************

	Measure the length of an input pulse
	io: The pin
	return: The pulse duration in us

	This will set the pin as an input and it will
	remain as an input on return

******************************************************/
TICK_COUNT pin_pulseIn(const IOPin* io, boolean activeHigh);

#endif
