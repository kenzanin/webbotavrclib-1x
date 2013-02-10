/*
 * $Id: CMPS09.h,v 1.3 2010/09/30 16:49:35 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: CMPS09.h,v $
 * Revision 1.3  2010/09/30 16:49:35  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 18:51:59  clivewebster
 * Add copyright license info
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
 * CMPS09.h
 *
 *  Created on: 19-May-2010
 *      Author: Clive Webster
 *
 *  Dont forget to calibrate the compass.
 *
 */

#ifndef CMPS09_H_
#define CMPS09_H_

#include "../_compass_common.h"
#include "../../../i2cBus.h"

/*
 *  Access the compass via the I2C bus.
 */
extern COMPASS_CLASS c_CMPS09_i2c;

typedef struct s_CMPS09_i2c{
	COMPASS 	 compass;
	I2C_DEVICE	  i2cInfo;
}CMPS09_I2C;

#define MAKE_CMPS09_I2C_At(i2cAddr) { \
	MAKE_COMPASS_SENSOR(c_CMPS09_i2c), \
	MAKE_I2C_DEVICE(i2cAddr) \
	}

// Use the default I2C address
#define MAKE_CMPS09_I2C() MAKE_CMPS09_I2C_At(0xC0)


#endif /* CMPS09 */
