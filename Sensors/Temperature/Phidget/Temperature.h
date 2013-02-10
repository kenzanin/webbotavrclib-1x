/*
 * $Id: Temperature.h,v 1.5 2010/06/14 19:10:52 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Temperature.h,v $
 * Revision 1.5  2010/06/14 19:10:52  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:23:22  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:43:05  clivewebster
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
 * Phidget_Temperature.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Phidget temperature sensor.
 *
 *  Returns a value between -40 and +125 degrees celsius
 *
 *  Connect the device to an ADC pin
 *
 *  Phidget_Temperature sensor = MAKE_Phidget_Temperature(J1);
 *  int8_t degrees = sensorRead(sensor);
 */

#ifndef Phidget_Temperature_H_
#define Phidget_Temperature_H_
#include "../_temperature_common.h"
#include "../../../a2d.h"

extern TEMPERATURE_CLASS c_Phidget_Temperature;

typedef struct s_Phidget_Temperature{
	TEMPERATURE 	temperature;
	ADC_CHANNEL 	adcPin;
}Phidget_Temperature;

#define MAKE_Phidget_Temperature(adcPin) {MAKE_TEMPERATURE_SENSOR(c_Phidget_Temperature),adcPin}


#endif /* Phidget_Temperature_H_ */
