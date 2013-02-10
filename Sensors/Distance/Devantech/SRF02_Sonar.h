/*
 * $Id: SRF02_Sonar.h,v 1.3 2010/09/30 16:51:02 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: SRF02_Sonar.h,v $
 * Revision 1.3  2010/09/30 16:51:02  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 19:00:14  clivewebster
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
 * SRF02_Sonar.h
 *
 *  Created on: 15-May-2010
 *      Author: Clive Webster
 *
 *  Devantech SRF02 Sonar over I2C
 *
 */

#ifndef SRF02_Sonar_H_
#define SRF02_Sonar_H_
#include "../_distance_common.h"
#include "../../../i2cBus.h"

extern DISTANCE_CLASS c_Devantech_SRF02;

typedef struct s_Devantech_SRF02{
	DISTANCE 		distance;
	I2C_DEVICE	  i2cInfo;
}Devantech_SRF02;

// Create the sensor at a given i2c address
#define MAKE_Devantech_SRF02_At(i2cAddr) { \
	MAKE_DISTANCE_SENSOR(c_Devantech_SRF02), \
	MAKE_I2C_DEVICE(i2cAddr) \
	}

// Create the sensor at the default i2c address
#define MAKE_Devantech_SRF02() MAKE_Devantech_SRF02_At(0xE0)

#endif /* SRF02_Sonar */
