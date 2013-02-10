/*
 * $Id: _stepper_common.h,v 1.3 2011/07/05 23:55:21 clivewebster Exp $
 * Revision History
 * ================
 * $Log: _stepper_common.h,v $
 * Revision 1.3  2011/07/05 23:55:21  clivewebster
 * STEPPER_LIST now requires PROGMEM
 *
 * Revision 1.2  2010/08/11 17:19:52  clivewebster
 * Version 1.24
 *
 * Revision 1.1  2010/08/10 22:51:58  clivewebster
 * Created
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
 *        File: _stepper_common.h
 *  Created on: 4 Aug 2010
 *      Author: Clive Webster
 */

#ifndef _STEPPER_COMMON_H_
#define _STEPPER_COMMON_H_

#include "../actuators.h"

// Define forward references
struct c_stepper_impl;

// The generic stepper implementation of the actuator calls
extern __ACTUATOR_DRIVER_CLASS c_stepper_actuator;


// A generic stepper motor
typedef struct s_stepper {
	__ACTUATOR actuator;			// common to all actuators

	const struct c_stepper_impl* class;// The driver class that controls this stepper motor
	boolean		continuous:1;		// keep moving continuously
	uint16_t	stepsPerRevolution;	// the number of steps for 360 degrees
	int16_t		stepsRemaining;		// If continuous=FALSE then how many steps are left
	int16_t		stepCount;			// Keep adding actualSpeed to stepCount until >= DRIVE_SPEED_MAX or <0
	DRIVE_SPEED	actualSpeed;		// The current speed - being moved towards 'requiredSpeed' based on acceleration
	volatile uint16_t position;	// The total number of steps the motor has turned 0..stepsPerRevolution-1
	DRIVE_SPEED accelerationStep;	// Accelerate by this amount ...
	uint16_t	accelerateEvery;	// ... every tick where a tick is the stepFrequency
} STEPPER_MOTOR;

#define MAKE_STEPPER_MOTOR(implement,inverted,steps,accel,every){ \
	MAKE_ACTUATOR_WITH_CLASS(&c_stepper_actuator, inverted), /* actuator */ \
	implement, /* The implementation of STEPPER_DRIVER_CLASS*/  \
	FALSE, /* continuous */ \
	steps, /* stepsPerRevolution */ \
	0, /* stepsRemaining */\
	0, /* stepCount */   \
	0, /* actualSpeed */ \
	0, /* position */    \
	accel,\
	every}


// Macro to convert an actual stepper motor into a generic one
#define STEPPER(motor) ( &((motor)->stepper) )

// An array of STEPPER_MOTORs stored in flash memory
// STEPPER_LIST PROGMEM list[] = { STEPPER(&motor1), STEPPER(&motor2) }
//
typedef STEPPER_MOTOR* /*PROGMEM*/  STEPPER_LIST;

typedef struct s_stepper_driver {
	STEPPER_LIST* const motors;		// The motors
	uint8_t 	num_motors; 		// The total number of motors
	uint16_t	stepFrequency;		// The maximum number of steps per second
	uint16_t	actualFrquency;		// The actual frequency of steps per second
	uint16_t	tickNumber;			// The tick number
} STEPPER_DRIVER;

#define MAKE_STEPPER_DRIVER(motorlst, freq) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(STEPPER_MOTOR*)), \
	freq,0,0 \
}

/*----------------------------------------------------------------------------------------
* Define the api for all stepper motors
-----------------------------------------------------------------------------------------*/
typedef struct PROGMEM c_stepper_impl{
	void	(*init)(STEPPER_MOTOR* motor);
	void 	(*setSpeed)(STEPPER_MOTOR* motor, DRIVE_SPEED speed);
	void 	(*setConnected)(STEPPER_MOTOR* motor, boolean connected);
	void	(*step)(STEPPER_MOTOR* motor);	// move by one step
} STEPPER_MOTOR_IMPL;

#define MAKE_STEPPER_MOTOR_IMPL(init,speed,connect,step)  {init, speed, connect,step}

/*----------------------------------------------------------------------------------------
* Define methods common to all stepper motors
-----------------------------------------------------------------------------------------*/


static __inline__ DRIVE_SPEED __stepper_getActualSpeed(const STEPPER_MOTOR* motor){
	return motor->actualSpeed;
}

static __inline__ uint16_t __stepper_getPosition(const STEPPER_MOTOR* motor){
	uint16_t rtn;
	CRITICAL_SECTION{
		rtn = motor->position;
	}
	return rtn;
}

static __inline__ int16_t __stepper_getStepsRemaining(const STEPPER_MOTOR* motor){
	uint16_t rtn;
	CRITICAL_SECTION{
		rtn = motor->stepsRemaining;
	}
	return rtn;
}

// Move the stepper through a given number of steps
void __stepper_move(STEPPER_MOTOR* motor, int16_t steps);

void stepper_init(STEPPER_DRIVER* driver, const Timer* timer);

/*----------------------------------------------------------------------------------------
* Define macros to make them accessible by derived classes
-----------------------------------------------------------------------------------------*/
// Actuator methods
#define stepper_setSpeed(p,v)     act_setSpeed(STEPPER(p),v)
#define stepper_getSpeed(p) 	  act_getSpeed(STEPPER(p))
#define stepper_setConnected(p,v) act_setConnected(STEPPER(p),v)
#define stepper_isConnected(p)    act_isConnected(STEPPER(p))
#define stepper_isInverted(p)     act_isInverted(STEPPER(p))
// Stepper specific
#define stepper_getActualSpeed(p) 	__stepper_getActualSpeed(STEPPER(p))
#define stepper_getPosition(p)  	__stepper_getPosition(STEPPER(p))
#define stepper_getStepsRemaining(p) __stepper_getStepsRemaining(STEPPER(p))
#define stepper_move(p,v)		  	__stepper_move(STEPPER(p),v)


#endif /* _STEPPER_COMMON_H_ */
