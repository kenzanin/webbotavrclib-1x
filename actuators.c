/*
 * $Id: actuators.c,v 1.6 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: actuators.c,v $
 * Revision 1.6  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/21 19:50:31  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 18:52:38  clivewebster
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
 * actuators.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "actuators.h"


void __act_setSpeed(__ACTUATOR* act, DRIVE_SPEED speed){
	// clamp it to the given range
	speed = CLAMP(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX);

	// Get the driver class
	const __ACTUATOR_DRIVER_CLASS* driver=act->class;

	// Call the setSpeed method on the class
	if(driver){
		void (*fn)(__ACTUATOR*,DRIVE_SPEED speed) = (void (*)(__ACTUATOR*, DRIVE_SPEED speed))pgm_read_word(&driver->setSpeed);
		if(fn!=null){
			fn(act,(act->inverted) ? speed*-1 : speed);
		}
	}

	// Store the new speed - do this at the end so that the setSpeed method
	// can access the previous speed
	act->required_speed = speed;
}


void __act_setConnected(__ACTUATOR* act,boolean connected){
	// Change the variable
	act->connected = connected;

	// Get the driver class
	const __ACTUATOR_DRIVER_CLASS* driver=act->class;

	// Call the setSpeed method on the class
	if(driver){
		void (*fn)(__ACTUATOR*,boolean connected) = (void (*)(__ACTUATOR*, boolean connected))pgm_read_word(&driver->setConnected);
		if(fn!=null){
			fn(act,connected);
		}
	}
}


//void __act_AttachSpeedCallback(__ACTUATOR* act,void (*speedCallback)(__ACTUATOR *actuator, DRIVE_SPEED speed)) {
//	act->speedCallback = speedCallback;
//	__act_setSpeed(act, __act_getSpeed(act));
//}
//
//void __act_AttachConnectCallback(__ACTUATOR* act,void (*connectCallback)(__ACTUATOR *actuator, boolean connected)){
//	act->connectCallback = connectCallback;
//}
