/*
 * $Id: SRF04_Sonar.h,v 1.2 2010/06/14 19:00:14 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: SRF04_Sonar.h,v $
 * Revision 1.2  2010/06/14 19:00:14  clivewebster
 * Add copyright license info
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
 * SRF04_Sonar.h
 *
 *  Created on: 16-Nov-2009
 *      Author: Clive Webster
 *
 *  Devantech SRF04 Sonar.
 *
 */

#ifndef SRF04_Sonar_H_
#define SRF04_Sonar_H_
#include "../_distance_common.h"
#include "../../../iopin.h"

extern DISTANCE_CLASS c_Devantech_SRF04;

typedef struct s_Devantech_SRF04{
	DISTANCE 		distance;
	const IOPin*    out;
	const IOPin*    in;
}Devantech_SRF04;

// Create the sensor
#define MAKE_Devantech_SRF04(trigger,echo) { \
	MAKE_DISTANCE_SENSOR(c_Devantech_SRF04), \
	trigger,echo }

#endif /* SRF04_Sonar */
