/*
 * $Id: _sensor_common.h,v 1.3 2010/06/14 19:12:24 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _sensor_common.h,v $
 * Revision 1.3  2010/06/14 19:12:24  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:46:09  clivewebster
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
 * _sensor_common.h
 *
 *  Created on: 20-Sep-2009
 *      Author: Clive Webster
 */

#ifndef _SENSOR_COMMON_H_
#define _SENSOR_COMMON_H_

#include "../libdefs.h"

// Define forward references
struct s_sensor;

// Define the api for the class
typedef struct PROGMEM c_sensor{
	void	(*initialise)(struct s_sensor*); 	// Initialise a sensor
	void 	(*read)(struct s_sensor*);			// Read all the values and store into the device
	const uint16_t ms_from_startup;				// number of ms from power on before it can be read
	const uint16_t ms_between_reads;			// number of ms between reads
} SENSOR_CLASS;

#define MAKE_SENSOR_CLASS(init,read,startup,delay) {init,read,startup,delay}

// Define the data for each sensor
typedef struct s_sensor{
	const SENSOR_CLASS* class;					// The class it belongs to
	TICK_COUNT  lastRead;						// The time when it was last read
	boolean  	initialised;					// Has it been initialised
} SENSOR;

#define MAKE_SENSOR(class) {class,(TICK_COUNT)0, FALSE}

// The virtual methods for sensors
extern boolean  _sensorRead_(SENSOR* sensor);
extern void     _sensorInit_(SENSOR* sensor);

#endif /* _SENSOR_COMMON_H_ */
