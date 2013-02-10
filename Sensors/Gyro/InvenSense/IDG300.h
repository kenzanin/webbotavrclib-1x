/*
 * $Id: IDG300.h,v 1.5 2010/07/15 14:38:19 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: IDG300.h,v $
 * Revision 1.5  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.4  2010/06/14 19:08:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 17:29:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.2  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.1  2009/09/26 18:01:40  clivewebster
 * Initial version
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
 * 2 axis gyro
 * See: http://www.sparkfun.com/datasheets/Components/IDG-300_Datasheet.pdf
 *
 * Requires 2 ADC channels
 *
 *  Created on: 23-Sep-2009
 *      Author: Clive Webster
 *
 *
 *  Specs summary:
 *  3v supply
 *  1.5v output for zero
 *  Power on time: 200ms
 *  If slow:-
 *  Sensitivity: 2mV per degree per second
 *  Full range:  500 degrees per second
 *  Equation for ADC10 reading to result = 2.445 * adc - 750.61
 *
 */

#ifndef IDG300_H_
#define IDG300_H_
#include "../_gyro_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern const GYRO_CLASS c_IDG300;

typedef struct s_IDG300{
	GYRO 		 gyro;
	ADC_CHANNEL  x_pin;
	ADC_CHANNEL  y_pin;
	uint16_t	 x_zero;		// The x adc value at init
	uint16_t	 y_zero;		// The y adc value at init
} IDG300;

// Create the sensor using X, Y pins
#define MAKE_IDG300(pinX, pinY) { \
	MAKE_GYRO_SENSOR(c_IDG300), \
	pinX,\
	pinY,\
	0,0}

#endif /* IDG300_H_ */
