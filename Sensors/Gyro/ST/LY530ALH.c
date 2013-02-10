
/*
 * $Id: LY530ALH.c,v 1.7 2011/09/08 11:58:54 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: LY530ALH.c,v $
 * Revision 1.7  2011/09/08 11:58:54  clivewebster
 * Changed fractions to 16 bit as now using mV not 10 bit adc value
 *
 * Revision 1.6  2010/07/19 19:55:08  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.5  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.4  2010/06/14 19:08:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 17:27:39  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.2  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.1  2009/10/01 19:01:16  clivewebster
 * Created
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
 * LY530ALH.c
 *
 *  Created on: 01-Oct-2009
 *      Author: Clive Webster
 */

#include "LY530ALH.h"
#include "../../../a2d.h"
#include "../../../fraction.h"

// 0.3 at 16 bit
static const uint16_t PROGMEM ly530alh_slow_frac[]     = {0, 4, 21, 518, 0};
// 1.2 at 16 bit
static const uint16_t PROGMEM ly530alh_fast_frac[]     = {1, 5, 207, 0};


static GYRO_TYPE __read_channel(ADC_CHANNEL pin, boolean slow, uint16_t zero){
	int16_t adc = a2dReadMv(pin);
	adc -= zero;

	GYRO_TYPE rtn;
	if(slow){
		rtn = fraction32(adc, ly530alh_slow_frac);
	}else{
		rtn = fraction32(adc, ly530alh_fast_frac);
	}
	return rtn;
}

static void __ly530alh_init(SENSOR* sensor){
	LY530ALH* device = (LY530ALH*)sensor;
	// The default is that device should be outputing 1.23v ie ADC10 of 251
	device->z_zero = 0;

	// Read 8 times
	for(uint8_t i=0; i<8; i++){
		device->z_zero += a2dReadMv(device->z_pin);
	}

	// Get average
	device->z_zero /= 8;

}

// Read all the values and store into the device
static void __ly530alh_read(SENSOR* sensor){
	LY530ALH* device = (LY530ALH*)sensor;
	device->gyro.z_axis_degrees_per_second = __read_channel(device->z_pin, device->slow, device->z_zero);
}

// 200ms startup time
GYRO_CLASS const c_LY530ALH = MAKE_GYRO_CLASS(&__ly530alh_init,&__ly530alh_read,200,0);



