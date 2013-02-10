/*
 * $Id: HMC5883L.h,v 1.1 2011/08/13 11:28:19 clivewebster Exp $
 * Revision History
 * ================
 * $Log: HMC5883L.h,v $
 * Revision 1.1  2011/08/13 11:28:19  clivewebster
 * Added
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: HMC5883L.h
 *  Created on: 8 Aug 2011
 *      Author: Clive Webster
 */

#ifndef HMC5883L_H_
#define HMC5883L_H_


#ifdef __cplusplus
/* ===================== C Code ===============================================*/
extern "C" {
#endif

#include "../_compass_common.h"
#include "../../../i2cBus.h"

extern const COMPASS_CLASS c_HMC5883L;

typedef struct s_HMC5883L{
	COMPASS 	 compass;
	I2C_DEVICE	  i2cInfo;
	int16_t		 rawX,rawY,rawZ;	// The raw x,y,z values
}HMC5883L;

#define MAKE_HMC5883L() { \
	MAKE_COMPASS_SENSOR(c_HMC5883L), \
	MAKE_I2C_DEVICE(0x3c), \
	0,0,0 \
	}

// Set the default refresh rates
extern void HMC5883L_1_5Hz(HMC5883L* compass);
extern void HMC5883L_3Hz(HMC5883L* compass);
extern void HMC5883L_7_5Hz(HMC5883L* compass);
extern void HMC5883L_15Hz(HMC5883L* compass);
extern void HMC5883L_30Hz(HMC5883L* compass);
extern void HMC5883L_75Hz(HMC5883L* compass);




#ifdef __cplusplus
}
/* ==================== C++ Code ==============================================*/
#endif


#endif /* HMC5883L_H_ */
