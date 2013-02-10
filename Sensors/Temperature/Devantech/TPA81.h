/*
 * $Id: TPA81.h,v 1.4 2010/09/30 16:51:44 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: TPA81.h,v $
 * Revision 1.4  2010/09/30 16:51:44  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.3  2010/06/14 19:10:52  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/06/03 18:43:05  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/20 00:46:48  clivewebster
 * *** empty log message ***
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
 * TPA81.h
 *
 *  Created on: 19 Mar 2010
 *      Author: Clive Webster
 *
 *  Measures ambient temperature and multiple direction readings
 *
 */

#ifndef TPA81_H_
#define TPA81_H_

#include "../_temperature_common.h"
#include "../../../i2cBus.h"

extern const TEMPERATURE_CLASS c_Devantech_TPA81;

typedef struct s_Devantech_TPA81{
	TEMPERATURE 	temperature;
	I2C_DEVICE	  i2cInfo;
	TEMPERATURE_TYPE sensor[8];			// The degrees celsius for each sensor
	uint8_t			version;			// The firmware version on the device
}Devantech_TPA81;

#define MAKE_Devantech_TPA81_At(i2cAddr) {MAKE_TEMPERATURE_SENSOR(c_Devantech_TPA81), \
	MAKE_I2C_DEVICE(i2cAddr), \
	{0,0,0,0,0,0,0,0},\
	0}
#define MAKE_Devantech_TPA81() MAKE_Devantech_TPA81_At(0xD0)



#endif /* TPA81_H_ */
