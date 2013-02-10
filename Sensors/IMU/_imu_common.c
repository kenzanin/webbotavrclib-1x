/*
 * $Id: _imu_common.c,v 1.1 2010/07/02 00:43:34 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _imu_common.c,v $
 * Revision 1.1  2010/07/02 00:43:34  clivewebster
 * Added
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
 *        File: _imu_common.c
 *  Created on: 24 Jun 2010
 *      Author: Clive Webster
 */
#include "_imu_common.h"
#include "../../rprintf.h"


void _imuDump(const IMU* device){
	rprintf("(gyro=%d,%d,%d  accel=%d,%d,%d yaw=%d pitch=%d roll=%d)",
			device->x_axis_degrees_per_second,
			device->y_axis_degrees_per_second,
			device->z_axis_degrees_per_second,
			device->x_axis_mG,
			device->y_axis_mG,
			device->z_axis_mG,
			device->bearingDegrees,
			device->pitchDegrees,
			device->rollDegrees);

}
