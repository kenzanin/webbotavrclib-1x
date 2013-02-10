

#ifndef _TIMER_H_
#define _TIMER_H_


/* ======================================================================================
 * $Id: timer.h,v 1.12 2010/09/08 18:28:34 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timer.h,v $
 * Revision 1.12  2010/09/08 18:28:34  clivewebster
 * Added TIMER_SNAPSHOT
 *
 * Revision 1.11  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.10  2010/02/21 19:53:23  clivewebster
 * Make timerGetMode into an inline function rather than querying the timer hardware. Much faster.
 *
 * Revision 1.9  2010/01/24 14:36:03  clivewebster
 * *** empty log message ***
 *
 * Revision 1.8  2009/11/16 03:33:46  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/10/27 20:54:22  clivewebster
 * Use shared null compare interrupt routine
 *
 * Revision 1.6  2009/10/26 18:59:58  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/10/09 17:40:11  clivewebster
 * *** empty log message ***
 *
 * ===========
 *
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
	timer.h


====================================================================================== */


#ifdef TIMER_H
#  error  You must not include any of the timerXX.h functions from AVRlib
#endif

//#ifndef THE_SYSTEM
//#  error  "Dont include this file from your own code. You should include one of the sys/xxxx.h files instead."
//#endif

#include "libdefs.h"
#include "errors.h"
#include "core.h"


extern const Timer* g_heartbeat;
extern const PROGMEM Timer PROGMEM pgm_Timers[];
extern const uint8_t NUMBER_OF_TIMERS;
#define TIMER_PRESCALE_MASK		0x07	// Timer Prescaler Bit-Mask
#define MAX_PRESCALE 1024				// The largest value for prescalers

// -- Define some fixed speeds for timerCalcPWM --
// Use the fastest possible PWM frequency
#define PWM_FASTEST 0xFFFFFFFF
// Use the slowest possible PWM frequency
#define PWM_SLOWEST 0


/* ---------------- Define various delay routines -------------------------*/

/**
    Delay loop using an 8-bit counter __count, so up to 256
    iterations are possible.  (The value 256 would have to be passed
    as 0.)  The loop executes three CPU cycles per iteration, not
    including the overhead the compiler needs to setup the counter
    register.

    Thus, at a CPU speed of 1 MHz, delays of up to 768 microseconds
    can be achieved.
*/
static __inline__ void _delay_loop_1(uint8_t __count){
	__asm__ volatile (
		"1: dec %0" "\n\t"
		"brne 1b"
		: "=r" (__count)
		: "0" (__count)
	);
}

/**

    Delay loop using a 16-bit counter __count, so up to 65536
    iterations are possible.  (The value 65536 would have to be
    passed as 0.)  The loop executes four CPU cycles per iteration,
    not including the overhead the compiler requires to setup the
    counter register pair.

    Thus, at a CPU speed of 1 MHz, delays of up to about 262.1
    milliseconds can be achieved.
 */
static __inline__ void _delay_loop_2(uint16_t __count){
	__asm__ volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (__count)
		: "0" (__count)
	);
}


/*
    Delay for a given number of milliseconds
*/
void delay_ms(uint32_t __ms);


/*
	Delay for a given number of microseconds
*/
void delay_us(uint32_t __us);

/*
	Delay for a given number of processor cycles
*/
void delay_cycles(uint32_t __cycles);

/*
 *   Find how many ticks for the given number
 *   of milliseconds with a given timer prescale value
 */
uint32_t ticks_per_ms(uint32_t ms, uint16_t prescale);

#if !defined(BUILDING_LIBRARY)
#define MS_TO_CYCLES(ms) ( (((uint64_t)ms)*F_CPU)/1000UL  )
#define US_TO_CYCLES(us) ( (((uint64_t)us)*F_CPU)/1000000UL  )
#endif
/* ----------------------- End delay routines -------------------------------------------*/


// Private methods

int __timerPrescalerIndex(const Timer* timer, uint16_t prescaler);

static __inline__ TimerData* timerGetData(const Timer* timer){
	return (TimerData*)pgm_read_word(&timer->pgm_data);
}

static __inline__ TimerDataCompare* compareGetData(const TimerCompare* compare){
	return (TimerDataCompare*)pgm_read_word(&compare->pgm_data);
}


/* -------------------------------------------------------
//
//  For a TimerCompare channel return the Timer it uses
//
------------------------------------------------------- */
static __inline__ const Timer* compareGetTimer(const TimerCompare* compare){
	return &pgm_Timers[pgm_read_byte(&compare->pgm_timer)];
}


/* -------------------------------------------------------
//
//  Does a timer support a given prescale value
//
------------------------------------------------------- */
static __inline__ boolean timerSupportsPrescaler(const Timer* timer, uint16_t prescaler){
	return (__timerPrescalerIndex(timer,prescaler) == -1) ? FALSE : TRUE;
}


/* -------------------------------------------------------
//
//  Get the prescaler value for this timer
//
//  Return: The prescaler value: 1, 8, 16, 32, 64 etc
//
//  The prescaler will divide your clock frequency by this
//  value before it increments the counter.
//  So an 8MHz clock with a prescaler of 8 results in the
//  timer being incremented at the same frequency as an
//  1MHz clock with a prescaler of 1.
------------------------------------------------------- */
static __inline__ uint16_t timerGetPrescaler(const Timer* timer){
	return timerGetData(timer)->prescale_value;
}


void timerSetPrescaler(const Timer* timer, uint16_t prescaler);

/* -----------------------------------------------
 *  Disable a timer
 -------------------------------------------------*/
static __inline__ void timerOff(const Timer* timer){
	_SFR_MEM8(pgm_read_word(&timer->pgm_prescaler)) &= ~ TIMER_PRESCALE_MASK;
}

static __inline__ boolean timerIs16bit(const Timer* timer){
	return pgm_read_byte(&timer->pgm_sixteenBit);
}

uint16_t timerGetBestPrescaler(const Timer* timer, uint16_t repeat_ms);
uint16_t timerGetClosestPrescale(const Timer* timer, uint16_t prescale);



// Read the timer counter TCNTxx
uint16_t timerGetCounter(const Timer* timer);

static __inline__ uint8_t timerNumberOfCompareUnits(const Timer* timer){
	return((uint8_t)pgm_read_byte(&timer->pgm_numCompare));
}

static __inline__ boolean timerSupportsCompare(const Timer* timer){
	return (timerNumberOfCompareUnits(timer)!=0);
}

uint16_t compareGetThreshold(const TimerCompare* channel);

void compareSetThreshold(const TimerCompare* channel, uint16_t threshold);


/* -------------------------------------------------------
//
//  Remove any pending overflow interrupt
//
------------------------------------------------------- */
static __inline__ void timerOverflowClearInterruptPending(const Timer* timer){
	__portMaskSet(&timer->pgm_overflowreq);
}

/* -------------------------------------------------------
//
//  Test if there is a pending overflow interrupt on the timer
//
------------------------------------------------------- */
static __inline__ boolean timerOverflowIsInterruptPending(const Timer* timer){
	return __portMaskGet(&timer->pgm_overflowreq);
}

/* -------------------------------------------------------
//
//  Clear a pending interrupt on a compare unit
//
------------------------------------------------------- */
static __inline__ void compareClearInterruptPending(const TimerCompare* channel){
	__portMaskSet(&channel->pgm_intpending);
}

/* -------------------------------------------------------
//
//  Test if there is a pending interrupt on a compare unit
//
------------------------------------------------------- */
static __inline__ boolean compareIsInterruptPending(const TimerCompare* channel){
	return __portMaskGet(&channel->pgm_intpending);
}

/* -------------------------------------------------------
//
//  Get the current heartbeat in microseconds
//
//  Note that this number will wrap around so a later reading
//  may give a smaller value
//  This happens every 0xffffffff or 429,4967,295 microseconds
//  ie every 429.5 seconds or every 7 minutes.
//  This means that the longest time difference you can sense
//  is about 7 minutes - this should not be a problem as you
//  will normally be using the heartbeat to measure durations
//  of less than one second.
------------------------------------------------------- */
TICK_COUNT clockGetus(void);

/* -------------------------------------------------------
//
//	Does the given timer support the given timer mode ?
//
//	return TRUE if it does, FALSE if not
//
------------------------------------------------------- */
static __inline__ boolean timerIsModeSupported(const Timer* timer, TIMER_MODE mode){
	return (pgm_read_word(&timer->pgm_modes) & BV(mode)) ? TRUE : FALSE;
}


/* -------------------------------------------------------
//
//  Get the capture pin for a timer
//  Return the IOPin or null if there isn't one
//
------------------------------------------------------- */
static __inline__ const IOPin* timerGetCapturePin(const Timer* timer){
	return (const IOPin*)(pgm_read_word(&timer->pgm_capturepin));
}


/* -------------------------------------------------------
//
//	Does the given timer support input capture ?
//
//	return TRUE if it does, FALSE if not
//
------------------------------------------------------- */
static __inline__ boolean timerIsCaptureSupported(const Timer* timer){
	return (pgm_read_byte(&timer->pgm_captureedge.mask) && timerGetCapturePin(timer)!=null) ? TRUE : FALSE;
}

/*
   returns true if the specified number of microseconds
   has passed since the start time
*/
boolean clockHasElapsed(TICK_COUNT usStart, TICK_COUNT usWait);

/*
   returns true if the specified number of microseconds
   has passed since the start time
   If true then set overflow to the number of microseconds
   that it is exceeded bt
*/
boolean clockHasElapsedGetOverflow(TICK_COUNT usStart, TICK_COUNT usWait, TICK_COUNT* overflow);

/* waits (pauses) for the specified number of microseconds */
void clockWaitus(TICK_COUNT us);

/* waits (pauses) for the specified number of milliseconds */
void clockWaitms(TICK_COUNT ms);


/* -------------------------------------------------------
//
//  Attach a callback function to a timer when it overflows
//
//  This function is called during an interrupt so should
//  be quick.
//
------------------------------------------------------- */
void timerOverflowAttach(const Timer* timer, TimerCallback callback, void* user_data );

/* -------------------------------------------------------
//
//  Detach any callback function to a timer
//
------------------------------------------------------- */
static __inline__ void timerOverflowDetach(const Timer* timer)
{
	timerOverflowAttach(timer,null,null);
}

/* -------------------------------------------------------
//
//  Attach a callback function to a timer when the capture
//  pin changes
//
//  This function is called during an interrupt so should
//  be quick.
//
------------------------------------------------------- */
void timerCaptureAttach(const Timer* timer, TimerCallback callback, void* user_data, boolean risingEdge );

/* -------------------------------------------------------
//
//  Detach any callback function to a timer
//
------------------------------------------------------- */
void timerCaptureDetach(const Timer* timer);

/* -------------------------------------------------------
//
//  Clear any pending capture interrupt
//
------------------------------------------------------- */
static __inline__ void timerCaptureClearInterruptPending(const Timer* timer){
	__portMaskSet(&timer->pgm_capturereq);
}

/* -------------------------------------------------------
//
//  Test if there is a pending capture interrupt on the timer
//
------------------------------------------------------- */
static __inline__ boolean timerCaptureIsInterruptPending(const Timer* timer){
	return __portMaskGet(&timer->pgm_capturereq);
}

boolean compareIsInUse(const TimerCompare* channel);

static __inline__ boolean timerIsInUse(const Timer* timer){
	return ((_SFR_MEM8(pgm_read_word(&timer->pgm_prescaler)) & TIMER_PRESCALE_MASK) != 0) ? TRUE : FALSE;
}

/* -------------------------------------------------------
//
//  Attach a callback function to a timer when the compare is met
//
------------------------------------------------------- */
void compareAttach(const TimerCompare* channel, TimerCompareCallback callback, uint16_t threshold, void* data );

// A TimerCompareCallback that does nothing except mark the channel as in use
void nullTimerCompareCallback(const TimerCompare *timer_compare, void* data);

/* -------------------------------------------------------
//
//  Detach any callback function from a timer
//
------------------------------------------------------- */
void compareDetach(const TimerCompare* channel);

/* -------------------------------------------------------
 *
 *   Get a given compare channel for a timer
 *
 --------------------------------------------------------*/
static __inline__ const TimerCompare* timerGetCompare(const Timer* timer, uint8_t channel){
	return ((const TimerCompare*)pgm_read_word(&timer->pgm_compares)) + channel;
}


/* -------------------------------------------------------
//
//  Re-initialise one timer.
//  Reset the timer + overflow to 0
//  Reset the prescaler to the current value
//  leave any overflow callback methods in place
//
------------------------------------------------------- */
void timerInit(const Timer* timer);

/* -------------------------------------------------------
//  Set the output mode of the channel to none, toggle,
//  non-inverting or inverting
//  This will disconnect the output OCx pins on all
//  channels and will remove any callbacks on the channels
------------------------------------------------------- */
typedef enum {
	CHANNEL_MODE_DISCONNECT=0,
	CHANNEL_MODE_TOGGLE,
	CHANNEL_MODE_NON_INVERTING,
	CHANNEL_MODE_INVERTING
} CHANNEL_MODE;

void compareSetOutputMode(const TimerCompare* channel, CHANNEL_MODE mode);
CHANNEL_MODE compareGetOutputMode(const TimerCompare* channel);
/* -------------------------------------------------------
//  Set the mode for a given timer
//  This will disconnect the output OCx pins on all
//  channels and will remove any callbacks on the channels
------------------------------------------------------- */
void timerSetMode(const Timer* timer, TIMER_MODE mode);

//TIMER_MODE timerGetMode(const Timer*timer);
static __inline__ TIMER_MODE timerGetMode(const Timer* timer){
	return timerGetData(timer)->mode;
}

#include "scheduler.h"


/* -------------------------------------------------------
//
//  Initialise all timers:
//  	Detach any overflow interrupt
//		timerInit
//
------------------------------------------------------- */
void initTimers(void);

/* -------------------------------------------------------
//
//  Get the pin which this channel uses for output
//  Return IOpin* or null if it has no header pins
//
------------------------------------------------------- */
static __inline__ const IOPin * compareGetPin(const TimerCompare* channel){
	return  (const IOPin *)pgm_read_word(&channel->pgm_pwm);
}

/* -------------------------------------------------------
//
//  Get the compare channel used by a given IOPin
//  May return null if this pin is not to do with a timer
//  or it does not exist on this system board
------------------------------------------------------- */
const TimerCompare* compareFromIOPin(const IOPin* pin);


/* -------------------------------------------------------
//
//  This is called after the app has been initialised.
//  Find a free channel to use for the heartbeat
//
------------------------------------------------------- */

void __clockInit(void);


uint16_t timerGetTOP(const Timer* timer);

boolean timerCalcPwm(const Timer* timer, uint32_t deciHertz, uint16_t steps, TIMER_MODE *modeRtn, uint16_t* icrRtn, uint16_t* prescaleRtn);
uint32_t TimerGetPwmDeciHertz(const Timer* timer);

// A snaphot of the timer - it is of no direct practical use
// other than it can be turned into a time in uS
typedef struct s_timer_snapshot{
	const Timer* timer;	// The timer
	TICK_COUNT whole;	// The overflow count
	uint16_t   part;	// The partial count
} TIMER_SNAPSHOT;

void clockGetSnapshot(TIMER_SNAPSHOT* snapshot);

TICK_COUNT timerSnapshotToTicks(const TIMER_SNAPSHOT* snapshot);

/* ----------------------------------------------------------------------------------------


	Status LED functionality.

---------------------------------------------------------------------------------------- */
#include "led.h"



void __error_flash(void * __error, TICK_COUNT lastTime, TICK_COUNT overflow);

// ISRs called from interrupts - do not call directly
void __timer_compareService(const TimerCompare* channel);
void __timer_overflowService(const Timer* timer);
void __timer_captureService(const Timer* timer);

#endif
