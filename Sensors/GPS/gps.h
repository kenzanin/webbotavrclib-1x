/*
 * $Id: gps.h,v 1.4 2011/07/05 23:53:05 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: gps.h,v $
 * Revision 1.4  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
 *
 * Revision 1.3  2010/06/14 19:06:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/18 00:01:15  clivewebster
 * Add flags for each returned value
 *
 * Revision 1.1  2010/01/30 21:20:40  clivewebster
 * Moved to sensors foldere
 *
 * Revision 1.3  2010/01/17 02:42:44  clivewebster
 * *** empty log message ***
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
 * gps.h
 *
 *  Created on: 07-May-2009
 *      Author: Clive Webster
 *
 * Define a data structure for storing the output of gps devices.
 *
 * Physical gps devices can be created using gpsNMEA.h for an NMEA compliant device.
 *
 * NB Not all GPS receivers will output all of the information.
 */

#ifndef GPS_H_
#define GPS_H_

#include "../../libdefs.h"

#ifdef __cplusplus
/* ===================== C Code ===============================================*/
extern "C" {
#endif

typedef struct s_gps {
	boolean valid;			// Is set to true once the satellites have been found and the data is valid
	double 	fixTime;		// The fix time
	double 	longitude;		// in degrees
	double 	latitude;		// in degrees
	double 	speed;			// in knots
	int		numSatellites;	// number of satellites
	double	altitude;		// in meters
	double  track;			// Track angle in degrees

	union {
		struct{
		boolean fixTime:1;
		boolean longitude:1;
		boolean latitude:1;
		boolean speed:1;
		boolean	numSatellites:1;
		boolean altitude:1;
		boolean track:1;
		} values;			// boolean list of variables modified
		uint8_t all;		// all of them clumped into one
	} changes;

} GPS;
#define MAKE_GPS() {FALSE,0,0,0,0,0,0,0}

#ifdef __cplusplus
}
#endif

#endif /* GPS_H_ */
