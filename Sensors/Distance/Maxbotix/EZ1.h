/*
 * $Id: EZ1.h,v 1.5 2010/06/14 19:03:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: EZ1.h,v $
 * Revision 1.5  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:17:35  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.3  2009/12/11 17:17:57  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:32:13  clivewebster
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
 * Distance sensor from Maxbotix
 * Requires 1 ADC channel
 *
 *  Created on: 09-Sep-2009
 *      Author: Clive Webster
 *
 *
 */
#ifndef Maxbotix_EZ1_H_
#define Maxbotix_EZ1_H_
#include "../_distance_common.h"
#include "../../../a2d.h"

extern const DISTANCE_CLASS c_Maxbotix_EZ1;

typedef struct s_Maxbotix_EZ1{
	DISTANCE 		distance;
	ADC_CHANNEL    	adcPin;
}Maxbotix_EZ1;

// Create the sensor
#define MAKE_Maxbotix_EZ1(adcPin) { \
	MAKE_DISTANCE_SENSOR(c_Maxbotix_EZ1), \
	adcPin }

#endif


