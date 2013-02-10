/*
 * $Id: actuators.h,v 1.9 2011/07/06 00:21:02 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: actuators.h,v $
 * Revision 1.9  2011/07/06 00:21:02  clivewebster
 * Add PROGMEM to ACTUATOR_LIST
 *
 * Revision 1.8  2010/08/11 17:19:02  clivewebster
 * Added MAKE_ACTUATOR_WITH_CLASS
 *
 * Revision 1.7  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/04/25 03:31:49  clivewebster
 * Add ACTUATOR_LIST
 *
 * Revision 1.5  2010/02/21 19:51:24  clivewebster
 * Change getSpeed, isConnected and isInverted into inline functions
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
 *
 */
#ifndef _ACTUATORS_H_
#define _ACTUATORS_H_

/* ---------------------------------------------------------------


	Define various data types for controlling actuators
	such as servos and DC motor controllers


   ---------------------------------------------------------------*/

#include "libdefs.h"
#include "uart.h"

// A datatype for setting speeds between
// DRIVE_SPEED_MIN (full reverse) and DRIVE_SPEED_MAX (full forward) where 0 = brake/stop
typedef int8_t	DRIVE_SPEED;

#define DRIVE_SPEED_BRAKE ((DRIVE_SPEED)0)
#define DRIVE_SPEED_CENTER DRIVE_SPEED_BRAKE
#define DRIVE_SPEED_MIN   ((DRIVE_SPEED)-127)
#define DRIVE_SPEED_MAX ((DRIVE_SPEED) 127)


// Define forward references
struct c_actuator_driver;

/*----------------------------------------------------------------------------------------
* An abstract datatype used by all of the different actuators.
* The only time when it is used by itself is a 'virtual' actuator where the device is
* connected to a 3rd party driver board - where we only talk to the controller board
* rather than to each actuator directly
-----------------------------------------------------------------------------------------*/

typedef struct s_actuator_common{
	const struct c_actuator_driver* class;// The driver class that controls this actuator
	DRIVE_SPEED	  required_speed;	// The last required speed that has been set. Note that the motor might not yet be at that speed. See encoders.
	boolean		  connected:1;		// When disconnected a device will freewheel, as opposed to speed=0 which will make it brake
	const boolean inverted:1;		// If TRUE then will reverse the direction of the motor ie it will use 'required_speed * -1'. So if your motor is
									// turning the wrong way then just flip this flag
} __ACTUATOR;

// Define the standard constructor for an actuator
// Start with a speed of -128 so that the next setSpeed is a change
#define MAKE_ACTUATOR_WITH_CLASS(class,inverted)  {class, -128 ,FALSE, inverted}
#define MAKE_ACTUATOR(inverted)  MAKE_ACTUATOR_WITH_CLASS(null,inverted)

typedef __ACTUATOR* /*PROGMEM*/  ACTUATOR_LIST;


/*----------------------------------------------------------------------------------------
* Define the api for all actuator drivers ie they can: be initialised, change speed,
* and their connected state
-----------------------------------------------------------------------------------------*/
typedef struct PROGMEM c_actuator_driver{
	void 	(*setSpeed)(__ACTUATOR*, DRIVE_SPEED speed);
	void 	(*setConnected)(__ACTUATOR*, boolean connected);
} __ACTUATOR_DRIVER_CLASS;

#define MAKE_ACTUATOR_DRIVER_CLASS(speed,connect)  {speed, connect}



/*----------------------------------------------------------------------------------------
* Define methods common to all actuators
-----------------------------------------------------------------------------------------*/

void __act_setSpeed(__ACTUATOR* act, DRIVE_SPEED speed);

static __inline__ DRIVE_SPEED __act_getSpeed(const __ACTUATOR* act){
	return act->required_speed;
}

void __act_setConnected(__ACTUATOR* act,boolean connected);

static __inline__ boolean __act_isConnected(const __ACTUATOR* act){
	return (act->connected) ? TRUE : FALSE;
}

static __inline__ boolean __act_isInverted(const __ACTUATOR* act){
	return (act->inverted) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------
* Define macros to make them accessible by derived classes
-----------------------------------------------------------------------------------------*/

#define act_setSpeed(p,v)     __act_setSpeed(&(p)->actuator,v)
#define act_getSpeed(p) 	  __act_getSpeed(&(p)->actuator)
#define act_setConnected(p,v) __act_setConnected(&(p)->actuator,v)
#define act_isConnected(p)    __act_isConnected(&(p)->actuator)
#define act_isInverted(p)     __act_isInverted(&(p)->actuator)


#endif
