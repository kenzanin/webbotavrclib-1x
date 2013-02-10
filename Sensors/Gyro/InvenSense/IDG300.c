/*
 * $Id: IDG300.c,v 1.6 2010/07/19 19:54:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: IDG300.c,v $
 * Revision 1.6  2010/07/19 19:54:25  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.5  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.4  2010/06/14 19:08:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 17:29:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.2  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.1  2009/09/26 18:01:40  clivewebster
 * Initial version
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
#include "IDG300.h"
#include "../../../a2d.h"

static GYRO_TYPE __read_channel(ADC_CHANNEL pin, uint16_t zero){
	int16_t adc = a2dReadMv(pin);
	adc -= zero;

	// Divide by 2mV per degree
	GYRO_TYPE rtn = adc / 2;
	return rtn;
}

static void __idg300_init(SENSOR* sensor){
	IDG300* device = (IDG300*)sensor;
	// The default is that device should be outputing 1.5v ie ADC10 = 307
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
static void __idg300_read(SENSOR* sensor){
	IDG300* device = (IDG300*)sensor;
	device->gyro.x_axis_degrees_per_second = __read_channel(device->x_pin, device->x_zero);
	device->gyro.y_axis_degrees_per_second = __read_channel(device->y_pin, device->y_zero);
}

// 200ms startup time, 20ms between readings
GYRO_CLASS c_IDG300 = MAKE_GYRO_CLASS(&__idg300_init,&__idg300_read,200,20);



