/*
 * $Id: LCD03.h,v 1.1 2011/03/07 01:32:51 clivewebster Exp $
 * Revision History
 * ================
 * $Log: LCD03.h,v $
 * Revision 1.1  2011/03/07 01:32:51  clivewebster
 * Added in Version 1.30
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
 *        File: LCD03.h
 *  Created on: 6 Mar 2011
 *      Author: Clive Webster
 */

#ifndef LCD03_H_
#define LCD03_H_

#include "../_display_common.h"
#include "../../i2cBus.h"

extern DISPLAY_CLASS c_lcd03_i2c;

typedef struct s_lcd03_i2c{
	DISPLAY _display_;
	I2C_DEVICE	  i2cInfo;
	uint8_t 	fifo;
} LCD03_I2C;

#define MAKE_LCD03_I2C_At(i2cAddr,writer) { \
	MAKE_DISPLAY(&c_lcd03_i2c, 20, 4, writer), \
	MAKE_I2C_DEVICE(i2cAddr), 0 \
	}

// Use the default I2C address
#define MAKE_LCD03_I2C(writer) MAKE_LCD03_I2C_At(0xC6,writer)

#endif /* LCD03_H_ */
