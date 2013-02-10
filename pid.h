/*
 * $Id: pid.h,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: pid.h,v $
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/03/20 02:44:00  clivewebster
 * Added
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
 * pid.h
 *
 *  Created on: 01-Nov-2009
 *      Author: Clive Webster
 *  Thanks to Bill Porter for implementation
 *
 *  A generic PID controller based on pseudo code from Wikipedia:
 *  http://en.wikipedia.org/wiki/PID_controller
 *
    previous_error = 0
	integral = 0
	start:
		error = setpoint - actual_position
		integral = integral + (error*dt)
		derivative = (error - previous_error)/dt
		output = (Kp*error) + (Ki*integral) + (Kd*derivative)
		previous_error = error
		wait(dt)
		goto start
 *
 */

#ifndef PID_H_
#define PID_H_
#include "libdefs.h"

typedef struct s_pid {
	float	kP;				// proportional scale factor
	float	kI;				// integral scale factor
	float	kD;				// derivative scale factor
	float   inSpan;         //the span of input for a circular PID
	float   outMax;			//max PID output
	float   outMin;			//min PID output
	float   iLimit;			//limit to avoid wind-up
	boolean isCircular;		//is this a circular system, like a compass or cont rotation actuator?
	float   integral;		// The sum of the integral component
	float	setPoint;		// The desired output position
	float	prevError;		// The previous error amount
	TICK_COUNT lastTime;	// The last time I was called by setActual
} PID;

// Construct a new PID controller
#define MAKE_PID(kP,kI,kD,il,outMin,outMax) \
	{kP,kI,kD,  			/* Constant multipliers 		*/		\
	(float)0.0,				/* The input span for circular	*/		\
	MAX(outMin,outMax), 	/* The max output				*/		\
	MIN(outMin,outMax), 	/* The min output				*/		\
	il, 					/* The integral limit			*/		\
	FALSE, 					/* Not circular					*/		\
	(float)(0.0),(float)(0.0),(float)(0.0),\
	(TICK_COUNT)0 }

// Construct a new circular PID controller
#define MAKE_CIRCULAR_PID(kP,kI,kD,il,outMin,outMax,inMin,inMax) \
	{kP,kI,kD,  			/* Constant multipliers 		*/		\
	ABS(inMax - inMin),		/* The input span for circular	*/		\
	MAX(outMin,outMax), 	/* The max output				*/		\
	MIN(outMin,outMax), 	/* The min output				*/		\
	il, 					/* The integral limit			*/		\
	TRUE, 					/* It is circular				*/      \
	(float)(0.0),(float)(0.0),(float)(0.0),\
	(TICK_COUNT)0 }

// Update the PID with the desired target position - the setPoint
void pidSetTarget(PID* pid,float setPoint);

// Update the PID with the actual position and return the new output
float pidSetActual(PID* pid,float actual);

#endif /* PID_H_ */
