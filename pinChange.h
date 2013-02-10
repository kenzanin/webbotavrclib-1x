/*
 * $Id: pinChange.h,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: pinChange.h,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 19:00:56  clivewebster
 * Added revision log
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
 * pinChange.h
 *
 *  Created on: 03-Apr-2009
 *      Author: Clive Webster
 */

#ifndef PINCHANGE_H_
#define PINCHANGE_H_

#include "iopin.h"

/******************************************************

	Functionality to attach a callback routine that
	is called as an interrupt when the poin changes.
	This is not support on ATMega8/32 processors

******************************************************/
// Define a callback for PinChange events
typedef void (*PinChangeCallback)(const IOPin* io,boolean val, volatile void* data);

typedef struct s_pinchange{
	volatile PinChangeCallback callback;
	volatile void*			   data;
	volatile boolean		   lastValue;
}PIN_CHANGE;



void pin_change_attach(const IOPin* io,PinChangeCallback callback, void* data);
void pin_change_dettach(const IOPin* io);


#if defined(BUILDING_LIBRARY)
// we are building the lib
extern const IOPin* PROGMEM PCINT_PINS[];
extern const uint8_t NUM_PCINT_PINS;
extern PIN_CHANGE pcCallbacks[];
#else
// we are building the app
#ifdef _NUM_PCINT_PINS
PIN_CHANGE pcCallbacks[_NUM_PCINT_PINS];
#else
#error "This device does not support Pin Change interrupts"
#endif
#endif
#endif /* PINCHANGE_H_ */
