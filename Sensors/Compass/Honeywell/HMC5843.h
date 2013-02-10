/*
 * $Id: HMC5843.h,v 1.3 2010/09/30 16:50:17 clivewebster Exp $
 * Revision History
 * ================
 * $Log: HMC5843.h,v $
 * Revision 1.3  2010/09/30 16:50:17  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/07/02 00:23:40  clivewebster
 * Return raw magetometer values and also calculate the roll and pitch angles
 *
 * Revision 1.1  2010/06/20 19:31:07  clivewebster
 * Added in Version 1.21
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
 *        File: HMC5843.h
 *  Created on: 20 Jun 2010
 *      Author: Clive Webster
 *
 *  3 axis magnetometer, i2c address = 0x3c
 *  2.5v - 3.3v supply, current 1mA
 *  Time between readings: 100ms (10Hz)
 *  Datasheet: http://www.sparkfun.com/datasheets/Sensors/Magneto/HMC5843.pdf
 */

#ifndef HMC5843_H_
#define HMC5843_H_

#include "../_compass_common.h"
#include "../../../i2cBus.h"

extern const COMPASS_CLASS c_HMC5843;

typedef struct s_HMC5843{
	COMPASS 	 compass;
	I2C_DEVICE	  i2cInfo;
	int16_t		 rawX,rawY,rawZ;	// The raw x,y,z values
}HMC5843;

#define MAKE_HMC5843() { \
	MAKE_COMPASS_SENSOR(c_HMC5843), \
	MAKE_I2C_DEVICE(0x3c), \
	0,0,0 \
	}

// Set the default refresh rates
extern void HMC5843_1Hz(HMC5843* compass);
extern void HMC5843_2Hz(HMC5843* compass);
extern void HMC5843_5Hz(HMC5843* compass);
extern void HMC5843_10Hz(HMC5843* compass);
extern void HMC5843_20Hz(HMC5843* compass);
extern void HMC5843_50Hz(HMC5843* compass);


#endif /* HMC5843_H_ */
