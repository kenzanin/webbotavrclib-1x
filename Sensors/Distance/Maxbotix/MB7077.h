/*
 * $Id: MB7077.h,v 1.1 2010/06/20 19:31:18 clivewebster Exp $
 * Revision History
 * ================
 * $Log: MB7077.h,v $
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
 *        File: MB7077.h
 *  Created on: 19 Jun 2010
 *      Author: Clive Webster
 *
 *  A sonar that can also be used under water (with some gloop!)
 *  The analogue output on pin 3 is Vcc/1024 per cm
 *  So a 10 bit analogue read will return the distance in cm
 *
 *  Current average=3.4mA peak=100mA
 *
 *  Datasheet: http://www.maxbotix.com/uploads/MB7067-MB7077_Datasheet.pdf
 *
 */

#ifndef MB7077_H_
#define MB7077_H_
#include "../_distance_common.h"
#include "../../../a2d.h"


extern DISTANCE_CLASS c_Maxbotix_MB7077;

typedef struct s_Maxbotix_MB7077{
	DISTANCE 		distance;
	ADC_CHANNEL    	adcPin;
	boolean			inWater;
}Maxbotix_MB7077;

// Create the sensor
#define MAKE_Maxbotix_MB7077(adcPin,inWater) { \
	MAKE_DISTANCE_SENSOR(c_Maxbotix_MB7077), \
	adcPin, inWater }

#endif /* MB7077_H_ */
