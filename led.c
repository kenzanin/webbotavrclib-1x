/*
 * $Id: led.c,v 1.5 2010/12/02 19:09:14 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: led.c,v $
 * Revision 1.5  2010/12/02 19:09:14  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/07/01 23:51:18  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 19:00:09  clivewebster
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
 * led.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "led.h"

// Create the status LED
LED statusLED; // All empty MAKE_LED(null,FALSE);

void LED_set(LED* led,	boolean value){
	if(led->pin){

		if(!led->activeHigh){
		 	value=!value;
		}

		if(!led->initialised){
			pin_make_output(led->pin,value);
			led->initialised=TRUE;
		}
		pin_set(led->pin,value);
	}
}

