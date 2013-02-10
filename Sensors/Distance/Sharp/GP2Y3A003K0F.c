
/*
 * $Id: GP2Y3A003K0F.c,v 1.6 2011/08/13 11:34:12 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: GP2Y3A003K0F.c,v $
 * Revision 1.6  2011/08/13 11:34:12  clivewebster
 * Return the minimum distance rather than the average distance.
 *
 * Revision 1.5  2011/07/06 00:43:44  clivewebster
 * Move wide sensors to own file
 *
 * Revision 1.4  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.3  2010/07/01 23:48:51  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.2  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/01/30 21:21:49  clivewebster
 * Added new sensors
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
 * GP2Y3A003K0F.c
 *
 *  Created on: 30-Jan-2010
 *      Author: Clive Webster
 *  A single unit with 5 distance sensors built in. Each sensor led is selected and then
 *  read using the single ADC output
 *
 *  On selecting an LED it takes between 16.5ms and 20.2ms to take the first reading and
 *  a further 5ms for it to appear on the output pin
 *
 *  You need to pull Vin low for at least 5ms between LEDs
 */

#include "GP2wide.h"
// Requires the floating point library
#include <math.h>

#include "../../../timer.h"

// Numer of ms to wait until taking reading (20.2 + 5) = 25.2ms minimum
#define _DELAY_MS_ 26

static void readNext(Sharp_GP2Y3A003K0F* device){
	// Set vin high
//	pin_high(device->vin);

	// Enable the LED
	pin_high(device->led[device->pinNo]);

	// Start measuring time interval from now
	device->distance.sensor.lastRead = clockGetus();
}

// Iinitialse the device
static void __GP2Y3A003K0F_init(SENSOR* sensor){
	Sharp_GP2Y3A003K0F* device = (Sharp_GP2Y3A003K0F*)sensor;
	// Make all leds into output pins and set low
	for(uint8_t i=0; i<5; i++){
		pin_make_output(device->led[i],FALSE);
//		pin_low(device->led[i]);
	}
	// Make vin an output pin and activate
	pin_make_output(device->vin,TRUE);
//	pin_high(device->vin);

	// Start reading the first LED
	device->pinNo = 0;
	readNext(device);
}

// Called when _DELAY_MS has passed
static void __GP2Y3A003K0F_read(SENSOR* sensor){
	Sharp_GP2Y3A003K0F* device = (Sharp_GP2Y3A003K0F*)sensor;

	// Read the current value
//	uint16_t adc = a2dConvert10bit(device->adcPin);
//	DISTANCE_TYPE cm = 16970.0 * pow(adc, -0.9661);

	uint16_t adc = a2dReadMv(device->adcPin);
	DISTANCE_TYPE cm = 78598.80 * pow(adc, -0.9661);


	device->led_cm[device->pinNo] = cm;

	// Turn everything off
	pin_low(device->led[device->pinNo]);
//	pin_low(device->vin);

	// Select the next led
	if(device->pinNo == 4){
		// Reset back to first
		device->pinNo = 0;

		// Store the lowest value
		DISTANCE_TYPE min = device->led_cm[0];
		for(uint8_t i=1; i<5; i++ ){
			if(device->led_cm[i] < min ){
				min = device->led_cm[i];
			}
		}
		device->distance.cm = min;

	}else{
		// Next led
		device->pinNo += 1;
	}

	// Start reading the next pin
	readNext(device);

}

DISTANCE_CLASS const c_Sharp_GP2Y3A003K0F = MAKE_DISTANCE_CLASS( \
		&__GP2Y3A003K0F_init, 	/* init routine */  \
		&__GP2Y3A003K0F_read,	/* read routine */  \
		0,						/* startp delay ms */ \
		_DELAY_MS_);			/* delay between readings */


