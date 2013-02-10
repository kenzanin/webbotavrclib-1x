/*
 * $Id: LPRY530AL.h,v 1.2 2011/10/04 15:11:29 clivewebster Exp $
 * Revision History
 * ================
 * $Log: LPRY530AL.h,v $
 * Revision 1.2  2011/10/04 15:11:29  clivewebster
 * Added
 *
 * ================
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
 * LPRY530AL.h
 *
 *  Created on: 4-Oct-2011
 *      Author: Clive Webster
 *
 * Combines the LPR530AL and LY530AL into a common device
 */

#ifndef LPRY530AL_H_
#define LPRY530AL_H_

#include "../_gyro_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern const GYRO_CLASS c_LPRY530AL;

typedef struct s_LPRY530AL{
	GYRO 		 gyro;
	ADC_CHANNEL  x_pin;
	ADC_CHANNEL  y_pin;
	ADC_CHANNEL  z_pin;
	boolean		slow;
	uint16_t	x_zero;		// The adc value at init
	uint16_t	y_zero;
	uint16_t	z_zero;
} LPRY530AL;

// Create the sensor
#define MAKE_LPRY530AL(pinX,pinY,pinZ,slow) { \
	MAKE_GYRO_SENSOR(c_LPRY530AL), \
	pinX, pinY, pinZ,\
	slow, \
	0,0,0}




#endif /* LPRY530AL_H_ */
