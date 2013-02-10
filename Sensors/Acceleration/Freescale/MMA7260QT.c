
/*
 * $Id: MMA7260QT.c,v 1.7 2010/07/19 19:52:34 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MMA7260QT.c,v $
 * Revision 1.7  2010/07/19 19:52:34  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.6  2010/07/15 15:11:47  clivewebster
 * Fixed compiler errors
 *
 * Revision 1.5  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.4  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 17:10:16  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.2  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.1  2009/11/01 00:50:25  clivewebster
 * *** empty log message ***
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
 * MMA7260QT.c
 *
 *  Created on: 29-Oct-2009
 *      Author: Clive Webster
 */
#include "MMA7260QT.h"

#include "../../../a2d.h"

/*
#include "../../../fraction.h"
static const uint16_t PROGMEM frac_1_5g[] = {3,  5, 0};		// times 3.5
static const uint16_t PROGMEM frac_2g[]   = {4,  2, 7, 43, 0};	// times 4.6666
static const uint16_t PROGMEM frac_4g[]   = {9,  3, 0};		// times 9.333333
static const uint16_t PROGMEM frac_6g[]   = {14,  0};	    // times 14
*/

static ACCEL_TYPE __read_channel(ADC_CHANNEL pin, uint8_t mode){


	uint16_t adc = a2dReadMv(pin);

	// Zero g is half the supply voltage ie 3v3 / 2 = 1v65 = 1650mV
	int32_t zrel = adc;
	zrel -= 1650;

	// Multiply by 1000 and divide by mV/g
	zrel *= 1000;

	uint16_t div;
	switch(mode){
	case 1:	// 2g
		div = 600;
		break;
	case 2: // 4g
		div = 300;
		break;
	case 3: // 6g
		div = 200;
		break;
	default: // 1.5g
		div = 800;
	}
	zrel /= div;
	ACCEL_TYPE rtn = zrel;

	return rtn;

/*
	const uint16_t * frac;
	switch(mode){
	case 1:
		frac = frac_2g;
		break;
	case 2:
		frac = frac_4g;
		break;
	case 3:
		frac = frac_6g;
		break;
	default:
		frac = frac_1_5g;
	}
	uint16_t adc = a2dConvert10bit(pin) - 511;
	return fraction32(adc,frac);
*/
}

// Read all the values and store into the device
static void read(SENSOR* sensor){
	FS_MMA7260QT* accel = (FS_MMA7260QT*)sensor;
	__accelerometerSetX(&accel->accelerometer,__read_channel(accel->x_pin, accel->mode));
	__accelerometerSetY(&accel->accelerometer,__read_channel(accel->y_pin, accel->mode));
	__accelerometerSetZ(&accel->accelerometer,__read_channel(accel->z_pin, accel->mode));

}

ACCELEROMETER_CLASS const c_FS_MMA7260QT = MAKE_ACCELEROMETER_CLASS(null,&read,2,0);

