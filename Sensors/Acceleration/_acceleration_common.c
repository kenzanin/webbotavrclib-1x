/*
 * $Id: _acceleration_common.c,v 1.5 2010/06/14 18:50:17 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _acceleration_common.c,v $
 * Revision 1.5  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/26 21:07:07  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/11/02 18:18:28  clivewebster
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
 *
 * _acceleration_common.c
 *
 *  Created on: 25-Sep-2009
 *      Author: Clive Webster
 */
#include "_acceleration_common.h"
#include "../../core.h"

void	__accelerometerSetX(ACCELEROMETER* device, ACCEL_TYPE x){
	device->x_axis_mG = interpolate(x, device->x_calib_min,device->x_calib_max,(ACCEL_TYPE)-1000,(ACCEL_TYPE)1000);
}
void	__accelerometerSetY(ACCELEROMETER* device, ACCEL_TYPE y){
	device->y_axis_mG = interpolate(y, device->y_calib_min,device->y_calib_max,(ACCEL_TYPE)-1000,(ACCEL_TYPE)1000);
}
void	__accelerometerSetZ(ACCELEROMETER* device, ACCEL_TYPE z){
	device->z_axis_mG = interpolate(z, device->z_calib_min,device->z_calib_max,(ACCEL_TYPE)-1000,(ACCEL_TYPE)1000);
}


void __accelerometerCalibrateX(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue){
	device->x_calib_min = minValue;
	device->x_calib_max = maxValue;
}
void __accelerometerCalibrateY(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue){
	device->y_calib_min = minValue;
	device->y_calib_max = maxValue;
}
void __accelerometerCalibrateZ(ACCELEROMETER* device,ACCEL_TYPE minValue,ACCEL_TYPE maxValue){
	device->z_calib_min = minValue;
	device->z_calib_max = maxValue;
}
