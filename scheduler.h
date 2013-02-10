/*
 * $Id: scheduler.h,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: scheduler.h,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 19:03:54  clivewebster
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
#ifndef _SCHEDULER_H_
/* --------------------------------------------------------------
	Automatically included by timer.h

	This module implements a scheduling mechanism whereby
	a method can be called asynchronously at a later date.

	This uses the heartbeat 1ms timer so any new entry that
	requires a delay of < 1ms will actually just wait in
	the foreground. Otherwise it will add the method to the
	scheduler.

	You need to specify the maximum number of scheduled jobs
	that the scheduler can cope with at one time. The default is 4.
	But you can change this in your own code prior to including
	the system file. eg
		#define SCHEDULER_MAX_JOBS 24	// allow 24 jobs
		#include "sys/axon.h"			// Define the axon system file
	Each job takes up some Flash RAM space so don't
	just go wild with the value.

	If you add a job and there is no space for it then it will be
	ignored but it will attempt to use 'setError' to indicate that
	there is an error and then delay it in the foreground



-------------------------------------------------------------- */
#define _SCHEDULER_H_

#include "errors.h"


// Define a data type for the callback data
typedef void* SchedulerData;

// Define a data type for the callback routine
typedef void (*SchedulerCallback)(SchedulerData data, TICK_COUNT lastTime, TICK_COUNT overflow);



// Define the structure to hold one job
typedef struct s_job{
	boolean				used;		// is this slot in use?
	SchedulerCallback	callback;	// The routine to callback
	volatile SchedulerData		data;// The data for the callback
	TICK_COUNT			delay;		// The delay required in us
	TICK_COUNT			start;		// The start of the delay
} JOB;

#if !defined(BUILDING_LIBRARY)
// Maximum number of jobs
#ifndef SCHEDULER_MAX_JOBS
#define SCHEDULER_MAX_JOBS 4
#endif
uint8_t maxJobs = SCHEDULER_MAX_JOBS;
// Create the queue of jobs
JOB	__queue[SCHEDULER_MAX_JOBS];
#else
extern JOB __queue[];
extern uint8_t maxJobs;
#endif



// schedule a new job
void scheduleJob(SchedulerCallback callback, SchedulerData data, TICK_COUNT start, TICK_COUNT delay);

// Called every 1ms to schedule tasks - don't call it directly
void __scheduleUpdate(const TimerCompare* channel, void* data);

#endif


