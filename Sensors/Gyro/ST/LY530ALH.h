/*
 * $Id: LY530ALH.h,v 1.6 2010/07/15 14:38:19 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: LY530ALH.h,v $
 * Revision 1.6  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.5  2010/06/14 19:08:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:27:39  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/10/01 19:32:58  clivewebster
 * *** empty log message ***
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
 * LY530ALH.h
 *
 *  Created on: 01-Oct-2009
 *      Author: Clive Webster
 *
 * 1 axis gyro for measuring 'yaw'
 * See: http://www.sparkfun.com/datasheets/Sensors/IMU/LY530ALH.pdf
 *
 * Requires 1 ADC channel
 *
 * Makes a good companion for a LPR530AL which can measure the pitch and roll
 *
 *
 *  Specs summary:
 *  3v supply
 *  1.23v output for zero
 *  Power on time: 200ms - doesn't specify but that is a common setting
 *
 *  If slow:-
 *  Sensitivity: 3.33mV per degree per second
 *  Full range:  300 degrees per second
 *  Equation for ADC10 reading to result = 1.4674 * adc - 369.28
 *
 *  If fast:-
 *  Sensitivity: 0.83V per degree per second
 *  Full range:  1200 degrees per second
 *  Equation for ADC10 reading to result = 5.8997 * adc - 1484.8
 *
 */

#ifndef LY530ALH_H_
#define LY530ALH_H_
#include "../_gyro_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern const GYRO_CLASS c_LY530ALH;

typedef struct s_LY530ALH{
	GYRO 		 gyro;
	ADC_CHANNEL  z_pin;
	boolean		 slow;
	uint16_t	 z_zero;		// The adc value at init
} LY530ALH;

// Create the sensor
#define MAKE_LY530ALH(pinZ,slow) { \
	MAKE_GYRO_SENSOR(c_LY530ALH), \
	pinZ,\
	slow, \
	0}

#endif /* LY530ALH_H_ */
