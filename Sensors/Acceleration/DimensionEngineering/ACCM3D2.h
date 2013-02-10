/*
 * $Id: ACCM3D2.h,v 1.6 2010/07/15 14:38:19 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ACCM3D2.h,v $
 * Revision 1.6  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.5  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:05:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:17:20  clivewebster
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
 * 3 axis accelerometer from Dimension Engineering
 * See: http://www.dimensionengineering.com/DE-ACCM3D2.htm
 *
 * Requires 3 ADC channels
 *
 *  Created on: 09-Sep-2009
 *      Author: Clive Webster
 *
 *  Based on original code by Admin at www.societyofrobots.com
 *
 *  Assumes that the device uses the default 3v3 supply
 *
 *  An output of 1.65v means nothing is happening
 *  The voltage changes by 0.666V per 1000mG
 *  Equation for ADC10 reading to result = 7.3387 * adc - 2492.5
 *
 *  7.3387 as a fraction is 7/1, 1/3, 1/189
 *
 */

#ifndef DE_ACCM3D2_H_
#define DE_ACCM3D2_H_
#include "../_acceleration_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern ACCELEROMETER_CLASS c_DE_ACCM3D2;

typedef struct s_DE_ACCM3D2{
	ACCELEROMETER accelerometer;
	ADC_CHANNEL x_pin;
	ADC_CHANNEL y_pin;
	ADC_CHANNEL z_pin;
}DE_ACCM3D2;

// Create the sensor using X, Y and Z pins
#define MAKE_DE_ACCM3D2(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_DE_ACCM3D2), \
	pinX,\
	pinY,\
	pinZ }

#endif /* DE_ACCM3D2_H_ */
