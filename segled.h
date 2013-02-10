/*
 * $Id: segled.h,v 1.10 2011/07/06 00:31:03 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: segled.h,v $
 * Revision 1.10  2011/07/06 00:31:03  clivewebster
 * Add PROGMEM to SEGLED_LIST
 *
 * Revision 1.9  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/02/18 00:41:23  clivewebster
 * Add in-built writer
 *
 * Revision 1.7  2010/01/25 18:58:51  clivewebster
 * Add blink to marquee
 *
 * Revision 1.6  2010/01/24 17:47:52  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2010/01/17 02:42:28  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/29 12:39:02  clivewebster
 * Make active flag volatile
 *
 * Revision 1.3  2009/12/28 19:20:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/12/28 07:53:07  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2009/12/27 18:23:01  clivewebster
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
 *
 * segled.h
 *
 * Support for an eight segment led made up of:
 *
 * 				---A---
 * 				|     |
 * 				F     B
 * 				|     |
 * 				---G---
 * 				|     |
 * 				E     C
 * 				|     |
 * 				---D---  H
 *
 *
 *
 *  Created on: 27-Dec-2009
 *      Author: Clive Webster
 */

#ifndef SEGLED_H_
#define SEGLED_H_

#include "iopin.h"

// Define the segment numbers
typedef enum e_segled {
	SEGMENT_A = 0,
	SEGMENT_B,
	SEGMENT_C,
	SEGMENT_D,
	SEGMENT_E,
	SEGMENT_F,
	SEGMENT_G,
	SEGMENT_H
} SEGLED_SEGMENT;

typedef struct s_segled {
	const IOPin* segment[8];
	boolean activeHigh:1;
	boolean initialised:1;
} SEGLED;

#define MAKE_SEGLED(a,b,c,d,e,f,g,h,activehigh) {{a,b,c,d,e,f,g,h},activehigh,FALSE}


// Write a character to the display
uint8_t segled_put_char(SEGLED* led, uint8_t ch);

static __inline__ void segled_init(SEGLED* led){
	segled_put_char(led,' ');
}

// Turn a particular segment on (TRUE) or off (FALSE)
void segled_set(SEGLED* led, SEGLED_SEGMENT segment, boolean value);
#define segled_on(led,segment) segled_set(led,segment,TRUE);
#define segled_off(led,segment) segled_set(led,segment,FALSE);

// A list of segment LEDs (1 or more) used to scroll a message
typedef SEGLED* /*PROGMEM*/ SEGLED_LIST;

// The maximum line length for the marquee
// If you need a bigger value then define it in your program before including this file
#ifndef MARQUEE_SIZE
#  define MARQUEE_SIZE 40
#endif

typedef struct s_marquee{
	SEGLED_LIST* const leds;// An array of LEDs to scroll the msg across
	size_t 		num_leds;	// How many LEDs are in the array
	char*		txt;		// The text to scroll
	size_t		readPos;	// The current offset into the text to read text from
	size_t		writePos;	// The current offset into the text to write to
	size_t		txtSize;	// The max length of the text buffer
	TICK_COUNT	delayChar;	// The delay between characters
	TICK_COUNT	delayEnd;	// The delay between repeats or 0 for no-repeat
	volatile boolean active:1;	// Is there an active message?
	volatile boolean blink:1;	// Are we writing chars or spaces?
	Writer		writer;		// The routine to write a character to the display
} MARQUEE;
#define MAKE_MARQUEE(ledlst,delayChar,delayEnd,writer) { \
	ledlst, \
	(size_t)(sizeof(ledlst)/sizeof(SEGLED*)), \
	null, 0, 0, MARQUEE_SIZE, delayChar, delayEnd, FALSE, FALSE,writer \
}

// Return a Writer that can be used to write to the device
static __inline__ Writer marqueeGetWriter(const MARQUEE* marquee){
	return marquee->writer;
}

// Stop an existing marquee from repeating
void marqueeStop(MARQUEE* marquee);

// Is there an existing message being scrolled
boolean marqueeIsActive(const MARQUEE * marquee);

void marqueeSetCharDelay(MARQUEE* marquee,TICK_COUNT delay);

void marqueeSetEndDelay(MARQUEE* marquee,TICK_COUNT delay);

uint8_t marqueeSendByte(MARQUEE* marquee, uint8_t data);

#endif /* SEGLED_H_ */

