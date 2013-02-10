/*
 * $Id: MB7077.c,v 1.1 2010/06/20 19:31:18 clivewebster Exp $
 * Revision History
 * ================
 * $Log: MB7077.c,v $
 * Revision 1.1  2010/06/20 19:31:18  clivewebster
 * Added in Version 1.21
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
 *        File: MB7077.c
 *  Created on: 19 Jun 2010
 *      Author: Clive Webster
 */
#include "MB7077.h"

#include "../../../fraction.h"

// 4.3 as a fraction
static const uint16_t PROGMEM _frac[]     = {4,  4, 20, 0};

// Read all the values and store into the device
static void _read(SENSOR* sensor){
	Maxbotix_MB7077* sonar = (Maxbotix_MB7077*)sensor;
	uint16_t adc = a2dConvert10bit(sonar->adcPin);
	if(sonar->inWater){
		// Multiply by 4.3
		adc = fraction32(adc,_frac);
	}
	sonar->distance.cm = adc;
}

DISTANCE_CLASS c_Maxbotix_MB7077 = MAKE_DISTANCE_CLASS(null,&_read,0,0);

