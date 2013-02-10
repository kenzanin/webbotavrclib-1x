/*
 * $Id: ADXL335.c,v 1.9 2010/07/19 19:51:24 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ADXL335.c,v $
 * Revision 1.9  2010/07/19 19:51:24  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.8  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.7  2010/06/14 18:48:08  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/01/24 17:02:29  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.4  2009/11/02 18:16:23  clivewebster
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
 *  Created on: 09-Sep-2009
 *      Author: Clive Webster
 *  Based on original code by Admin at www.societyofrobots.com
 *
 *  Assumes 3v3 supply where 0.33v represents one 'g'
 *
 */
#include "ADXL335.h"
#include "../../../a2d.h"

static ACCEL_TYPE __read_channel(ADC_CHANNEL pin){
	int16_t adc = a2dReadMv(pin);
	// Zero g is half the supply voltage ie 3v3 / 2 = 1v65 = 1650mV
	int32_t zrel = adc - 1650;
	// Multiply by 1000 and divide by 330
	zrel *= 1000;
	zrel /= 330;
	ACCEL_TYPE rtn = zrel;
	return rtn;
}

// Read all the values and store into the device
static void __ad_adxl335_read(SENSOR* sensor){
	ADXL335* accel = (ADXL335*)sensor;

	__accelerometerSetX(&accel->accelerometer,__read_channel(accel->x_pin));
	__accelerometerSetY(&accel->accelerometer,__read_channel(accel->y_pin));
	__accelerometerSetZ(&accel->accelerometer,__read_channel(accel->z_pin));
}

// Datasheet: Turn-on time is dependent on CX, CY, CZ and is approximately 160 × CX or CY or CZ + 1 ms,
// 			   where CX, CY, CZ are in microfarads (uF).
// My board has 0.1uF caps so 160*0.1 + 1 => 17ms
// 20ms between readings since these caps give 50Hz readings ie every 20ms
ACCELEROMETER_CLASS c_ADXL335 = MAKE_ACCELEROMETER_CLASS(null,&__ad_adxl335_read,20,20);



