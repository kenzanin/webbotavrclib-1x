/*
 * $Id: HD44780.h,v 1.3 2011/03/07 01:33:41 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: HD44780.h,v $
 * Revision 1.3  2011/03/07 01:33:41  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.2  2010/06/14 18:45:05  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/02/18 01:45:09  clivewebster
 * Added
 *
 * ===========
 *
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
 * HD44780.h
 *
 *  Created on: 09-Feb-2010
 *      Author: Clive Webster
 *
 *  Define a generic parallel display based on the HD44780 or SED1278 controller chips
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "../../iopin.h"
#include "../_display_common.h"


extern DISPLAY_CLASS c_HD44780;


typedef struct s_hd44780{
	DISPLAY _display_;		// Common display stuff
	const IOPin* ctrlRS;
	const IOPin* ctrlRW;	// Read or write output pin
	const IOPin* ctrlE;		// The enable pin
	const IOPin* data[8];	// The low nibble data pins (only used for 8 bit mode)
	const boolean fourBit;	// Are we in 4 bit mode?
} HD44780;

#define MAKE_HD44780_8PIN(columns, rows ,rs,rw,e,d0,d1,d2,d3,d4,d5,d6,d7, writer) {\
	MAKE_DISPLAY(&c_HD44780, columns, rows, writer), rs,rw,e, {d0,d1,d2,d3,d4,d5,d6,d7}, FALSE \
}

#define MAKE_HD44780_4PIN(columns, rows,rs,rw,e,d4,d5,d6,d7, writer) {\
	MAKE_DISPLAY(&c_HD44780, columns, rows, writer), rs,rw,e, {null,null,null,null, d4,d5,d6,d7}, TRUE \
}

#endif /* HD44780_H_ */
