/*
 * $Id: led.h,v 1.4 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: led.h,v $
 * Revision 1.4  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/11/02 19:00:09  clivewebster
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
 *
 */
#ifndef _LED_H_
#define _LED_H_

#include "iopin.h"

typedef struct s_led {
	const IOPin* pin;
	boolean activeHigh:1;
	boolean initialised:1;
} LED;

#define MAKE_LED(pin, activehigh) {pin, activehigh, FALSE }

// The status LED
extern LED statusLED;



void LED_set(LED* led,	boolean value);

static __inline__ void LED_on(LED* led){
	LED_set(led,TRUE);
}

static __inline__ void LED_off(LED* led){
	LED_set(led,FALSE);
}

static __inline__ void statusLEDregister(const IOPin* pin, boolean activeHigh){
	statusLED.pin = pin;
	statusLED.activeHigh = activeHigh;
	statusLED.initialised=FALSE;
	LED_on(&statusLED);
}

static __inline__ void statusLED_on(){
	if(pin_is_output(statusLED.pin)){
		LED_on(&statusLED);
	}
}

static __inline__ void statusLED_off(){
	if(pin_is_output(statusLED.pin)){
		LED_off(&statusLED);
	}
}

static __inline__ void statusLED_set(boolean value){
	LED_set(&statusLED, value);
}




#endif
