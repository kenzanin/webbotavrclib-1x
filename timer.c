/*
 * $Id: timer.c,v 1.19 2011/08/13 11:30:56 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: timer.c,v $
 * Revision 1.19  2011/08/13 11:30:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.18  2010/09/08 18:28:34  clivewebster
 * Added TIMER_SNAPSHOT
 *
 * Revision 1.17  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.16  2010/06/07 18:34:01  clivewebster
 * Only enable compare interrupts if there is a user defined callback
 *
 * Revision 1.15  2010/02/21 19:53:23  clivewebster
 * Make timerGetMode into an inline function rather than querying the timer hardware. Much faster.
 *
 * Revision 1.14  2009/11/16 03:33:46  clivewebster
 * *** empty log message ***
 *
 * Revision 1.13  2009/11/01 20:16:55  clivewebster
 * *** empty log message ***
 *
 * Revision 1.12  2009/10/27 20:53:43  clivewebster
 * Use shared null compare interrupt routine
 *
 * Revision 1.11  2009/10/26 18:54:40  clivewebster
 * *** empty log message ***
 *
 * Revision 1.10  2009/10/10 20:15:20  clivewebster
 * *** empty log message ***
 *
 * Revision 1.9  2009/10/10 18:45:59  clivewebster
 * Try to maximise the clock overflow to the largest possible value of milliseconds - but a minimum of 12ms
 *
 * Revision 1.8  2009/10/09 17:40:11  clivewebster
 * *** empty log message ***
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
 * timer.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "timer.h"
#include "scheduler.h"
#include "rprintf.h"


// Increment clock every n 'us'
static TICK_COUNT _CLOCK_US_;
// The number of ticks for the clock
static volatile TICK_COUNT clockTicks;

// The prescale factors for the different timers
uint16_t PROGMEM TimerPrescaleFactor[] 		= {0,1,8,   64,    256,1024};
uint16_t PROGMEM TimerRTCPrescaleFactor[] 	= {0,1,8,32,64,128,256,1024};



//#include "device.h"  NO
extern const uint8_t NUMBER_OF_TIMERS;

// The timer used for the clock, or null if not set
const Timer* g_heartbeat;

uint32_t ticks_per_ms(uint32_t ms, uint16_t prescale){
	return (ms * cpu_speed_div_1000)/prescale;
}

/*
    Delay for a given number of milliseconds
*/
void delay_ms(uint32_t __ms){
	if(g_heartbeat){
		clockWaitms(__ms);
	}else{
		uint32_t cycles = ticks_per_ms(__ms, 1U);
		delay_cycles(cycles);
	}
}


/*
	Delay for a given number of microseconds
*/
void delay_us(uint32_t __us){
	if(g_heartbeat && __us > 100){
		clockWaitus(__us);
	}else{
		uint32_t cycles = ticks_per_ms(__us, 1000U);
		delay_cycles(cycles);
	}
}

/*
	Delay for a given number of processor cycles
*/
void delay_cycles(uint32_t __cycles){
	if(__cycles <= (3*256U)){
		_delay_loop_1((uint8_t) (__cycles / 3) );
		return;
	}

	uint32_t cyc = __cycles / 4;
	while(cyc != 0 ){
	   uint16_t actual = (cyc <= 65535U) ? cyc : 65535U;
	   _delay_loop_2(actual);
	   cyc -= actual;
	}
}

// Magic callback for compare matches that just mark the channel as in use
// but there is nothing to do in the interrupt routine
void nullTimerCompareCallback(const TimerCompare *timer_compare, void* data){}

// Service interrupts for the system clock
static void clockCallback(const TimerCompare *timer_compare, void* data){
	clockTicks++;
}




/* ------------------------------------------------------
 *
 *  	Get the value of TOP for a given timer
 *  	will return 0 if the timer is not in use
 *
 -------------------------------------------------------*/
uint16_t timerGetTOP(const Timer* timer){
	uint16_t rtn = 0;
	if(timerIsInUse(timer)){
		switch(timerGetMode(timer)){
		case TIMER_MODE_NORMAL:
			rtn = 0xffffU;
			break;
		case TIMER_MODE_PWM8_PHASE_CORRECT:
		case TIMER_MODE_PWM8_FAST:
			rtn = 0xffU;
			break;
		case TIMER_MODE_PWM9_PHASE_CORRECT:
		case TIMER_MODE_PWM9_FAST:
			rtn = 0x1ffU;
			break;
		case TIMER_MODE_PWM10_PHASE_CORRECT:
		case TIMER_MODE_PWM10_FAST:
			rtn = 0x3ffU;
			break;

		// The following all use channel OCRnA	for top and fires compare match interrupts instead of overflows
		case TIMER_MODE_CTC_OCR:
		case TIMER_MODE_PWM_PHASE_FREQ_OCR:
		case TIMER_MODE_PWM_PHASE_CORRECT_OCR:
		case TIMER_MODE_PWM_FAST_OCR:
			{
				const TimerCompare* tc = timerGetCompare(timer,0); // uses channel A
				rtn = compareGetThreshold(tc);
			}
			break;

		// The following all use ICRn for top and fires ICFn interrupts instead of overflows
		case TIMER_MODE_PWM_PHASE_FREQ_ICR:
		case TIMER_MODE_PWM_PHASE_CORRECT_ICR:
		case TIMER_MODE_CTC_ICR:
		case TIMER_MODE_PWM_FAST_ICR:
			{
				PORT icr = pgm_read_word(&timer->pgm_icr);
				rtn = _SFR_MEM16(icr);
			}
			break;
		case TIMER_MODE_13_RESVD:
			break;
		}// end switch
	}
	if(!timerIs16bit(timer)){
		rtn &= 0xffu;
	}
	return rtn;
}

/* -------------------------------------------------------
//
//  Does a timer support a given prescale value
//
------------------------------------------------------- */
int __timerPrescalerIndex(const Timer* timer, uint16_t prescaler){
	uint8_t count;
	uint16_t *array;
	int8_t i;

	if(pgm_read_byte(&timer->pgm_rtc)){
		array =TimerRTCPrescaleFactor;
		count = sizeof(TimerRTCPrescaleFactor) / sizeof(uint16_t);
	}else{
		array =TimerPrescaleFactor;
		count = sizeof(TimerPrescaleFactor) / sizeof(uint16_t);

	}

	for(i=count-1; i>=0; i--){
		uint16_t one = pgm_read_word(array + i);
		if(one == prescaler){
			return i;
		}
	}
	return -1;
}



void timerSetPrescaler(const Timer* timer, uint16_t prescaler){
	PORT pre = pgm_read_word(&timer->pgm_prescaler);

	CRITICAL_SECTION_START;
	int inx = __timerPrescalerIndex(timer,prescaler);
	if(inx!=-1){
		timerGetData(timer)->prescale_value = prescaler;
		_SFR_MEM8(pre) &= ~ TIMER_PRESCALE_MASK;
		_SFR_MEM8(pre) |= inx;
	}else{
		setError(TIMER_PRESCALER_UNSUPPORTED);
	}
	CRITICAL_SECTION_END;

}

// Round up to the nearest prescaler value
uint16_t timerGetClosestPrescale(const Timer* timer, uint16_t prescale){
	prescale = CLAMP(prescale, 1, MAX_PRESCALE);

	while(__timerPrescalerIndex(timer,prescale)==-1){
		prescale++;
	}
	return prescale;
}

uint16_t timerGetBestPrescaler(const Timer* timer, uint16_t repeat_ms){
	uint32_t maxval;

	// Find maximum value for the timer counter
	if(timerIs16bit(timer)){
		maxval = 0xffffU;
	}else{
		maxval = 0xffU;
	}

	uint16_t prescale;
	uint16_t available=1;
	for(prescale=1; prescale<=MAX_PRESCALE; prescale<<=1){
		int inx = __timerPrescalerIndex(timer, prescale);
		if(inx!=-1){
			available = prescale;

			if(ticks_per_ms(repeat_ms,prescale)<= maxval){
				return prescale;
			}
		}
	}
	setError(NO_APPROPRIATE_TIMER_PRESCALE_VALUES);
	return available;
}

uint16_t compareGetThreshold(const TimerCompare* channel){
	uint16_t rtn;

	const Timer* timer = compareGetTimer(channel);
	if(timerSupportsCompare(timer)){
		PORT port = (PORT)pgm_read_word(&channel->pgm_threshold);
		if(timerIs16bit(timer)){
			rtn = _SFR_MEM16(port); 					// set 16 bit word
		}else{
			rtn = _SFR_MEM8(port); 					// set low byte
		}
	}else{
		setError(TIMER_COMPARE_NOT_SUPPORTED);
		rtn = 0xff;
	}
	return rtn;
}


void compareSetThreshold(const TimerCompare* channel, uint16_t threshold){
	const Timer* timer = compareGetTimer(channel);
	if(timerSupportsCompare(timer)){
		PORT port = (PORT)pgm_read_word(&channel->pgm_threshold);
		if(timerIs16bit(timer)){
			_SFR_MEM16(port) = threshold; 					// set 16 bit word
		}else{
			if(threshold > 0xffU){
				threshold=0xffU;
				setError(TIMER_COMPARE_NOT_8_BIT);
			}
			_SFR_MEM8(port) = threshold; 				// set low byte
		}
	}else{
		setError(TIMER_COMPARE_NOT_SUPPORTED);
	}
}



uint16_t timerGetCounter(const Timer* timer){
	PORT counter = pgm_read_word(&timer->pgm_counter);
	if(timerIs16bit(timer)){
		return _SFR_MEM16(counter);
	}
	return _SFR_MEM8(counter);
}

static void __timerRead(const Timer* timer,TIMER_SNAPSHOT* snapshot){
	TIMER_MODE mode = timerGetMode(timer);

	snapshot->timer = timer;
	if(!timer){
		snapshot->part = snapshot->whole = 0;
		return;
	}

	if(mode==TIMER_MODE_CTC_OCR
			|| mode==TIMER_MODE_PWM_PHASE_FREQ_OCR
			|| mode==TIMER_MODE_PWM_PHASE_CORRECT_OCR
			|| mode==TIMER_MODE_PWM_FAST_OCR){
		// These mode all generate compare interrupts on channel A
		const TimerCompare* channel = timerGetCompare(timer,0);
		CRITICAL_SECTION_START;
		{
			uint16_t tcnt  = timerGetCounter(timer);	// get the current ticks
			snapshot->whole  = (timer==g_heartbeat) ? clockTicks : 0;
			snapshot->part   = timerGetCounter(timer);				// get the current ticks again
			// If the second reading of tcnt has gone down then there must have been an overflow
			// since reading the 'rtn' value. Or there may be a pending interrupt which may be
			// because interrupts are currently turned off. In either case increment the 'rtn' value
			// as if the interrupt has happened
			if(snapshot->part < tcnt || compareIsInterruptPending(channel)){
				snapshot->whole+=1;
				snapshot->part = timerGetCounter(timer); 		// get the current ticks again
			}
		}
		CRITICAL_SECTION_END;
	}else{
		// These modes generate overflow interrupts.
		CRITICAL_SECTION_START;
		{
			uint16_t tcnt  = timerGetCounter(timer);	// get the current ticks
			snapshot->whole = (timer==g_heartbeat) ? clockTicks : 0;
			snapshot->part = timerGetCounter(timer);				// get the current ticks again
			// If the second reading of tcnt has gone down then there must have been an overflow
			// since reading the 'rtn' value. Or there may be a pending interrupt which may be
			// because interrupts are currently turned off. In either case increment the 'rtn' value
			// as if the interrupt has happened
			if(snapshot->part < tcnt || timerOverflowIsInterruptPending(timer)){
				snapshot->whole+=1;
				snapshot->part = timerGetCounter(timer); 		// get the current ticks again
			}
		}
		CRITICAL_SECTION_END;
	}
}

TICK_COUNT timerSnapshotToTicks(const TIMER_SNAPSHOT* snapshot){
	TICK_COUNT rtn;
	TICK_COUNT top = timerGetTOP(snapshot->timer);

	if(snapshot->timer == g_heartbeat){
		rtn = snapshot->whole * _CLOCK_US_;

		// top = _CLOCK_US_
		// part     x
		TICK_COUNT frac  = snapshot->part;
		frac *= _CLOCK_US_;
		frac /= top;

		rtn += frac;

	}else{
		rtn = snapshot->whole;
		rtn*= (top + 1);
		rtn+= snapshot->part;

	}
	return rtn;
}

TICK_COUNT timerGetTicks(const Timer* timer){

	TIMER_SNAPSHOT snapshot;
	__timerRead(timer, &snapshot);
	return timerSnapshotToTicks(&snapshot);
}


void clockGetSnapshot(TIMER_SNAPSHOT* snapshot){
	__timerRead(g_heartbeat, snapshot);
}

/* -------------------------------------------------------
//
//  Get the current heartbeat in microseconds
//
//  Note that this number will wrap around so a later reading
//  may give a smaller value
//  This happens every 0xffffffff or 4,294,967,295 microseconds
//  ie every 4295 seconds or every 70 minutes.
//  This means that the longest time difference you can sense
//  is about 70 minutes - this should not be a problem as you
//  will normally be using the heartbeat to measure durations
//  of less than one second.
------------------------------------------------------- */
TICK_COUNT clockGetus(void){
	return timerGetTicks(g_heartbeat);
}


/*
   returns true if the specified number of microseconds
   has passed since the start time
*/
boolean clockHasElapsed(TICK_COUNT usStart, TICK_COUNT usWait){
	TICK_COUNT now = clockGetus();
	TICK_COUNT test = now;
	test -= usStart;
	if( test  >= usWait){
		return TRUE;
	}
	return FALSE;
}


/* waits (pauses) for the specified number of milliseconds */
void clockWaitms(TICK_COUNT ms){
	clockWaitus(ms*1000UL);
}

/* Pause for the given number of microseconds */
void clockWaitus(TICK_COUNT us){
	TICK_COUNT start = clockGetus();
	while(clockGetus() - start < us){
		nop();
	}

//	TICK_COUNT start = timerGetTicks(g_heartbeat);
//	uint16_t prescaler = timerGetPrescaler(g_heartbeat);
//	uint16_t mhz = cpu_speed_div_1000000;
//	TICK_COUNT pause = (us / prescaler) * mhz;
//	while(pause > (timerGetTicks(g_heartbeat) - start)){
//		nop();
//	}
}

/*
   returns true if the specified number of microseconds
   has passed since the start time
   If true then set overflow to the number of microseconds
   that it is exceeded bt
*/
boolean clockHasElapsedGetOverflow(TICK_COUNT usStart, TICK_COUNT usWait, TICK_COUNT* overflow){
	boolean rtn = FALSE;
	TICK_COUNT now = clockGetus();
	TICK_COUNT test = now;
	test -= usStart;			// The actual delay that has happened
	if( test  > usWait){
		TICK_COUNT ovr = test - usWait;
		*overflow = ovr;		// Return the number of microseconds we have overshot by
		rtn = TRUE;
	}else{
		TICK_COUNT ovr = usWait - test;
		*overflow = ovr;		// Return the number of microseconds remaining
	}
	return rtn;
}





/* -------------------------------------------------------
//
//  Attach a callback function to a timer when the compare is met
//
------------------------------------------------------- */
void compareAttach(const TimerCompare* channel, TimerCompareCallback callback, uint16_t threshold, void* data ){

	const Timer* timer = compareGetTimer(channel);
	if(timerSupportsCompare(timer)){

		TimerDataCompare* td = compareGetData(channel);

		// Check if compare is already in use
		if(callback!=null && td->compare_callback!=null){
			setError(TIMER_COMPARE_CALLBACK_EXISTS);
		}

		// Make sure the timer is set up and running
		timerSetPrescaler(timer,timerGetPrescaler(timer));

		// Disallow compare interrupts whilst changing
		__portMaskClear(&channel->pgm_intenable);

		// Set the compare threshold
		compareSetThreshold(channel, threshold);

		// Add the callback routine
		td->compare_callback = callback;
		td->compare_data = data;

		// Clear old interrupt pending before re-enabling interrupt
		compareClearInterruptPending(channel);

		// Allow compare interrupts - unless it is the null routine
		if(callback && callback != &nullTimerCompareCallback){
			__portMaskSet(&channel->pgm_intenable);
		}
	}else{
		setError(TIMER_COMPARE_NOT_SUPPORTED);
	}
}


/* -------------------------------------------------------
//
//  Detach any callback function from a timer
//
------------------------------------------------------- */
void compareDetach(const TimerCompare* channel)
{
	const Timer* timer = compareGetTimer(channel);
	if(timerSupportsCompare(timer)){


		// Disallow compare interrupts for this channel
		__portMaskClear(&channel->pgm_intenable);

		// Stop it from changing the output pin
		compareSetOutputMode(channel,CHANNEL_MODE_DISCONNECT);

		// remove routine
		compareGetData(channel)->compare_callback = null;
	}else{
		setError(TIMER_COMPARE_NOT_SUPPORTED);
	}
}

/* -------------------------------------------------------
//
//  Re-initialise one timer.
//  Reset the timer + overflow to 0
//  Reset the prescaler to the current value
//  leave any overflow callback methods in place
//
------------------------------------------------------- */
void timerInit(const Timer* timer){
	uint8_t i;

	// Get data from ROM to RAM
	PORT counter = pgm_read_word(&timer->pgm_counter);
	TimerData* data = timerGetData(timer);

	CRITICAL_SECTION_START;

		// Turn the timer off for now
		timerOff(timer);

		// Disallow timer overflow interrupt for now
		__portMaskClear(&timer->pgm_overflowint);

		// Clear the pre-overflow counter register
		_SFR_MEM8(counter) = 0;

		// Clear the overflow counter
//		data->overflow = 0;

		// Set up the prescaler and turn the timer back on
		timerSetPrescaler(timer,data->prescale_value);

		// Initialise the compare units
		uint8_t numCompare = timerNumberOfCompareUnits(timer);
		for(i=0;i<numCompare;i++){
			// Get the channel
			const TimerCompare* channel = timerGetCompare(timer,i);

			// Remove any interrupt pending for the channel
			compareClearInterruptPending(channel);
		}

		// Remove any interrupt pending
		timerOverflowClearInterruptPending(timer);

		// If there is an overflow callback then re-enable timer overflow interrupt
		if(data->overflow_callback!=null){
			__portMaskSet(&timer->pgm_overflowint);
		}

	CRITICAL_SECTION_END;
}
void compareSetOutputMode(const TimerCompare* channel, CHANNEL_MODE mode){
	PORT port = pgm_read_word(&channel->pgm_com.port);
	PIN  bit = pgm_read_byte(&channel->pgm_com.mask);
	mode <<= bit;

	PIN mask = BV(bit);	// occupies the next bit up as well
	mask |= (mask<<1);

	_SFR_MEM8(port) = (_SFR_MEM8(port) & ~mask) | (mode & mask);
}

CHANNEL_MODE compareGetOutputMode(const TimerCompare* channel){
	PORT port = pgm_read_word(&channel->pgm_com.port);
	PIN  bit = pgm_read_byte(&channel->pgm_com.mask);

	PIN mask = BV(bit);	// occupies the next bit up as well
	mask |= (mask<<1);

	uint8_t val = _SFR_MEM8(port) & mask;
	val >>= bit;

	return (CHANNEL_MODE)val;
}
uint8_t PROGMEM __3bit_modes[]={0,1,255,255,2,3,255,255,255,255,255,5,255,255,255,7};
uint8_t PROGMEM __2bit_modes[]={0,1,255,255,2,3};

/*
TIMER_MODE timerGetMode(const Timer*timer){
	uint16_t abilities = pgm_read_word(&timer->pgm_modes);	// Get the bitmasks of modes availables
	if(abilities==0){
		return 0;
	}

	TIMER_MODE mode=0;
	uint8_t shift=1;
	for(int i=0; i<4;i++){
		PORT    port= pgm_read_word(&timer->pgm_wgm[i].port);
		if(port==0){
			break;
		}

		PIN mask= pgm_read_byte(&timer->pgm_wgm[i].mask);
		if(_SFR_MEM8(port) & mask){
			mode|=shift;
		}
		shift<<=1;
	}

	if(abilities==TIMER_3BIT_MODES){
		// expand out to large set of mode
		for(int i=0; i<sizeof(__3bit_modes);i++){
			uint8_t aMode = pgm_read_byte(&__3bit_modes[i]);
			if(aMode==mode){
				return i;
			}
		}
	}else if(abilities==TIMER_2BIT_MODES){
		// expand out to large set of mode
		for(int i=0; i<sizeof(__2bit_modes);i++){
			uint8_t aMode = pgm_read_byte(&__2bit_modes[i]);
			if(aMode==mode){
				return i;
			}
		}
	}
	return mode;
}
*/

/* -------------------------------------------------------
//  Set the mode for a given timer
//  This will disconnect the output OCx pins on all
//  channels and will remove any callbacks on the channels
------------------------------------------------------- */
void timerSetMode(const Timer* timer, TIMER_MODE mode){
	if(mode == TIMER_MODE_NORMAL || timerGetMode(timer)!=mode){
		uint16_t abilities = pgm_read_word(&timer->pgm_modes);	// Get the bitmasks of modes availables
		uint8_t wgm=255;

		if(BV(mode) & abilities){
			// It is capable of the mode

			// Save the new mode
			timerGetData(timer)->mode = mode;

			if(abilities == TIMER_ALL_MODES){
				wgm = mode;
			}else if(abilities == TIMER_3BIT_MODES){
				// Timer has the small subset of modes
				wgm = pgm_read_byte(&__3bit_modes[mode]);
			}else if(abilities == TIMER_2BIT_MODES){
				// Timer has the small subset of modes
				wgm = pgm_read_byte(&__2bit_modes[mode]);
			}
			if(wgm==255){
				// Mode is not supported
				setError(TIMER_MODE_NOT_SUPPORTED);
			}else{
				timerOff(timer);	// turn timer off whilst changing the mode

				const PORT_MASK* pm = &timer->pgm_wgm[0];
				for(int i=0; i<4;i++,pm++){
					PORT port= pgm_read_word(pm->port);

					if(port==0){
						break;
					}

					if(wgm & 1){
						__portMaskSet(pm);
					}else{
						__portMaskClear(pm);
					}
					wgm>>=1;
				}

				// For each channel - disconnect the output pins and remove any callback
				uint8_t numCompare = timerNumberOfCompareUnits(timer);
				int8_t i;
				for(i=numCompare-1; i>=0;i--){
					const TimerCompare* channel = timerGetCompare(timer,i);
// is done in compareDetach
//					compareSetOutputMode(channel,CHANNEL_MODE_DISCONNECT);
					compareDetach(channel);
				}

				// re-initialise the timer and its channels
				timerInit(timer);

			}
		}else{
			// Timer is not able to do this mode
			setError(TIMER_MODE_NOT_SUPPORTED);
		}
	}
}

/* -------------------------------------------------------
//
//  Initialise all timers:
//  	Detach any overflow interrupt
//		timerInit
//
------------------------------------------------------- */
void initTimers(void){
	uint8_t t;
    cli();
    for(t=0; t < NUMBER_OF_TIMERS; t++){
		const Timer * timer = &pgm_Timers[t];
		timerOverflowDetach(timer);						// Detach any overyflow call back
		timerCaptureDetach(timer);						// Detach any capture call back
		timerSetMode(timer,TIMER_MODE_NORMAL);
		timerOff(timer);						// initialise the timer
	}
    sei();
}

/* -------------------------------------------------------
//
//  An overflow interrupt has been called for a given
//  timer. Do not call directly.
//
------------------------------------------------------- */
void __timer_overflowService(const Timer* timer){
	TimerData* td = timerGetData(timer);

// 	td->overflow++;	// increment the overflow counter
	if(td->overflow_callback){
		td->overflow_callback(timer,td->overflow_data);
	}
}

/* -------------------------------------------------------
//
//  A capture interrupt has been called for a given
//  timer. Do not call directly.
//
------------------------------------------------------- */
void __timer_captureService(const Timer* timer){
	TimerData* td = timerGetData(timer);

	if(td->capture_callback){
		td->capture_callback(timer,td->capture_data);
	}
}

/* -------------------------------------------------------
//
//  A compare interrupt has been called for a given
//  timer. Do not call directly.
//
------------------------------------------------------- */
void __timer_compareService(const TimerCompare* channel){
	TimerDataCompare* td = compareGetData(channel);
//	td->overflow++;
	if(td->compare_callback){
		td->compare_callback(channel,td->compare_data);
	}
}

/* -------------------------------------------------------
//
//  This is called after the app has been initialised.
//  Find a free channel to use for the heartbeat
//
------------------------------------------------------- */
ERROR __error; // All zeros so exclude -  = {0,0,FALSE,null};

void __clockInit(void){

	const Timer* bestTimer;
	uint16_t     bestCompare;
	uint16_t	 bestPrescaler;

	g_heartbeat = null;
	uint16_t	 clock_ms=12;		// 12ms work on 8 bit timer, 20MHz ie the worst case

	bestTimer=null;
	bestCompare = 0;
	bestPrescaler = 0;

	// find a free timer that can support the required mode with lowest prescaler
	// Since this mode will always use channel A to set the value of top then the timer must be completely unused
    for(int8_t t=NUMBER_OF_TIMERS-1; t>=0 ; t--){
		const Timer * timer = &pgm_Timers[t];
		if(!timerIsInUse(timer) && timerIsModeSupported(timer,TIMER_MODE_CTC_OCR)
				&& timerNumberOfCompareUnits(timer) >= 2 // Need one for timer and one for scheduler
				){
			if(bestCompare>0xFFU && !timerIs16bit(timer)){
				continue;
			}
			// It is a candidate
			uint16_t thePreScale = timerGetBestPrescaler(timer, clock_ms); // At least 16ms before overflow
			uint32_t compare = ticks_per_ms(clock_ms,thePreScale);		// Get the value for top

			// See if it is the best so far - and if so then remember it
			if(bestTimer==null || compare > bestCompare){
				bestTimer = timer;
				bestCompare = compare;
				bestPrescaler = thePreScale;
			}
		}
    }

    if(bestTimer!=null){
    	// The largest possible value for top
    	TICK_COUNT max = (timerIs16bit(bestTimer)) ? 0xFFFFUL : 0xFFUL;
recalc:
    	if(bestCompare + ticks_per_ms(1,bestPrescaler) + 1 < max){
        	// See if we can boost the number of 'ms'
    		uint16_t thePreScale = timerGetBestPrescaler(bestTimer, clock_ms+1); // Try another ms
    		uint32_t compare = ticks_per_ms(clock_ms+1,thePreScale);			 // Get the value for top
    		if(thePreScale==bestPrescaler && compare <= max && compare>bestCompare){
    			// Same prescaler and increased compare
    			clock_ms++;
    			bestCompare = compare;
    			goto recalc;
    		}
    	}

		// We have got the highest setting
    	g_heartbeat = bestTimer;
    	_CLOCK_US_ = clock_ms * 1000UL;
        timerSetPrescaler(g_heartbeat, bestPrescaler);


		// Change the mode of the timer
		timerSetMode(g_heartbeat, TIMER_MODE_CTC_OCR);
		const TimerCompare* channel = timerGetCompare(g_heartbeat, 0);
		compareAttach(channel, &clockCallback, bestCompare, null);

		// Enable overflow interrupts - NOT NEEDED - uses compare interrupts
		//__portMaskSet(&g_heartbeat->pgm_overflowint);
	}

	// if an error was previously set then start it flashing on the LED
	ERROR_CODE current = __error.errorCode;
	if(current!=0){
		__error.errorCode = 0;
		setError(current);
	}

}

ERROR_CODE getError(void){
	 return __error.errorCode;
}

void __error_flash(void * __error, TICK_COUNT lastTime, TICK_COUNT overflow){
	ERROR* err = (ERROR*) __error;

	if(err->remaining==0){
		err->remaining = ABS(err->errorCode);
	}

	TICK_COUNT delay = (err->errorCode <0 ) ? (TICK_COUNT)250000UL : (TICK_COUNT)500000UL;
	if(err->phase){
		// turn led off
		statusLED_off();
		err->remaining--;
		if(err->remaining==0){
			delay = (TICK_COUNT)2000000UL;
		}

		err->phase=FALSE;
	}else{
		// turn led on
		statusLED_on();
		err->phase=TRUE;

	}



/*
	if(overflow > delay){
	  delay = 0U;
	}else{
		delay -= overflow;	// remove any overflow
	}
*/

	// queue the next event
	scheduleJob(&__error_flash, __error, lastTime, delay);

}

void setError(ERROR_CODE err){
	if(__error.errorCode==0){
		__error.errorCode = err;
		// Start to flash the error code if we can
		if(statusLED.pin != null && g_heartbeat != null){
			scheduleJob(&__error_flash, &__error, clockGetus(),0);
		}

		// log to any rprintf writer
		Writer writer = __error.output;
		if(writer){
			Writer old = rprintfInit(writer);
			if(err < 0 ){
				rprintf("WebbotLib Error:%d\n",(int)-err);
			}else{
				rprintf("User Error:%d\n",(int)err);
			}
			rprintfInit(old);
		}

	}
}

void setErrorLog(Writer log){
	__error.output = log;
}

