/*
 * $Id: gpsNMEA.h,v 1.5 2011/07/05 23:53:05 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: gpsNMEA.h,v $
 * Revision 1.5  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
 *
 * Revision 1.3  2010/06/14 19:06:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/17 23:45:18  clivewebster
 * Add flags for each returned value
 *
 * Revision 1.1  2010/01/30 21:20:40  clivewebster
 * Moved to sensors foldere
 *
 * Revision 1.2  2009/11/02 18:57:28  clivewebster
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
 * gpsNMEA.h
 *
 *  Created on: 07-May-2009
 *      Author: Clive Webster
 *
 * A GPS device that output NMEA compliant messages
 * The following messages are supported:-
 *   $GPRMC
 *   $GPGGA
 */

#ifndef GPSNMEA_H_
#define GPSNMEA_H_

#include "../gps.h"
#include "../_gps_common.h"
#include "../../../uart.h"
#include "../../../buffer.h"

#ifdef __cplusplus
/* ===================== C Code ===============================================*/
extern "C" {
#endif

// The longest possible NMEA msg length, 82 bytes plus zero delimiter
#define MAX_NMEA_MSG 83

extern const GPS_CLASS c_gps_nmea;

typedef struct s_gps_nmea{
	GPS_COMMON gps;				// Include stuff common to all gps devices
	UART*	uart;				// The uart we are receiving the messages on
	char*	msg;				// Temporary area for the msg currently being received
	int		msgIndex;			// Current Index into msg
	boolean	error;				// is there an error in the current msg
	BAUD_RATE  baudRate;		// The baud rate to use
	uint8_t*   msgQueue;			// The valid received messages waiting to be processed
	cBuffer msgQueueBuf;		// Wraps 'msgs' into a buffer
}GPS_NMEA;

#define MAKE_GPS_NMEA(uart,baudRate) { MAKE_GPS_SENSOR(c_gps_nmea), &((uart)->_uart_), null, 0, TRUE, baudRate,null }

// Initialise the gps and start reading
//void gpsNMEAinit(GPS_NMEA* gps);
// Now uses the sensor init routine
#define gpsNMEAinit(device) gpsInit(device)

// Call this routine to access incoming gps info
// Return TRUE if something has changed since last time
// Now uses the sensor read routine
//boolean gpsNMEAprocess(GPS_NMEA* gps);
#define gpsNMEAprocess(device) _sensorRead_( &((device)->gps.sensor) )

#ifdef __cplusplus
}

#endif

#endif /* GPSNMEA_H_ */
