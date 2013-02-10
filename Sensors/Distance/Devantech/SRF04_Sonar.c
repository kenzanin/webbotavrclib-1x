/*
 * $Id: SRF04_Sonar.c,v 1.4 2010/07/02 00:18:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: SRF04_Sonar.c,v $
 * Revision 1.4  2010/07/02 00:18:21  clivewebster
 * Add activeHigh parameter for pulseIn and pulseOut
 *
 * Revision 1.3  2010/07/01 23:48:16  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.2  2010/06/14 19:00:14  clivewebster
 * Add copyright license info
 *
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
 * SRF04_Sonar.c
 *
 *  Created on: 16-Nov-2009
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
#include "SRF04_Sonar.h"
#include "../../../fraction.h"

// 0.0172212
static const uint16_t PROGMEM srf04_frac[]     = {0, 59, 3676, 0};


// Read all the values and store into the device
static void __srf04_read(SENSOR* sensor){
	TICK_COUNT duration;

	Devantech_SRF04* device = (Devantech_SRF04*)sensor;

	// initialise the pins
	pin_make_output(device->out,FALSE);		// Set low
	pin_make_input(device->in,FALSE);

	// 10us high trigger pulse
	pin_pulseOut(device->out,10,TRUE);

	// Measure the inbound pulse
	duration = pin_pulseIn(device->in,TRUE);

	device->distance.cm = fraction32(duration, srf04_frac);
}

// Requires a 50ms delay between readings
DISTANCE_CLASS c_Devantech_SRF04 = MAKE_DISTANCE_CLASS(null,&__srf04_read,0,50);

