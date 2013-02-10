/*
 * $Id: ADXL335.h,v 1.7 2010/07/15 14:38:19 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ADXL335.h,v $
 * Revision 1.7  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.6  2010/06/14 18:48:08  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/24 17:02:29  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:16:23  clivewebster
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
 * 3 axis +-3g accelerometer from Analog Devices
 * See: http://www.sparkfun.com/datasheets/Components/SMD/adxl335.pdf
 * Supplier: http://www.sparkfun.com/commerce/product_info.php?products_id=9268
 *
 * Requires 3 ADC channels
 *
 *  Created on: 23-Sep-2009
 *      Author: Clive Webster
 *
 *  Assumes that the device uses the default 3v3 supply.
 *
 *  Output is typically 330mV per g at 3v3 supply and zero g is half the supply voltage
 */

#ifndef ADXL335_H_
#define ADXL335_H_
#include "../_acceleration_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern ACCELEROMETER_CLASS c_ADXL335;

typedef struct s_ADXL335{
	ACCELEROMETER accelerometer;
	ADC_CHANNEL x_pin;
	ADC_CHANNEL y_pin;
	ADC_CHANNEL z_pin;
} ADXL335;

// Create the sensor using X, Y and Z pins
#define MAKE_ADXL335(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_ADXL335), \
	pinX,\
	pinY,\
	pinZ }

#endif /* ADXL335_H_ */
