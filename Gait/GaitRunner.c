/*
 * $Id: GaitRunner.c,v 1.2 2010/07/29 02:24:41 clivewebster Exp $
 * Revision History
 * ================
 * $Log: GaitRunner.c,v $
 * Revision 1.2  2010/07/29 02:24:41  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/07/03 15:27:28  clivewebster
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
 *        File: GaitRunner.c
 *  Created on: 30 Jun 2010
 *      Author: Clive Webster
 *
 *  Functions to execute a gait created using Gait Designer. This will execute
 *  the gait without requiring any connection to the PC
 *
 */
#include "GaitRunner.h"
#include "../timer.h"
#include <stdlib.h>

//#define DEBUG

#ifdef DEBUG
#include "../rprintf.h"
#endif

// The scaling factor used in GaitDesigner when generating the gait
#define SCALE_X 1000

// Initialise a gait runner from appInitHardware or appInitSoftware
void gaitRunnerInit(G8_RUNNER* runner){
	if(runner->speeds == null){
		runner->speeds = malloc(runner->num_actuators * sizeof(DRIVE_SPEED));
		runner->delta = malloc(runner->num_actuators * sizeof(DRIVE_SPEED));
		for(uint8_t i=0; i<runner->num_actuators; i++){
			runner->speeds[i] = runner->delta[i] = 0;
		}
	}
}

// Start running a new animation
void gaitRunnerPlay(G8_RUNNER* runner, uint8_t animation, int16_t loopSpeed, DRIVE_SPEED speed, int16_t repeatCount){
	// Update variables with interrupts off - in case the gait is
	// updated under interrupts
	TICK_COUNT now = clockGetus();
	CRITICAL_SECTION {
		runner->animation = animation;
		runner->repeatCount = repeatCount;
		runner->frame = 0;
		runner->playing = TRUE;
		runner->startTime =  now;
		runner->currentTime = (speed<0) ? loopSpeed : 0;
		runner->totalTime = loopSpeed;
		runner->speed = speed;
		runner->backwards = FALSE;
	}
	// Set servos to initial position
	gaitRunnerProcess(runner);
}

static uint16_t calcX(const G8_LIMB_POSITION* limb, double t1){
	int16_t a = (int)pgm_read_word(&limb->cubeX);
	int16_t b = (int)pgm_read_word(&limb->squareX);
	int16_t c = (int)pgm_read_word(&limb->timeX);
	double t2 = t1 * t1;
	double t3 = t2 * t1;
	uint16_t nx = (uint16_t) ( (t3 * a) + (t2 * b) + (t1 * c) );
	return nx;
}

static DRIVE_SPEED calcY(const G8_LIMB_POSITION* limb, double t1){
	int16_t a = (int)pgm_read_word(&limb->cubeY);
	int16_t b = (int)pgm_read_word(&limb->squareY);
	int16_t c = (int)pgm_read_word(&limb->timeY);
	DRIVE_SPEED d = (DRIVE_SPEED)pgm_read_byte(&limb->startY);
	double t2 = t1 * t1;
	double t3 = t2 * t1;
	DRIVE_SPEED ny = (DRIVE_SPEED) ( (t3 * a) + (t2 * b) + (t1 * c) + d );
	return ny;
}


// Update the gait runner and move servos to new positions
// Call it from your main loop or via the scheduler to do it in the background
// NB There is no point scheduling any faster than 20ms as that is the servo refresh rate
// Return true if an animation is playing
boolean gaitRunnerProcess(G8_RUNNER* runner){

	if(!gaitRunnerIsPlaying(runner) || runner->speeds==null){
		return FALSE;
	}

	TICK_COUNT  now = clockGetus();
	int16_t  interval = (now - runner->startTime)>>16;
	if(interval == 0){
		return TRUE;
	}

	// There has been a noticeable change in time
	runner->startTime = now;
	if(runner->backwards){
		interval *= -1;
	}
	interval *= runner->speed;

	// Re-check as drive speed could be zero
	if(interval == 0){
		return TRUE;
	}

	// Locate the current animation
	const G8_ANIMATION* animation = &runner->animations[runner->animation];

	// Update the current time with the new interval
	int16_t currentTime = runner->currentTime + interval;
	if(currentTime >= runner->totalTime){
		// We have finished playing the animation
		if(pgm_read_byte(&animation->sweep)==FALSE){
			currentTime %= runner->totalTime;			// Set back to start of loop
			if(runner->repeatCount){
				runner->repeatCount -= 1;				// One less frame to go
				if(runner->repeatCount==0){
					runner->playing = FALSE;			// we have reached the end
					currentTime = 0;					// set servos to final position
				}
			}
		}else{
			// Start going backwards through the animation
			currentTime = runner->totalTime - (currentTime - runner->totalTime);
			runner->backwards = TRUE;
		}
	}else if(currentTime < 0){
		// We have moved before the start
		if(pgm_read_byte(&animation->sweep)==FALSE){
			currentTime = runner->totalTime + currentTime;
			if(runner->repeatCount){
				runner->repeatCount += 1;				// One more frame to go
				if(runner->repeatCount==0){
					runner->playing = FALSE;			// we have reached the end
					currentTime = 0;					// set servos to start position
				}
			}
		}else{
			// We have completed a sweep
			runner->backwards = FALSE;
			currentTime = -currentTime;

			if(runner->repeatCount){
				runner->repeatCount -= 1;			// One less frame to go
				if(runner->repeatCount==0){
					runner->playing = FALSE;		// we have reached the end
					currentTime = 0;				// set servos to initial position
				}
			}
		}
	}
	runner->currentTime = currentTime; // range is 0....totalTime

	// Current time in the range 0...SCALE_X
	uint16_t frameTime = interpolateU(currentTime, 0,runner->totalTime, 0, SCALE_X);
	uint16_t frameStartTime = 0;
	uint16_t frameEndTime = SCALE_X;

	// Locate the correct frame
	const G8_FRAME* frame = (const G8_FRAME*)pgm_read_word(&animation->frames);
	uint8_t i;
	for(i = pgm_read_byte(&animation->numFrames)-1; i>0; i--){
		const G8_FRAME* f = &frame[i];
		frameStartTime = pgm_read_word(&f->time);
		if(frameStartTime <= frameTime){
			frame = f;
			break;
		}
		frameEndTime = frameStartTime;
		frameStartTime = 0;
	}
	runner->frame = i;

#ifdef DEBUG
rprintf("\n%u,%d",i,currentTime);
#endif

	// Now have:- frameStartTime <= frameTime <= frameEndTime

	// We now need to find the distance along the curve (0...1) that represents
	// the x value = frameTime

	// First guess from 0..1
	uint16_t frameTimeOffset = frameTime-frameStartTime;
	double distanceGuess = ((double)(frameTimeOffset)) / ((double)(frameEndTime-frameStartTime));

	const G8_LIMB_POSITION* limb = (const G8_LIMB_POSITION*)pgm_read_word(&frame->limbs);
	for(uint16_t l = 0; l < runner->num_actuators; l++, limb++){
		double distanceMin = 0.0;
		double distanceMax = 1.0;
		double distance = distanceGuess;

		// Find the correct distance along the line for the required frameTime
		for(uint8_t iterations=0; iterations<20; iterations++){
			uint16_t actualX = calcX(limb, distance);
			if(actualX == frameTimeOffset) break;	// Found it

			if( actualX < frameTimeOffset){
				// We need to increase t
				distanceMin = distance;
			}else{
				distanceMax = distance;
			}

			// Next guess is half way between
			distance = distanceMin + ((distanceMax - distanceMin) / 2);
		}

		// We now know the distance
		runner->speeds[l] = calcY(limb,distance);

#ifdef DEBUG
rprintf(",%d",speed);
#endif
	}	// next limb

#ifndef DEBUG
	// Set all the servo speeds in quick succession
	for(uint16_t l = 0; l < runner->num_actuators; l++){
		__ACTUATOR* servo = (__ACTUATOR*)pgm_read_word(&runner->actuators[l]);
		int16_t speed = (int16_t)(runner->speeds[l]) + (int16_t)(runner->delta[l]);
		speed = CLAMP(speed,DRIVE_SPEED_MIN,DRIVE_SPEED_MAX);
		__act_setSpeed(servo,(DRIVE_SPEED)speed);
	}
#endif

	return gaitRunnerIsPlaying(runner);
}

void gaitRunnerSetDelta(G8_RUNNER* runner, uint8_t limbNumber, DRIVE_SPEED speed ){
	if(limbNumber < runner->num_actuators){
		runner->delta[limbNumber] = speed;
		if(!gaitRunnerIsPlaying(runner)){
			// Send the output now
			__ACTUATOR* servo = (__ACTUATOR*)pgm_read_word(&runner->actuators[limbNumber]);
			int16_t speed = (int16_t)(runner->speeds[limbNumber]) + (int16_t)(runner->delta[limbNumber]);
			speed = CLAMP(speed,DRIVE_SPEED_MIN,DRIVE_SPEED_MAX);
			__act_setSpeed(servo,(DRIVE_SPEED)speed);
		}
	}
}



