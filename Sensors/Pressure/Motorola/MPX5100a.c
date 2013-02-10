/*
 * $Id: MPX5100a.c,v 1.9 2011/08/13 11:29:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MPX5100a.c,v $
 * Revision 1.9  2011/08/13 11:29:21  clivewebster
 * Return values in Pa rather than kPa
 *
 * Revision 1.8  2010/07/19 19:55:53  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.7  2010/07/15 15:11:46  clivewebster
 * Fixed compiler errors
 *
 * Revision 1.6  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.5  2010/06/14 19:09:50  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:22:24  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:41:41  clivewebster
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
 *  Created on: 24-Sep-2009
 *      Author: Clive Webster
 *
 */
#include "MPX5100a.h"
#include "../../../a2d.h"


static PRESSURE_TYPE adc2pa(uint16_t adc){
	// 45mV per kpA
	PRESSURE_TYPE val = adc;
	val *= 1000;		// Convert to Pa
	val /= 45;
	return val;
}

#define SAMPLES 8
static void __mpx5100a_init(SENSOR* sensor){
	MPX5100A* device = (MPX5100A*)sensor;

	// Get the initial value
	uint16_t total=0;
	for(int i=0; i<SAMPLES;i++){
		total += a2dReadMv(device->adcPin);
	}
	uint16_t start = (total + (SAMPLES >> 1)) / SAMPLES;

	device->pressure.paStart = adc2pa(start);
}

// Read the value and store into the device
static void __mpx5100a_read(SENSOR* sensor){
	MPX5100A* device = (MPX5100A*)sensor;
	// read the sensor
	uint16_t adc = a2dReadMv(device->adcPin);

	device->pressure.pa = adc2pa(adc);
}

// 20ms startup time
PRESSURE_CLASS const c_MPX5100A = MAKE_PRESSURE_CLASS(&__mpx5100a_init,&__mpx5100a_read,20,0);



