/*
 * $Id: PingSonar.h,v 1.4 2010/06/14 19:03:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: PingSonar.h,v $
 * Revision 1.4  2010/06/14 19:03:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/12/11 17:17:55  clivewebster
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
 * PingSonar.h
 *
 *  Created on: 26-Mar-2009
 *      Author: Clive Webster
 *
 *  Parallax Ping Sonar. Claims 2cm to 300cm
 *
 *  Requires one I/O pin that is connected to the +5v regulated supply.
 *  On the Axon this means you must use one of the ADC pins - using other I/O pins
 *  will fry the device.
 *
 *  In order to avoid echoes the fastest sample time is every 200uS. This library
 *  will test this condition and if you attempt to read faster than this then we
 *  will return the previous reading.
 *
 *  PingSonar sonar = MAKE_PingSonar(C0);
 *  distanceRead(sonar);	// Read the sonar
 *  sonar.distance.cm  holds the result
 */

#ifndef PingSonar_H_
#define PingSonar_H_

#include "../_distance_common.h"
#include "../../../iopin.h"

extern const DISTANCE_CLASS c_PingSonar;

typedef struct s_PingSonar{
	DISTANCE 		distance;
	const IOPin*    ioPin;
}PingSonar;

// Create the sensor
#define MAKE_PingSonar(ioPin) { \
	MAKE_DISTANCE_SENSOR(c_PingSonar), \
	ioPin }

#endif /* PingSonar */
