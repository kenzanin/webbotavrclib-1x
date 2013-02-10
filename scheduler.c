/*
 * $Id: scheduler.c,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: scheduler.c,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/12/29 12:36:52  clivewebster
 * Stop recursive calls into scheduler
 *
 * Revision 1.4  2009/11/02 19:03:54  clivewebster
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
 * scheduler.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "scheduler.h"

#include "timer.h"

// Is the scheduler execution loop running
volatile boolean __running; //=FALSE;

volatile boolean __recheck; //=FALSE;

volatile int __numJobs;

#define CHANNEL_NUM 1


// Calculate the required compare threshold to cause an interrupt at the required time
static uint16_t calcTicks(TICK_COUNT us){
	TICK_COUNT ticks = (us * cpu_speed_div_1000000) / timerGetPrescaler(g_heartbeat);
	uint16_t top = timerGetTOP(g_heartbeat);
	uint16_t rtn = MIN(ticks,top);
//	uint16_t rtn;
//	if(timerIs16bit(g_heartbeat)){
//		rtn = (ticks > 0xFFFFUL) ? 0xFFFFU : ticks;
//	}else{
//		rtn = (ticks > 0xFFUL) ? 0xFFU : ticks;
//	}
	return rtn;
}


// schedule a new job
// callback Is the function to be run at a later date
// data is a block of data to be passed into the callback
// start (in us) Is the start time of the delay
// delay (in us) Is the amount to delay by
void scheduleJob(SchedulerCallback callback, SchedulerData data, TICK_COUNT start, TICK_COUNT delay){
	boolean doItNow = FALSE;

 	if(delay < 1000U){
		// it needs to happen now as the delay is < the heartbeat timer interrupt of 1ms
		if(__running){
			// make sure we do another loop of __scheduleUpdate to find it
			__recheck=TRUE;
		}else{
			// scheduler is dormant so just do it now
			doItNow = TRUE;
		}
	}

	if(!doItNow){
		int slot;
		// queue it up
		boolean found=FALSE;
		CRITICAL_SECTION_START;
			for(slot=0; slot<maxJobs; slot++){
				JOB* job = &__queue[slot];
				if(!job->used){
					job->used = TRUE;
					job->callback = callback;
					job->data = data;
					job->start=start;
					job->delay=delay;
					found=TRUE;
					__numJobs++;
					break;
				}
			}
		CRITICAL_SECTION_END; // end atomic

		if(!found){
			// the queue is exhausted
			setError(SCHEDULER_EXHAUSTED);
			doItNow = TRUE;
		}
	}



	if(doItNow){
		// we need to do it now
		TICK_COUNT overflow;

		// wait for expiry
		while(!clockHasElapsedGetOverflow(start, delay, &overflow)){
		}

		// callback
		callback(data,start+delay,overflow);
	}else{
		if(!__running){
			const TimerCompare* channel = timerGetCompare(g_heartbeat,CHANNEL_NUM);
			__scheduleUpdate(channel, null);
		}
	}
}

// called under compare interrupts when there is something in the queu
void __scheduleUpdate(const TimerCompare *channel, void* data){
	// Dont call me again
	compareDetach(channel);

	// Turn interrupts back on
	INTERRUPTABLE_SECTION_START;
	__running = TRUE;
	TICK_COUNT lowest;
	do{
		int slot;
		__recheck = FALSE;
		JOB* job;
		lowest=0;
		for(slot=maxJobs-1, job=&__queue[slot]; slot>=0; slot--, job--){
			if(job->used){
				// check if time has elapsed
				TICK_COUNT overflow;	// how many us the timer has overshot when it should have happened
				TICK_COUNT start=job->start;
				TICK_COUNT delay=job->delay;
				if(clockHasElapsedGetOverflow(start, delay, &overflow)){
					SchedulerCallback callback = job->callback;
					SchedulerData data = job->data;

					// Mark this job as unused. No more references shoud be made to job->xxxx
					job->used = FALSE;
					--__numJobs;

					// Run the job with interrupts enabled
					callback(data,start+delay,overflow);


					// Force another loop as the time taken may mean
					// something else can now run
					__recheck = TRUE;
				}else{
				   // overflow has the remaining number of microseconds to wait
				   if(lowest==0 || overflow < lowest){
					   lowest = overflow;
				   }
				}
			}
		}
	}while( __recheck);
	__running=FALSE;

	if(__numJobs > 0){
		uint16_t compare = calcTicks(lowest);
		uint16_t top = timerGetTOP(g_heartbeat);
		compare += timerGetCounter(g_heartbeat);
		compare %= top;
        compareAttach(channel,&__scheduleUpdate,compare,null);
	}

	// Restore previous interrupt enable
	INTERRUPTABLE_SECTION_END;
}
