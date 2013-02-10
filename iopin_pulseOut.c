/*
 * $Id: iopin_pulseOut.c,v 1.4 2010/07/02 00:11:39 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: iopin_pulseOut.c,v $
 * Revision 1.4  2010/07/02 00:11:39  clivewebster
 * Add activeHigh parameter
 *
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:58:48  clivewebster
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
 * iopin_pulseOut.c
 *
 *  Created on: 22-Sep-2009
 *      Author: Clive Webster
 */
#include "iopin.h"
#include "timer.h"

void pin_pulseOut(const IOPin* pin, TICK_COUNT us, boolean activeHigh){
	pin_make_output(pin,(activeHigh)
			? FALSE : TRUE);				// Make it an output pin with the inactive state
	delay_us(2);							// wait a bit
	pin_toggle(pin);						// Bring pin to active state
	delay_us(us);       					// Wait for duration
	pin_toggle(pin);						// Bring pin to inactive state
}

