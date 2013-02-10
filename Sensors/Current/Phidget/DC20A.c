/*
 * $Id: DC20A.c,v 1.3 2010/06/14 18:57:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: DC20A.c,v $
 * Revision 1.3  2010/06/14 18:57:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:23:50  clivewebster
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
 *
 * Phidget_DC20A.c
 *
 *  Created on: 27-Mar-2009
 *      Author: Clive Webster
 */
#include "DC20A.h"
#include "../../phidgetsCommon.h"

static void __Phidget_DC20A_read(SENSOR* sensor){
	Phidget_DC20A* device = (Phidget_DC20A*)sensor;
	uint16_t amps = __phidgetsRead(device->pin) - 500;
	// scale it
	amps /= 20;

	device->current.amps = amps;
}

CURRENT_CLASS c_Phidget_DC20A = MAKE_CURRENT_CLASS(null,&__Phidget_DC20A_read, 0 ,0 );

