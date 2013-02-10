/*
 * $Id: serLCD.h,v 1.4 2011/07/06 00:15:15 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: serLCD.h,v $
 * Revision 1.4  2011/07/06 00:15:15  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.3  2011/03/07 01:33:42  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.2  2010/06/14 18:45:06  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/02/18 01:45:09  clivewebster
 * Added
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
 * serLCD.h
 *
 *  Created on: 10-Feb-2010
 *      Author: Clive Webster
 *
 *  Support the Sparkun 'serLCD' backpack to allow serial access to a parallel LCD
 *
 */

#ifndef SERLCD_H_
#define SERLCD_H_

#include "../../uart.h"
#include "../_display_common.h"

typedef struct s_sparkfun_serLCD {
	DISPLAY _display_;
	UART*	_uart_;
	BAUD_RATE baudrate;
} SPARKFUN_SERLCD;

extern const DISPLAY_CLASS c_SERLCD;
#define MAKE_SPARKFUN_SERLCD_16_2(uart,baud,writer) { \
	MAKE_DISPLAY(&c_SERLCD, 16, 2, writer), \
	&((uart)->_uart_),baud	\
}
#define MAKE_SPARKFUN_SERLCD_16_4(uart,baud,writer) { \
	MAKE_DISPLAY(&c_SERLCD, 16, 4, writer), \
	&((uart)->_uart_),baud	\
}

#define MAKE_SPARKFUN_SERLCD_20_2(uart,baud,writer) { \
	MAKE_DISPLAY(&c_SERLCD, 20, 2, writer), \
	&((uart)->_uart_),baud	\
}
#define MAKE_SPARKFUN_SERLCD_20_4(uart,baud,writer) { \
	MAKE_DISPLAY(&c_SERLCD, 20, 4, writer), \
	&((uart)->_uart_),baud	\
}


#endif /* SERLCD_H_ */
