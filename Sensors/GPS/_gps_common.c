/*
 * $Id: _gps_common.c,v 1.1 2011/07/05 23:53:05 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _gps_common.c,v $
 * Revision 1.1  2011/07/05 23:53:05  clivewebster
 * Change to buffer incoming messages without overflow
 *
 * ================
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
 *  Created on: 28-Apr-2011
 *      Author: Clive Webster
 */
#include "_gps_common.h"
#include <stdlib.h>
#include <string.h>


void __gps_read(SENSOR* sensor){
	GPS_COMMON* gps = (GPS_COMMON*)sensor;
	CRITICAL_SECTION{
		memcpy(&gps->info,&gps->work,sizeof(GPS));

		// Nothing has been changed since last read
		gps->work.changes.all = 0;
	}

//	return (gps->info.changes.all == 0) ? FALSE : TRUE;
}

