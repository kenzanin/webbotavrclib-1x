
/*
 * $Id: pid.c,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: pid.c,v $
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
 * pid.c
 *
 *  Created on: 20 Mar 2010
 *      Author: Clive Webster
 */

#include "pid.h"
#include "timer.h"

// Update the PID with the desired target position - the setPoint
void pidSetTarget(PID* pid,float setPoint){
	CRITICAL_SECTION_START;					//Just in case PID is used in interrupts
	pid->setPoint = setPoint; 				//set goal
	CRITICAL_SECTION_END;
}

// Update the PID with the actual position and return the new output
float pidSetActual(PID* pid,float actual){

	// Find time difference since last call
	TICK_COUNT dt;

	if(g_heartbeat){
		// We have got a timer
		TICK_COUNT now = clockGetus();
	 	dt = now - pid->lastTime;
		pid->lastTime = now;
	}else{
		// There is no timer - so assume called every 10ms
		dt = (TICK_COUNT)10000;
	}
	
	float target;
	CRITICAL_SECTION{
		target = pid->setPoint;
	}
	float error =  target - actual;  		//find error

	//if circular control system, check for and handle wrap around
	if (pid->isCircular){
		float spanDiv2 = pid->inSpan / 2;
		if (error < -spanDiv2){
			error += pid->inSpan;
		}
  		if (error > spanDiv2){
  			error -= pid->inSpan;
  		}
	}
	pid->prevError = error;				// save error for next go around

	//sum the errors for integral and clamp to limit
	pid->integral += error;
	pid->integral = CLAMP( pid->integral, -(pid->iLimit), pid->iLimit);


	//find slope of error over time
	float derivative = (error - pid->prevError) / dt;

	float out = (pid->kP * error) + (pid->kI * pid->integral) + (pid->kD * derivative);

	//compare output to limits
	out = CLAMP(out, pid->outMin, pid->outMax);



	return out;

}



