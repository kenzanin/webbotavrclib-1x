/*
 * $Id: AC50A.h,v 1.5 2010/06/14 18:57:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: AC50A.h,v $
 * Revision 1.5  2010/06/14 18:57:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:13:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:23:50  clivewebster
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
 *
 * Phidget_AC50A.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Phidget current sensor.
 *
 *  Returns a value between 0 and +50 amps
 *
 *  Connect the device to an ADC pin
 *
 *  Phidget_AC50A sensor = MAKE_Phidget_AC50A(J1);
 *  uint8_t amps = sensorRead(sensor);
 */

#ifndef Phidget_AC50A_H_
#define Phidget_AC50A_H_
#include "../_current_common.h"
#include "../../../a2d.h"

extern const CURRENT_CLASS c_Phidget_AC50A;

typedef struct s_Phidget_AC50A{
	CURRENT current;
	ADC_CHANNEL pin;
}Phidget_AC50A;

#define MAKE_Phidget_AC50A(pin) {MAKE_CURRENT_SENSOR(c_Phidget_AC50A),pin}

#endif /* Phidget_AC50A_H_ */
