/*
 * $Id: Temperature.c,v 1.3 2010/06/14 19:10:52 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: Temperature.c,v $
 * Revision 1.3  2010/06/14 19:10:52  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:43:05  clivewebster
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
 * Phidget_Temperature.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 */
#include "Temperature.h"
#include "../../phidgetsCommon.h"
#include "../../../fraction.h"

static const uint16_t PROGMEM phidget_humidity_frac[] = {0, 6, 36, 0};

static void __Phidget_Temperature_read(SENSOR* sensor){
	Phidget_Temperature* device = (Phidget_Temperature*)sensor;
	uint16_t adc = __phidgetsRead(device->adcPin);
	// scale it:  (0.1946 * adc)-41.98
	adc -= 200;
	adc >>= 2;
	device->temperature.celsius = adc;
}

TEMPERATURE_CLASS const c_Phidget_Temperature = MAKE_TEMPERATURE_CLASS(null,&__Phidget_Temperature_read, 0 ,0 );
