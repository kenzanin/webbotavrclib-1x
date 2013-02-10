/*
 * $Id: IDG500.c,v 1.9 2010/07/19 19:54:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: IDG500.c,v $
 * Revision 1.9  2010/07/19 19:54:25  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.8  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.7  2010/06/14 19:08:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/01/24 17:29:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.5  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.4  2009/11/02 18:38:42  clivewebster
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
 *  Created on: 23-Sep-2009
 *      Author: Clive Webster
 *
 */
#include "IDG500.h"
#include "../../../a2d.h"
#include "../../../fraction.h"

// 0.10989 at 10 bit
static const uint16_t PROGMEM idg500_slow_frac[]     = {0, 10, 101, 0};

static GYRO_TYPE __read_channel(ADC_CHANNEL pin, boolean slow, int16_t zero){
	int16_t adc = a2dReadMv(pin);
	adc -= zero;

	GYRO_TYPE rtn;
	if(slow){
		// Divide by 9.1mV = multiply by 0.10989
		rtn = fraction32(adc, idg500_slow_frac);
		rtn -= 148;
	}else{
		// Divide by 2mV
		rtn = adc / 2;
	}
	return rtn;
}

static void __idg500_init(SENSOR* sensor){
	IDG500* device = (IDG500*)sensor;
	// The default is that device should be outputing 1.35v ie ADC10 of 276
	device->x_zero = 0;
	device->y_zero = 0;

	// Read 8 times
	for(uint8_t i=0; i<8; i++){
		device->x_zero += a2dReadMv(device->x_pin);
		device->y_zero += a2dReadMv(device->y_pin);
	}

	// Get average
	device->x_zero /= 8;
	device->y_zero /= 8;
}

// Read all the values and store into the device
static void __idg500_read(SENSOR* sensor){
	IDG500* device = (IDG500*)sensor;
	device->gyro.x_axis_degrees_per_second = __read_channel(device->x_pin, device->slow, device->x_zero);
	device->gyro.y_axis_degrees_per_second = __read_channel(device->y_pin, device->slow, device->y_zero);
}

// 200ms startup time, 20ms between readings
GYRO_CLASS c_IDG500 = MAKE_GYRO_CLASS(&__idg500_init,&__idg500_read,200,20);



