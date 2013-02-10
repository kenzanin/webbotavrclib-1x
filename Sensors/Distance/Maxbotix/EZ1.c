/*
 * $Id: EZ1.c,v 1.8 2010/07/19 19:53:17 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: EZ1.c,v $
 * Revision 1.8  2010/07/19 19:53:17  clivewebster
 * Cope with -ve  numbers since using a2dReadMv
 *
 * Revision 1.7  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.6  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/24 17:17:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.4  2009/12/11 17:17:57  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:32:13  clivewebster
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
 */
#include "EZ1.h"
#include "../../../a2d.h"
#include "../../../fraction.h"

// Based on readings in Sensor Equation Calculator
// Formula: 1.3166 * adc + 2.4752
// 1.3166
//static const uint16_t PROGMEM ez1_frac[]     = {1,  4, 16, 244, 0};

// 10mV per inch = 3.937mV per cm
// Divide by 3.937 = multiply by 0.254
static const uint16_t PROGMEM ez1_frac[]     = {0,  4, 250, 0};


// Read all the values and store into the device
static void __ez1_read(SENSOR* sensor){
	Maxbotix_EZ1* ez1 = (Maxbotix_EZ1*)sensor;
//	uint16_t adc = a2dConvert10bit(ez1->adcPin);
//	ez1->distance.cm = fraction32(adc, ez1_frac) + 2;

	int16_t adc = a2dReadMv(ez1->adcPin);
	ez1->distance.cm = fraction32(adc, ez1_frac);

}

DISTANCE_CLASS const c_Maxbotix_EZ1 = MAKE_DISTANCE_CLASS(null,&__ez1_read,0,0);
