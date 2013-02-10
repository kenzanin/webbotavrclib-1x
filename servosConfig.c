
/*
 * $Id: servosConfig.c,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: servosConfig.c,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/05/28 18:02:57  clivewebster
 * Reset the current speed if the servo config is changed
 *
 * Revision 1.1  2010/02/06 13:26:04  clivewebster
 * Allow setServoConfig to change the settings for a servo
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
 * servosConfig.c
 *
 *  Created on: 06-Feb-2010
 *      Author: Clive Webster
 *
 * Change the config settings for a given servo
 */
#include "servos.h"
void servoSetConfig(SERVO* servo, uint16_t center, uint16_t range){
	servo->center_us = center;						// Set the new center value
	servo->range_us = range;						// Set the new range value

	SERVO_DRIVER* driver = servo->driver;			// Get the current driver
	if(driver){
		// The servo is 'live'
		CRITICAL_SECTION_START;

		// Take the useable servo range and calculate the min/max pulses
		// (SERVO_CYCLE*1000) = servo_cycle
		//        min			    x
		// x = (min * g_servo_cycle)/(SERVO_CYCLE*1000)
		uint32_t min = center - range;
		uint32_t m1 =  servo->top / 1000;
		uint32_t m2 =  m1 * min;
		uint32_t m3 =  m2 / SERVO_CYCLE;
		if( (m3 & 0xFFFF0000UL)!=0){
			// doesn't fit
			setError(SERVO_TIMING);
			m3 = 0xFFFFUL;
		}
		servo->min_ticks = m3;

		uint32_t max = center + range;
		m2 =  m1 * max;
		m3 =  m2 / SERVO_CYCLE;
		if( (m3 & 0xFFFF0000UL)!=0){
			// doesn't fit
			setError(SERVO_TIMING);
			m3 = 0xFFFFUL;
		}
		servo->max_ticks = m3;

		// Set the speed again
		if(act_isConnected(servo)){
			DRIVE_SPEED speed = servo->actuator.required_speed;
			servo->actuator.required_speed-=1;
			act_setSpeed(servo,speed);
		}
		CRITICAL_SECTION_END;
	}
}
