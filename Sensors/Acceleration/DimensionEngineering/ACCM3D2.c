/*
 * $Id: ACCM3D2.c,v 1.9 2010/07/19 19:51:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ACCM3D2.c,v $
 * Revision 1.9  2010/07/19 19:51:51  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.8  2010/07/15 15:11:47  clivewebster
 * Fixed compiler errors
 *
 * Revision 1.7  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.6  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/24 17:05:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:17:20  clivewebster
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
 * DE_ACCM3D2.c
 *
 *  Created on: 09-Sep-2009
 *      Author: Clive Webster
 *  Based on original code by Admin at www.societyofrobots.com
 *
 *  Assumes 3v3 supply where 0.666v represents one 'g' and zero g is 1.65V
 *
 */
#include "ACCM3D2.h"
#include "../../../a2d.h"
//#include "../../../fraction.h"

//static const uint16_t PROGMEM accm3d2_frac[]     = {7,  3,  189, 0};

static ACCEL_TYPE __read_channel(ADC_CHANNEL channel){
	int16_t adc = a2dReadMv(channel);
	// Zero g is half the supply voltage ie 3v3 / 2 = 1v65 = 1650mV
	int32_t zrel = adc - 1650;
	// Multiply by 1000 and divide by 666
	zrel *= 1000;
	zrel /= 666;
	ACCEL_TYPE rtn = zrel;
	return rtn;
}

// Read all the values and store into the device
static void __de_accm3d2_read(SENSOR* sensor){
	DE_ACCM3D2* accel = (DE_ACCM3D2*)sensor;
	__accelerometerSetX(&accel->accelerometer,__read_channel(accel->x_pin));
	__accelerometerSetY(&accel->accelerometer,__read_channel(accel->y_pin));
	__accelerometerSetZ(&accel->accelerometer,__read_channel(accel->z_pin));

}

ACCELEROMETER_CLASS c_DE_ACCM3D2 = MAKE_ACCELEROMETER_CLASS(null,&__de_accm3d2_read,0,0);


