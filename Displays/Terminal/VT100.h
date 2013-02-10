/*
 * $Id: VT100.h,v 1.1 2011/03/07 01:32:35 clivewebster Exp $
 * Revision History
 * ================
 * $Log: VT100.h,v $
 * Revision 1.1  2011/03/07 01:32:35  clivewebster
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
 *        File: VT100.h
 *  Created on: 3 Mar 2011
 *      Author: Clive Webster
 */

#ifndef VT100_H_
#define VT100_H_

#include "../../uart.h"
#include "../_display_common.h"

typedef struct s_vt100 {
	DISPLAY _display_;
	UART*	_uart_;
	BAUD_RATE baudrate;
} VT100;

extern DISPLAY_CLASS c_VT100;
#define MAKE_VT100(cols,rows,uart,baud,writer) { \
	MAKE_DISPLAY(&c_VT100, cols, rows, writer), \
	&((uart)->_uart_),baud	\
}

#endif /* VT100_H_ */
