/*
 * $Id: Humidity.c,v 1.3 2010/06/14 19:09:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Humidity.c,v $
 * Revision 1.3  2010/06/14 19:09:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:40:29  clivewebster
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
 * Phidget_Humidity.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 */
#include "Humidity.h"
#include "../../phidgetsCommon.h"
#include "../../../fraction.h"

static const uint16_t PROGMEM phidget_humidity_frac[] = {0, 6, 36, 0};

static void __Phidget_Humidity_read(SENSOR* sensor){
	Phidget_Humidity* device = (Phidget_Humidity*)sensor;
	uint16_t adc = __phidgetsRead(device->adcPin);
	// scale it:  (0.1946 * adc)-41.98
	adc = fraction32(adc, phidget_humidity_frac);
	adc -= 42;
	device->humidity.percent = adc;
}

HUMIDITY_CLASS c_Phidget_Humidity = MAKE_HUMIDITY_CLASS(null,&__Phidget_Humidity_read, 0 ,0 );
