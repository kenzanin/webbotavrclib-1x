/*
 * $Id: 30V.h,v 1.5 2010/06/14 19:11:42 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: 30V.h,v $
 * Revision 1.5  2010/06/14 19:11:42  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:24:38  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:57  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:44:48  clivewebster
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
 * Phidget_30V.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Phidget voltage sensor.
 *
 *  Returns a voltage between -30 and +30
 *
 *  Connect the device to an ADC pin
 *
 *  Phidget_30V sensor = MAKE_Phidget_30V(J1);
 *  int8_t volts = sensorRead(sensor);
 */

#ifndef Phidget_30V_H_
#define Phidget_30V_H_
#include "../_voltage_common.h"
#include "../../../a2d.h"

extern VOLTAGE_CLASS c_Phidget_30V;

typedef struct s_Phidget_30V{
	VOLTAGE voltage;
	ADC_CHANNEL adcPin;
}Phidget_30V;

#define MAKE_Phidget_30V(adcPin) {MAKE_VOLTAGE_SENSOR(c_Phidget_30V),adcPin}

#endif /* Phidget_30V_H_ */
