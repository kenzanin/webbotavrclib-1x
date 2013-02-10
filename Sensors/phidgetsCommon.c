/*
 * $Id: phidgetsCommon.c,v 1.7 2010/06/14 19:12:24 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: phidgetsCommon.c,v $
 * Revision 1.7  2010/06/14 19:12:24  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/01/24 16:57:59  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/12/11 17:16:44  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.4  2009/11/02 18:46:09  clivewebster
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
 * phidgetsCommon.c
 *
 *  Created on: 28-Mar-2009
 *      Author: Clive Webster
 *
 *
 */

#include "phidgetsCommon.h"
#include "../core.h"
#include "../a2d.h"

uint16_t __phidgetsRead(ADC_CHANNEL adcPin){
	uint16_t raw = a2dConvert10bit(adcPin);

	return interpolateU(raw, 0, 1023, 0, 1000);

}
