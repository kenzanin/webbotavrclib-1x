/*
 * $Id: MatrixOrbital.h,v 1.2 2010/06/14 18:45:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MatrixOrbital.h,v $
 * Revision 1.2  2010/06/14 18:45:04  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/02/18 01:45:08  clivewebster
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
 * MatrixOrbital.h
 *
 *  Created on: 10-Feb-2010
 *      Author: Clive Webster
 *
 *  Serial displays of different sizes
 */

#ifndef MATRIX_ORBITAL_H_
#define MATRIX_ORBITAL_H_

#include "../../uart.h"
#include "../_display_common.h"
typedef struct s_matrix_orbital {
	DISPLAY _display_;
	UART*	_uart_;
	BAUD_RATE baudrate; // 9600 or 19200
} MATRIX_ORBITAL;

extern const DISPLAY_CLASS c_MatrixOrbital;

#define MAKE_MATRIX_ORBITAL_MOSAL162A(uart,baud,writer) { \
	MAKE_DISPLAY(&c_MatrixOrbital, 16, 2, writer), \
	&((uart)->_uart_),baud	\
}

#define MAKE_MATRIX_ORBITAL_MOSAL202A(uart,baud,writer) { \
	MAKE_DISPLAY(&c_MatrixOrbital, 20, 2, writer), \
	&((uart)->_uart_),baud	\
}


#endif /* MATRIX_ORBITAL_H_ */
