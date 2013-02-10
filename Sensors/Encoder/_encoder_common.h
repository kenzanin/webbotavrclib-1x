/*
 * $Id: _encoder_common.h,v 1.10 2010/10/15 12:16:36 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _encoder_common.h,v $
 * Revision 1.10  2010/10/15 12:16:36  clivewebster
 * Fix encoderSubtract macro
 *
 * Revision 1.9  2010/09/08 18:25:32  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.8  2010/08/14 21:04:02  clivewebster
 * Store encoder timings
 *
 * Revision 1.7  2010/08/10 22:45:15  clivewebster
 * Change numStripes from uint8_t to uint16_t
 *
 * Revision 1.6  2010/06/14 19:05:36  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/06/13 18:14:51  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 18:36:20  clivewebster
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
 * _encoder_common.h
 *
 *  Created on: 20-Sep-2009
 *      Author: Clive Webster
 */

#ifndef _ENCODER_COMMON_H_
#define _ENCODER_COMMON_H_
#include "../_sensor_common.h"
#include "../../timer.h"

// Define the data type used by all encoders for counting ticks
// Changing this will mean changing encoderDump
#define ENCODER_TYPE int16_t

// Define forward references
struct s_encoder;

// Define the api for the class
typedef struct PROGMEM c_encoder{
	SENSOR_CLASS sensor;						// include parent class
} ENCODER_CLASS;

#define MAKE_ENCODER_CLASS(init,read,startup,delay) {MAKE_SENSOR_CLASS(init,read,startup,delay)}

// Define the data for each encoder sensor
typedef struct s_encoder{
	SENSOR			sensor;			// Include fields from parent type
	ENCODER_TYPE	value;			// Last read value
	TICK_COUNT		timeSinceLastTick;
	TICK_COUNT		durationOfLastTick;
	volatile ENCODER_TYPE counter;	// The tick counter - don't access this directly !!
	const boolean inverted:1;		// Is it inverted (has the same meaning as with a motor)
	const uint16_t numStripes;		// Number of stripes on the wheel
	TIMER_SNAPSHOT snapshot[2];		// The timer snapshots for the last two ticks
	uint8_t		   tickIndex;		// The next index into ticks[]
	boolean		   interpolate;		// Interpolate between ticks
} ENCODER;

#define MAKE_ENCODER_SENSOR(class, inverted, numStripes,interpolate) {MAKE_SENSOR(&(class.sensor)), \
		(ENCODER_TYPE)0, \
		(TICK_COUNT)0, \
		(TICK_COUNT)0, \
		(ENCODER_TYPE)0, \
		inverted, numStripes, { {NULL,0,0}, {NULL,0,0} }, 0, interpolate}

// Create macros to make the virtual methods easier to call
#define encoderRead(device) _sensorRead_(&device.encoder.sensor)
#define encoderInit(device) _sensorInit_(&device.encoder.sensor)

// Dump the value of a sensor to the rprintf destination
void _encoderDump(const ENCODER* device);
#define encoderDump(device) _encoderDump(&device.encoder);

#define encoderSubtract( device,count ) _encoderSubtract( &((device).encoder),count)
void _encoderSubtract(ENCODER* encoder, ENCODER_TYPE count);

// Return how many ticks there are per revolution
#define encoderTicksPerRevolution(device) _encoderTicksPerRevolution( &((device).encoder) )
uint16_t _encoderTicksPerRevolution(const ENCODER* encoder);

// -- Support for 'old' calls
#define quadratureSubtract(encoder,count) encoderSubtract(encoder,count)

// Return how many ticks there are per revolution
#define quadratureTicksPerRevolution(encoder) encoderTicksPerRevolution(encoder)

#if defined(BUILDING_LIBRARY)
// Low level routine - do not call it yourself!
void __encoder_read(SENSOR* sensor);
void _encoder_tick(ENCODER* encoder);
#endif

#endif /* _ENCODER_COMMON_H_ */
