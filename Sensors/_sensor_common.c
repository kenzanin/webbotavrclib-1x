/*
 * $Id: _sensor_common.c,v 1.4 2010/06/14 19:12:24 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _sensor_common.c,v $
 * Revision 1.4  2010/06/14 19:12:24  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/02/09 16:44:49  clivewebster
 * Changed time of last read to be before the sensor is read
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
 * _sensor_common.c
 *
 *  Created on: 20-Sep-2009
 *      Author: Clive Webster
 */
#include "_sensor_common.h"
#include "../timer.h"

void _sensorInit_(SENSOR* sensor){
	if(sensor->initialised==FALSE){
		// Locate the class definition in PROGMEM
		const SENSOR_CLASS* class = sensor->class;

		// call the initialisation code
		void (*fnInit)(SENSOR*) = (void (*)(SENSOR*))pgm_read_word(&class->initialise);
		if(fnInit!=null){
			uint16_t ms_from_startup = pgm_read_word(&class->ms_from_startup);
			if(ms_from_startup!=0){
				if(g_heartbeat==null){
					delay_ms(ms_from_startup);
				}else{
					// assumes clock is set up
					TICK_COUNT us_from_startup = ((TICK_COUNT)ms_from_startup) * 1000UL;
					TICK_COUNT power_on = (TICK_COUNT)0;
					while(!clockHasElapsed( power_on, us_from_startup) );
				}
			}

			// Call the start up code
			fnInit(sensor);
		}

		// It has now been initialised
		sensor->initialised = TRUE;
	}
}

boolean  _sensorRead_(SENSOR* sensor){
	// Locate the class definition in PROGMEM
	const SENSOR_CLASS* class = sensor->class;

	// If there is some initialisation code and it has not been called then set an error
	if(sensor->initialised==FALSE && pgm_read_word(&class->initialise)!=0){
		setError(SENSOR_NOT_INITIALISED);
		return FALSE;
	}

	// If we are calling too quickly then return the old value
	TICK_COUNT now = clockGetus();
	uint16_t ms_between_reads = pgm_read_word(&class->ms_between_reads);
	if(ms_between_reads!=0){
		TICK_COUNT us_between_reads = ((TICK_COUNT)ms_between_reads) * 1000UL;
		if(g_heartbeat==null){
			delay_us(us_between_reads);
		}else{
			if(!clockHasElapsed( sensor->lastRead, us_between_reads) ){
				return FALSE;
			}
		}
	}

	// call the read code
	void (*fnRead)(SENSOR*) = (void (*)(SENSOR*))pgm_read_word(&class->read);
	fnRead(sensor);

	// Save time last read
	sensor->lastRead = now;
	return TRUE;
}

