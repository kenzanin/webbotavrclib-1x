/*
 * $Id: PingSonar.c,v 1.7 2010/07/02 00:18:38 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: PingSonar.c,v $
 * Revision 1.7  2010/07/02 00:18:38  clivewebster
 * Add activeHigh parameter for pulseIn and pulseOut
 *
 * Revision 1.6  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/12/11 17:17:55  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.4  2009/11/02 18:32:13  clivewebster
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
 * PingSonar.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 *
 *  Sound travels at 1130ft/sec or 13,560in/sec or 73.7463uS per inch
 *
 *  Since we are detecting the echo then the sound needs to travel out and back so it will take twice
 *  as long ie 147.4926254uS per inch or 58.06796274uS per cm.
 *
 *  We use the clock to measure the pulse length in uS and then divide by 58.06796274 to get cm.
 *  This is the same as multiplying by 0.0172212 which as a fraction is 1/59 + 1/3676
 *
 *
 */
#include "PingSonar.h"
#include "../../../fraction.h"

// 0.0172212
static const uint16_t PROGMEM ping_frac[]     = {0, 59, 3676, 0};


// Read all the values and store into the device
static void __ping_read(SENSOR* sensor){
	TICK_COUNT duration;

	PingSonar* device = (PingSonar*)sensor;

	// 5 us Trigger Pulse
	pin_pulseOut(device->ioPin,5,TRUE);

	// Measure the inbound pulse
	duration = pin_pulseIn(device->ioPin,TRUE);

	device->distance.cm = fraction32(duration, ping_frac);
//	device->distance.cm = duration;

}

// Requires a 200us (so say 1ms) delay between readings
DISTANCE_CLASS const c_PingSonar = MAKE_DISTANCE_CLASS(null,&__ping_read,0,1);

