/*
 * $Id: GaitRunner.h,v 1.2 2010/07/29 02:24:41 clivewebster Exp $
 * Revision History
 * ================
 * $Log: GaitRunner.h,v $
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
 *        File: GaitRunner.h
 *  Created on: 28 Jun 2010
 *      Author: Clive Webster
 *
 *  Responsible for running gaits in standalone mode
 */

#ifndef GAITRUNNER_H_
#define GAITRUNNER_H_

#include "../libdefs.h"
#include "../actuators.h"

// Define the position of one limb
typedef struct s_limbPos{
	int16_t		cubeX,cubeY;
	int16_t		squareX,squareY;
	int16_t		timeX,timeY;
	DRIVE_SPEED	startY;
} G8_LIMB_POSITION;
#define MAKE_G8_LIMB_POSITION(cubeX,cubeY,squareX,squareY, timeX, timeY, startY) {cubeX,cubeY,squareX,squareY, timeX, timeY, startY}

// Define the position of all limbs for a given frame
typedef struct s_frame{
	uint16_t	time;				// This moment in time 0...1000
	const G8_LIMB_POSITION* limbs;	// The array of limb positions
} G8_FRAME;
#define MAKE_G8_FRAME(time, limbs) { time, limbs }

// Define one animation
typedef struct s_animation{
	const uint8_t			numFrames;		// Number of frames in this animation
	const G8_FRAME* const	frames;			// The array of frames
	const boolean			sweep;			// Does it sweep back and forth?
} G8_ANIMATION;
#define MAKE_G8_ANIMATION(numFrames, frames, sweep) { numFrames, frames, sweep }

// Define the gait runner itself
typedef struct s_runner{
	ACTUATOR_LIST* const 	actuators;		// The list of actuators to control
	const uint8_t 	 		num_actuators;	// The number of actuators in the list
	const G8_ANIMATION* const 	animations;	// The address of the animations array
	volatile uint8_t 		animation;		// The current animation
	volatile uint8_t		frame;			// The current frame in the animation
	volatile int16_t		repeatCount;	// Number of loops to play (0=forever)
	volatile boolean		playing;		// Is an animation current playing?
	volatile TICK_COUNT		startTime;		// Time when the animation started
	volatile int16_t		currentTime;	// The current time offset
	volatile int16_t		totalTime;		// The total time required to play the animation
	volatile boolean		backwards;		// Are we playing the animation backwards
	volatile DRIVE_SPEED	speed;			// The speed of animation
	volatile DRIVE_SPEED*	speeds;			// The speed/position setting for each actuator
	volatile DRIVE_SPEED*	delta;			// The speed/position setting to add for each actuator
} G8_RUNNER;

#define MAKE_G8_RUNNER(list, animations) { list, \
		(uint8_t)(sizeof(list)/sizeof(__ACTUATOR*)), \
		animations, 0,0,0,false,0,0,0,false, 0, null,null };

// Initialise a gait runner from appInitHardware or appInitSoftware
void gaitRunnerInit(G8_RUNNER* runner);

// Start running a new animation
void gaitRunnerPlay(G8_RUNNER* runner, uint8_t animation, int16_t loopSpeed, DRIVE_SPEED speed, int16_t repeatCount);

// Stop the animation when it next reaches its last frame
static __inline__ void gaitRunnerStop(G8_RUNNER* runner){
	runner->repeatCount = (runner->speed < 0) ? -1 : 1;
}

// Update the gait runner and move servos to new positions
// Call it from your main loop or via the scheduler to do it in the background
// NB There is no point scheduling any faster than 20ms as that is the servo refresh rate
// Return true if an animation is playing
boolean gaitRunnerProcess(G8_RUNNER* runner);


static __inline__ boolean gaitRunnerIsPlaying(const G8_RUNNER* runner){
	return runner->playing;
}

static __inline__ int16_t gaitRunnerRepeatCount(const G8_RUNNER* runner){
	return runner->repeatCount;
}

static __inline__ void gaitRunnerSetSpeed(G8_RUNNER* runner, DRIVE_SPEED speed ){
	runner->speed = speed;
}
static __inline__ DRIVE_SPEED gaitRunnerGetSpeed(const G8_RUNNER* runner ){
	return runner->speed;
}

void gaitRunnerSetDelta(G8_RUNNER* runner, uint8_t limbNumber, DRIVE_SPEED speed );
static __inline__ DRIVE_SPEED gaitRunnerGetDelta(const G8_RUNNER* runner, uint8_t limbNumber){
	return runner->delta[limbNumber];
}

#endif /* GAITRUNNER_H_ */
