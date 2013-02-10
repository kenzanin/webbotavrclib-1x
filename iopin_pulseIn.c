/*
 * $Id: iopin_pulseIn.c,v 1.5 2010/07/02 00:11:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: iopin_pulseIn.c,v $
 * Revision 1.5  2010/07/02 00:11:04  clivewebster
 * Add activeHigh parameter
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
 * iopin_pulseIn.c
 *
 *  Created on: 22-Sep-2009
 *      Author: Clive Webster
 */
#include "iopin.h"
#include "timer.h"

TICK_COUNT pin_pulseIn(const IOPin* pin, boolean activeHigh){
	TICK_COUNT start;
	TICK_COUNT end;
	TICK_COUNT duration;

	pin_make_input(pin,FALSE);				// Switch pin to INPUT with no pull up
	while(pin_get(pin)==activeHigh);		// Loop until the the pin goes inactive
	while(pin_get(pin)!=activeHigh);		// Loop until the the pin goes active
	start = clockGetus();					// Get the start time
	while(pin_get(pin)==activeHigh);		// Loop until the the pin goes inactive
	end = clockGetus();						// Get the end time

	duration = end - start;					// Get the duration in uS
	return duration;
}
