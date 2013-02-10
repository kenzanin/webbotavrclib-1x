/*
 * $Id: Humidity.h,v 1.5 2010/06/14 19:09:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Humidity.h,v $
 * Revision 1.5  2010/06/14 19:09:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:25:34  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:40:29  clivewebster
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
 * Phidget_Humidity.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Phidget humidity sensor.
 *
 *  Returns a value between 10 and 95 %
 *
 *  Connect the device to an ADC pin
 *
 *  Phidget_Humidity sensor = MAKE_Phidget_Humidity(J1);
 *  int8_t humidity = sensorRead(sensor);
 */

#ifndef Phidget_Humidity_H_
#define Phidget_Humidity_H_
#include "../_humidity_common.h"
#include "../../../a2d.h"

extern HUMIDITY_CLASS c_Phidget_Humidity;

typedef struct S_Phidget_Humidity{
	HUMIDITY 	 humidity;
	ADC_CHANNEL  adcPin;
}Phidget_Humidity;

#define MAKE_Phidget_Humidity(adcPin) {MAKE_HUMIDITY_SENSOR(c_Phidget_Humidity) ,adcPin}

#endif /* Phidget_Humidity_H_ */
