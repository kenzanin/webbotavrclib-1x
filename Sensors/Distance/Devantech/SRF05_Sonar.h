/*
 * $Id: SRF05_Sonar.h,v 1.4 2010/06/14 19:00:14 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: SRF05_Sonar.h,v $
 * Revision 1.4  2010/06/14 19:00:14  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:32:14  clivewebster
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
 * SRF05_Sonar.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Devantech SRF05 Sonar. Claims up to 430cm
 *
 *  Requires one I/O pin that is connected to the +5v regulated supply.
 *  On the Axon this means you must use one of the ADC pins - using other I/O pins
 *  will fry the device.
 *
 *  In order to avoid echoes the fastest sample time is every 50mS. This library
 *  will test this condition and if you attempt to read faster than this then we
 *  will return the previous reading.
 *
 *  We operate the sensor in 'Mode 2' so you should connect the 'Mode' pin on the sonar board to
 *  the ground pin.
 *
 *  SRF05_Sonar sonar = MAKE_SRF05_Sonar(C0);
 *  distanceRead(sonar);	// Read the sonar
 *  sonar.distance.cm  holds the result
 */

#ifndef SRF05_Sonar_H_
#define SRF05_Sonar_H_
#include "../_distance_common.h"
#include "../../../iopin.h"

extern const DISTANCE_CLASS c_Devantech_SRF05;

typedef struct s_Devantech_SRF05{
	DISTANCE 		distance;
	const IOPin*    ioPin;
}Devantech_SRF05;

// Create the sensor
#define MAKE_Devantech_SRF05(ioPin) { \
	MAKE_DISTANCE_SENSOR(c_Devantech_SRF05), \
	ioPin }

#endif /* SRF05_Sonar */
