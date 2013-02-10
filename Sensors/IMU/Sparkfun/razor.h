/*
 * $Id: razor.h,v 1.1 2010/07/02 00:43:12 clivewebster Exp $
 * Revision History
 * ================
 * $Log: razor.h,v $
 * Revision 1.1  2010/07/02 00:43:12  clivewebster
 * Added. But requires firmware to be uploaded to the Razor board.
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
 *        File: razor.h
 *  Created on: 24 Jun 2010
 *      Author: Clive Webster
 *
 *   The Sparkfun Razor 9 DoF IMU - using WebbotLib firmware
 */

#ifndef RAZOR_H_
#define RAZOR_H_

#include "../../../_uart_common.h"
#include "../_imu_common.h"

extern IMU_CLASS c_razor;

typedef struct s_RAZOR{
	IMU			imu;							// Include fields from parent type
	UART*		uart;							// The UART to talk to the razor
	BAUD_RATE   baud;
}RAZOR;

#define MAKE_RAZOR(uart, baud) { \
	MAKE_IMU_SENSOR(c_razor), \
	&((uart)->_uart_), baud}

static __inline__ void razorLED(const RAZOR* razor, boolean on){
	_uartSendByte(razor->uart, (on) ? '4' : '3');
}

#endif /* RAZOR_H_ */
