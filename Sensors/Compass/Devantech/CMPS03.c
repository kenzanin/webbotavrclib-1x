/*
 * $Id: CMPS03.c,v 1.4 2010/06/14 18:51:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: CMPS03.c,v $
 * Revision 1.4  2010/06/14 18:51:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/12/11 17:17:55  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:20:31  clivewebster
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
 *
 * CMPS03.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 *
 *
 */
#include "CMPS03.h"
#include "../../../timer.h"


static void __CMPS03_read(SENSOR* sensor){
	TICK_COUNT start;
	TICK_COUNT end;

	CMPS03* compass = (CMPS03*)sensor;

	register const IOPin* pin = compass->pin;

	pin_make_input(pin,FALSE);				// Switch pin to INPUT
	while(pin_is_high(pin));				// Wait for any previous pulse to finish
	while(pin_is_low(pin));					// Wait until the start of the next pulse
	start = clockGetus();					// Get the start time
	while(pin_is_high(pin));				// Loop until the the pin goes low
	end = clockGetus();						// Get the end time

	// Get the duration in uS
	uint16_t duration = end - start;

	// The raw value is 1ms + 100us for each degree
	duration -= 1000;						// remove the 1ms start value

	compass->compass.bearingDegrees = (duration + 50) / 100;// round to the nearest degree
}

COMPASS_CLASS c_CMPS03 = MAKE_COMPASS_CLASS(null,&__CMPS03_read, 0 ,0 );
