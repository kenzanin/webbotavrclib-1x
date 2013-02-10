/*
 * $Id: 30V.c,v 1.3 2010/06/14 19:11:42 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: 30V.c,v $
 * Revision 1.3  2010/06/14 19:11:42  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:44:48  clivewebster
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
 * Phidget_30V.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 */
#include "30V.h"
#include "../../phidgetsCommon.h"

static void __Phidget_30V_read(SENSOR* sensor){
	Phidget_30V* device = (Phidget_30V*)sensor;
	uint16_t volts = __phidgetsRead(device->adcPin);
	// scale it: (adc - 500) * 0.06
	volts -= 500;
	volts *= 3;
	volts /= 50;

	device->voltage.volts = volts;
}

VOLTAGE_CLASS c_Phidget_30V = MAKE_VOLTAGE_CLASS(null,&__Phidget_30V_read, 0 ,0 );
