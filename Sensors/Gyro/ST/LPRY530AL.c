
/*
 * $Id: LPRY530AL.c,v 1.2 2011/10/04 15:11:29 clivewebster Exp $
 * Revision History
 * ================
 * $Log: LPRY530AL.c,v $
 * Revision 1.2  2011/10/04 15:11:29  clivewebster
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
 *
 * LPRY530AL.c
 *
 *  Created on: 04-Oct-2011
 *      Author: Clive Webster
 */

#include "LPRY530AL.h"
#include "../../../a2d.h"
#include "../../../fraction.h"

// 0.3 at 16 bit
static const uint16_t PROGMEM slow_frac[]     = {0, 4, 21, 518, 0};
// 1.2 at 16 bit
static const uint16_t PROGMEM fast_frac[]     = {1, 5, 207, 0};

#define SAMPLES 1
static GYRO_TYPE __read_channel(ADC_CHANNEL pin, boolean slow, uint16_t zero){
	// Read the voltage im mV
	int16_t adc = 0;

	for(uint8_t s=0; s<SAMPLES;s++){
		adc += a2dReadMv(pin);
	}
	adc /= SAMPLES;


	// Make zero relative
	adc -= zero;

	GYRO_TYPE rtn;
	if(slow){
		// Divide by 3.33mV = multiply by 0.3
		rtn = fraction32(adc, slow_frac);
	}else{
		// Divide by 0.83mV = multiply by 1.2
		rtn = fraction32(adc, fast_frac);
	}
	return rtn;
}

static void init(SENSOR* sensor){
	// The default is that device should be outputing 1.23v ie ADC10 of 251
	LPRY530AL* device = (LPRY530AL*)sensor;
	device->x_zero = 0;
	device->y_zero = 0;
	device->z_zero = 0;

	// Read 8 times
	for(uint8_t i=0; i<8; i++){
		device->x_zero += a2dReadMv(device->x_pin);
		device->y_zero += a2dReadMv(device->y_pin);
		device->z_zero += a2dReadMv(device->z_pin);
	}

	// Get average
	device->x_zero /= 8;
	device->y_zero /= 8;
	device->z_zero /= 8;
}

// Read all the values and store into the device
static void read(SENSOR* sensor){
	LPRY530AL* device = (LPRY530AL*)sensor;
	device->gyro.x_axis_degrees_per_second = __read_channel(device->x_pin, device->slow, device->x_zero);
	device->gyro.y_axis_degrees_per_second = __read_channel(device->y_pin, device->slow, device->y_zero);
	device->gyro.z_axis_degrees_per_second = __read_channel(device->z_pin, device->slow, device->z_zero);
}

// 200ms startup time
GYRO_CLASS PROGMEM c_LPRY530AL = MAKE_GYRO_CLASS(&init,&read,200,0);



