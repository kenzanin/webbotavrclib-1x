/*
 * $Id: CMPS03.h,v 1.6 2010/09/30 16:49:35 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: CMPS03.h,v $
 * Revision 1.6  2010/09/30 16:49:35  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.5  2010/06/14 18:51:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/05/19 01:59:04  clivewebster
 * Add I2C interface to CMPS03 in Version 1.19
 *
 * Revision 1.3  2009/12/11 17:17:55  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:20:31  clivewebster
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
 * CMPS03.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Dont forget to calibrate the compass. See http://www.robot-electronics.co.uk/htm/cmps3tech.htm
 *
 */

#ifndef CMPS03_H_
#define CMPS03_H_

#include "../_compass_common.h"
#include "../../../i2cBus.h"

/*
 *  Requires one I/O pin that is connected to the +5v regulated supply.
 *  On the Axon this means you must use one of the ADC pins - using other I/O pins
 *  will fry the device.
 *
 *  This library measures the pulse length from Pin4 (PWM) of the device.
 *
 *  If your application has various other interrupts happening then they will effect the
 *  accuracy of the measurement.
 *
 */

extern const COMPASS_CLASS c_CMPS03;

typedef struct s_CMPS03{
	COMPASS 	 compass;
	const IOPin* pin;
}CMPS03;

#define MAKE_CMPS03(pin) { \
	MAKE_COMPASS_SENSOR(c_CMPS03), \
	pin}

/*
 *  Access the compass via the I2C bus.
 */
extern const COMPASS_CLASS c_CMPS03_i2c;

typedef struct s_CMPS03_i2c{
	COMPASS 	 compass;
	I2C_DEVICE	  i2cInfo;
}CMPS03_I2C;

#define MAKE_CMPS03_I2C_At(i2cAddr) { \
	MAKE_COMPASS_SENSOR(c_CMPS03_i2c), \
	MAKE_I2C_DEVICE(i2cAddr) \
	}

// Use the default I2C address
#define MAKE_CMPS03_I2C() MAKE_CMPS03_I2C_At(0xC0)


#endif /* CMPS03 */
