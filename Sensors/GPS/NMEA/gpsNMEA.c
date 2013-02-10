/*
 * $Id: gpsNMEA.c,v 1.7 2011/07/05 23:53:05 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: gpsNMEA.c,v $
 * Revision 1.7  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
 *
 * Revision 1.5  2010/12/02 19:08:40  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/06/14 19:06:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/04/25 03:25:15  clivewebster
 * Reduce rounding errors when reading longitude latitude from GPS message
 *
 * Revision 1.2  2010/02/17 23:45:18  clivewebster
 * Add flags for each returned value
 *
 * Revision 1.1  2010/01/30 21:20:40  clivewebster
 * Moved to sensors foldere
 *
 * Revision 1.5  2010/01/17 02:40:52  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 18:57:28  clivewebster
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
 * gpsNMEA.c
 *
 *  Created on: 07-May-2009
 *      Author: Clive Webster
 */
#include "gpsNMEA.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../../../core.h"


// Fwd refs
static void _gps_process_gprmc(GPS_NMEA* gps,int inx);
//static void _gps_process_gpgsa(GPS_NMEA* gps,int inx);
//static void _gps_process_gpgsv(GPS_NMEA* gps,int inx);
static void _gps_process_gpgga(GPS_NMEA* gps,int inx);
static void handler(unsigned char c, void* data);

// Message buffer to process a queued message, only used
// in the foreground so can have one across all devices
static char msgBuf[MAX_NMEA_MSG];

static void init(SENSOR* sensor){
	GPS_NMEA* gps = (GPS_NMEA*)sensor;

	// Allocate a buffer for the queued msgs
	if(gps->msgQueue==null){
		size_t space = MAX_NMEA_MSG * 3;					// Queue up to 3 messages
		gps->msgQueue = malloc(space);						// Allocate space
		bufferInit(&gps->msgQueueBuf,gps->msgQueue,space);	// Initialise queue buffer

	}

	// Allocate a buffer for the msg
	if(gps->msg==null){
		gps->msg = malloc(MAX_NMEA_MSG);
		__uartAttach(gps->uart,&handler, gps);
		_uartInit(gps->uart, gps->baudRate);
//		__uartFlushReceiveBuffer(gps->uart);
		#ifdef DEBUG
		PRINTF(stdout,"Start monitoring GPS\n");
		#endif
	}
}



// Service the next incoming character under interrupts

static void handler(unsigned char c, void* device){
	GPS_NMEA* gps = (GPS_NMEA*)device;

	// Get working data
	int msgIndex = gps->msgIndex;
	boolean error = gps->error;
	char* msg = gps->msg;

	// If its a $ then its the start of a msg
	if(c=='$'){
		msgIndex = 0;	// reset the msg buffer
		error=FALSE;	// clear error state
	}

	// Store the character
	if(msgIndex < MAX_NMEA_MSG-1){
		msg[msgIndex++] = c;	// store character
		msg[msgIndex] = 0;		// null terminate string
	}else{
		error=TRUE;				// the message is too long
	}

	// Test for end of line
	if(c=='\n' && error==FALSE){
		// Have got a line of length msgIndex

		// Test for minimum size of '$GP...<,>.....*xx<cr><lf>'
		if(msgIndex >= 8){
			if(msg[0]=='$' && msg[1]=='G' && msg[2]=='P' && msg[6]==',' && msg[msgIndex-1]=='\n' && msg[msgIndex-2]=='\r' && msg[msgIndex-5]=='*'){
				uint8_t myChecksum=0;
				uint8_t i;

				// Looks like a valid message
				msg[msgIndex-2]=0;	// chop off cr/lf
				msgIndex=0;

				// We only use GPRMC and GPGGA
				if( msg[3]=='R' && msg[4]=='M' && msg[5]=='C') goto ok;
				if( msg[3]=='G' && msg[4]=='G' && msg[5]=='A') goto ok;
				goto end;

				// Calculate the checksum
ok:				for(i=1; ;){
					c = msg[i++];

					// Shouldn't happen - but check for end of string
					if(c=='\0'){
						goto end;
					}

					if(c=='*'){
						break;
					}
					myChecksum ^= c;
				}

				// Remove checksum
				msg[i-1]=0;

				// String length including terminator
				size_t msgLen = i;

				if(bufferFreeSpace(&gps->msgQueueBuf) < msgLen){
					// not enough space in buffer
					goto end;
				}


				// Get received checksum
				uint8_t rxChecksum=0;
				c = msg[i++];
				if(c>='0' && c<='9'){
					rxChecksum += c-'0';
				}else{
					rxChecksum += c-'A'+10;
				}
				rxChecksum *= 16;
				c = msg[i++];
				if(c>='0' && c<='9'){
					rxChecksum += c-'0';
				}else{
					rxChecksum += c-'A'+10;
				}

				// If checksums agree then add message to queue
				if(rxChecksum==myChecksum){
					if( ! bufferQueue(&gps->msgQueueBuf, msg, msgLen) ){
						goto end;			// couldn't add it for some reason
					}

				}else{
					#ifdef DEBUG
					PRINTF(stdout,"Bad checksum:");
					#endif
				}
			}
		}
	}
end:
	// Set working data
	gps->msgIndex = msgIndex;
	gps->error = error;
}


static int _gps_skip(char* msg,int inx){
	while(msg[inx++] != ',');
	return inx;
}

static double _gps_getDegrees(char* msg,int inx){
	/*
	 * All this multiplication and division is causing rounding errors
	double rtn = strtod(&msg[inx], null);
	// convert to pure degrees [dd.dddd] format
	double degrees;
	double minutesfrac = modf(rtn/100.0, &degrees);
	rtn = degrees + (minutesfrac*100.0)/60.0;
	*/

	int16_t degrees=atoi(&msg[inx])/100;
	while(msg[inx]!='.') inx++;
	double rtn = degrees + strtod(&msg[inx-2],null)/ 60.0;

	// convert to radians
//	rtn *= (M_PI/180);
	return rtn;
}

static int _gps_getLongitude(char* msg, int inx, double * rtn){
	double longitude = _gps_getDegrees(msg,inx);
	inx = _gps_skip(msg,inx);

	// correct latitute for E/W
	if(msg[inx++] == 'W'){
		longitude = -longitude;
	}
	inx = _gps_skip(msg,inx);
	*rtn = longitude;
	return inx;
}

static int _gps_getLatitude(char* msg,int inx, double * rtn){
	double latitude = _gps_getDegrees(msg,inx);
	inx = _gps_skip(msg,inx);

	// get N/S indicator
	// correct latitute for N/S
	if(msg[inx++] == 'S'){
		latitude = -latitude;
	}
	inx = _gps_skip(msg,inx);
	*rtn = latitude;
	return inx;
}

/*
 RMC - NMEA has its own version of essential gps pvt (position, velocity, time) data. It is called RMC, The Recommended Minimum, which will look similar to:

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
 */
static void _gps_process_gprmc(GPS_NMEA* gps,int inx){
	double fixTime;
	double latitude;
	double longitude;
	double speed;
	double track;
	char* msg = msgBuf;

	// get UTC time [hhmmss.sss]
	fixTime = strtod(&msg[inx], null);
	inx = _gps_skip(msg,inx);
	if(fixTime != 0){
		gps->gps.work.fixTime = fixTime;
		gps->gps.work.changes.values.fixTime = TRUE;
	}

	// ignore if not active
	if(msg[inx++]!='A')
		return;
	inx = _gps_skip(msg,inx);

	// get latitude  [ddmm.mmmmm]
	inx = _gps_getLatitude(msg,inx,&latitude);

	// get longitude [ddmm.mmmmm]
	inx = _gps_getLongitude(msg,inx,&longitude);

	//Get speed in knots
	speed = strtod(&msg[inx], null);
	inx = _gps_skip(msg,inx);

	//Get track angle in degrees
	track = strtod(&msg[inx], null);

	gps->gps.work.valid = TRUE;
	gps->gps.work.longitude = longitude;
	gps->gps.work.latitude = latitude;
	gps->gps.work.speed = speed;
	gps->gps.work.track = track;

	gps->gps.work.changes.values.latitude =
	gps->gps.work.changes.values.longitude =
	gps->gps.work.changes.values.speed =
	gps->gps.work.changes.values.track = TRUE;

}

/*
GSA - GPS DOP and active satellites. This sentence provides details on the nature of the fix. It includes the numbers of the satellites being used in the current solution and the DOP. DOP (dilution of precision) is an indication of the effect of satellite geometry on the accuracy of the fix. It is a unitless number where smaller is better. For 3D fixes using 4 satellites a 1.0 would be considered to be a perfect number, however for overdetermined solutions it is possible to see numbers below 1.0.

There are differences in the way the PRN's are presented which can effect the ability of some programs
to display this data. For example, in the example shown below there are 5 satellites in the solution
and the null fields are scattered indicating that the almanac would show satellites in the null
positions that are not being used as part of this solution. Other receivers might output all of
the satellites used at the beginning of the sentence with the null field all stacked up at the end.
This difference accounts for some satellite display programs not always being able to display the
satellites being tracked. Some units may show all satellites that have ephemeris data without regard
to their use as part of the solution but this is non-standard.

  $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual)
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12)
     2.5      PDOP (dilution of precision)
     1.3      Horizontal dilution of precision (HDOP)
     2.1      Vertical dilution of precision (VDOP)

 */
//static void _gps_process_gpgsa(GPS_NMEA* gps, int inx){
//}


/*
GSV - Satellites in View shows data about the satellites that the unit might be able to find based on
its viewing mask and almanac data. It also shows current ability to track this data.
Note that one GSV sentence only can provide data for up to 4 satellites and thus there may need to be
3 sentences for the full information. It is reasonable for the GSV sentence to contain more satellites
than GGA might indicate since GSV may include satellites that are not used as part of the solution.
It is not a requirement that the GSV sentences all appear in sequence. To avoid overloading the data
bandwidth some receivers may place the various sentences in totally different samples since each
sentence identifies which one it is.

The field called SNR (Signal to Noise Ratio) in the NMEA standard is often referred to as signal strength.
SNR is an indirect but more useful value that raw signal strength. It can range from 0 to 99 and has units
of dB according to the NMEA standard, but the various manufacturers send different ranges of numbers with
different starting numbers so the values themselves cannot necessarily be used to evaluate different units.
The range of working values in a given gps will usually show a difference of about 25 to 35 between the
lowest and highest values, however 0 is a special case and may be shown on satellites that are in view but
not being tracked.

  $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75

Where:
      GSV          Satellites in view
      2            Number of sentences for full data
      1            sentence 1 of 2
      08           Number of satellites in view

      01           Satellite PRN number
      40           Elevation, degrees
      083          Azimuth, degrees
      46           SNR - higher is better
           for up to 4 satellites per sentence

 */
//static void _gps_process_gpgsv(GPS_NMEA* gps, int inx){
//}


/*
GGA - essential fix data which provide 3D location and accuracy data.

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *

If the height of geoid is missing then the altitude should be suspect. Some non-standard implementations report altitude with respect to the ellipsoid rather than geoid altitude. Some units do not report negative altitudes at all. This is the only sentence that reports altitude.
 */

static void _gps_process_gpgga(GPS_NMEA* gps, int inx){

	double fixTime;
	double latitude;
	double longitude;
	int satellites;
	double altitude;
	char* msg = msgBuf;

	// get UTC time [hhmmss.sss]
	fixTime = strtod(&msg[inx], null);
	inx = _gps_skip(msg,inx);
	if(fixTime != 0){
		gps->gps.work.fixTime = fixTime;
		gps->gps.work.changes.values.fixTime = TRUE;
	}

	// If no longitude or latitude then reject it
	if(msg[inx]==',' && msg[inx+1]==',')
		return;

	// get latitude  [ddmm.mmmmm]
	inx = _gps_getLatitude(msg,inx,&latitude);

	// get longitude [ddmm.mmmmm]
	inx = _gps_getLongitude(msg,inx,&longitude);

	// Test if it is valid
	if( (msg[inx] == '0') || (msg[inx] == ',') )
		return;
	inx = _gps_skip(msg,inx);

	// get number of satellites used in GPS solution
	satellites = atoi(&msg[inx]);
	inx = _gps_skip(msg,inx);

	// skip dilution
	inx = _gps_skip(msg,inx);

	// get altitude (in meters)
	altitude = strtod(&msg[inx], null);

	gps->gps.work.valid = TRUE;
	gps->gps.work.latitude = latitude;
	gps->gps.work.longitude = longitude;
	gps->gps.work.numSatellites = satellites;
	gps->gps.work.altitude = altitude;

	gps->gps.work.changes.values.latitude =
	gps->gps.work.changes.values.longitude =
	gps->gps.work.changes.values.numSatellites =
	gps->gps.work.changes.values.altitude = TRUE;
}

// Process any queued messages
static void read(SENSOR* sensor){
	GPS_NMEA* gps = (GPS_NMEA*)sensor;
	// Keep looping whilst there is a message
	while(bufferBytesUsed(&gps->msgQueueBuf)){

		uint8_t byte;

		// Got a message so unqueue it
		size_t dstIndex = 0;
		do{
			byte = 0;
			bufferGet(&gps->msgQueueBuf,&byte);
			msgBuf[dstIndex++] = byte;
		}while(byte != 0);

		// Now process the message in 'msgBuf'
		#ifdef DEBUG
		// Dump out the message
		PRINTF(stdout,"%s\n",msgBuf);
		#endif


		// Process the msg
		if(msgBuf[3]=='R' && msgBuf[4]=='M' && msgBuf[5]=='C' ){
			// $GPRMC msg
			_gps_process_gprmc(gps,7);
		}else if(msgBuf[3]=='G' && msgBuf[4]=='G' && msgBuf[5]=='A' ){
			// $GPGGA msg
			_gps_process_gpgga(gps,7);
		}
	}

	__gps_read(sensor);
}

// No point reading more often then every 500ms
GPS_CLASS const PROGMEM c_gps_nmea = MAKE_GPS_CLASS(&init,&read,0,500);
