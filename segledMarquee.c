
/*
 * $Id: segledMarquee.c,v 1.9 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: segledMarquee.c,v $
 * Revision 1.9  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/02/21 19:55:56  clivewebster
 * Improve handling for packed fields
 *
 * Revision 1.7  2010/02/18 00:41:17  clivewebster
 * Add in-built writer
 *
 * Revision 1.6  2010/02/06 13:09:06  clivewebster
 * Fixed bug in blink
 *
 * Revision 1.5  2010/02/04 19:45:58  clivewebster
 * Only flash if the frame is the same as the previous frame
 *
 * Revision 1.4  2010/01/25 18:58:51  clivewebster
 * Add blink to marquee
 *
 * Revision 1.3  2010/01/24 17:47:52  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/12/29 12:38:33  clivewebster
 * Move short marquee calls into segledMarquee.c
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
 * segledMarquee.c
 *
 *  Created on: 27-Dec-2009
 *      Author: Clive Webster
 */

#include "segled.h"
#include "rprintf.h"
#include "scheduler.h"
#include "timer.h"

// For malloc
#include <stdlib.h>

// Initialise a given marquee
static void marquee_init(MARQUEE* marquee){
	if(marquee->txt == null){
		marquee->txt = malloc(1 + marquee->txtSize);
		if(marquee->txt){
			for(int i=0; i<=marquee->txtSize;i++){
				marquee->txt[i] = '\0';
			}
			marquee->readPos = marquee->writePos = 0;
			marquee->active = FALSE;
			for(size_t i = 0; i < marquee->num_leds; i++){
				SEGLED* led = (SEGLED*)pgm_read_word(&marquee->leds[i]);
				segled_put_char(led,' ');
			}
		}
	}
}

static void marqueeUpdate(SchedulerData data, TICK_COUNT lastTime, TICK_COUNT overflow){
	MARQUEE* m = (MARQUEE*)data;

	char* readPos = m->txt + m->readPos;
	char  first=*readPos;
	boolean blink = m->blink;

	TICK_COUNT delay = (first=='\0') ? m->delayEnd : m->delayChar;
	TICK_COUNT delayDiv4 = delay >> 2;

	if(blink==FALSE){
		// we are not currently bliking. Check if we should start.
		if(first!='\0'){
			char* pos = readPos;
			char prev = readPos[-1];
			blink = TRUE;
			for(size_t led = 0; led < m->num_leds; led++){
				char ch = *pos++;
				// If we've hit end of line or a different char then no blink
				if(ch=='\0' || ch!=prev){
					blink=FALSE;
					break;
				}
				prev = ch;
			}
		}
	}else{
		// We are already blinking, so turn it off
		blink=FALSE;
	}

	// Set it for next time
	if(blink != m->blink){
		m->blink = blink;
		delay = (blink) ? delayDiv4 : delay - delayDiv4;
	}

	// Write chars to leds
	for(size_t l = 0; l < m->num_leds; l++){
		SEGLED* led = (SEGLED*)pgm_read_word(&m->leds[l]);
		char ch = (blink) ? '\0' : *readPos;
		if(ch!='\0'){
			readPos++;
		}else{
			ch = ' ';
		}
		segled_put_char(led,ch);
	}

	if(first=='\0'){
		// line is finished
		m->readPos = 0;
		if(m->delayEnd==0 || m->txt[0]=='\0'){
			m->active=FALSE;
		}else{
			m->blink = FALSE;
		}
	}else{
		// middle of line
		if(!blink){
			// Show character next time
			m->readPos += 1;
		}
	}

	if(m->active){
		scheduleJob(&marqueeUpdate,data,lastTime,delay);
	}
}

uint8_t marqueeSendByte(MARQUEE* marquee, uint8_t byte){
	marquee_init(marquee);
	if(marquee->txt){

		if(byte=='\n'){
			// Start writing at the beginning of the line
			marquee->txt[marquee->writePos] = '\0';
			marquee->writePos = 0;
			CRITICAL_SECTION_START;
			if(!marquee->active){
				marquee->active = TRUE;
				marquee->blink = FALSE;
				marquee->readPos=0;
				scheduleJob(&marqueeUpdate,(SchedulerData)marquee,clockGetus(),marquee->delayChar);
			}
			CRITICAL_SECTION_END;
		}else if(byte!='\r'){
			// Now put the character to the buffer
			if(marquee->writePos < marquee->txtSize){
				char* put = marquee->txt + marquee->writePos;
				CRITICAL_SECTION_START;
				*put++ = byte;
				*put = '\0';
				marquee->writePos += 1;
				marquee->readPos = 0;
				CRITICAL_SECTION_END;
			}
		}
	}
	return byte;
}


// Stop an existing marquee
void marqueeStop(MARQUEE* marquee){
	if(marquee && marquee->txt){
		CRITICAL_SECTION_START;
		marquee->readPos = marquee->writePos = 0;
		marquee->txt[0]='\0';
		CRITICAL_SECTION_END;
	}
}

// Is there an existing message being scrolled
boolean marqueeIsActive(const MARQUEE * marquee){
	return (marquee->active) ? TRUE : FALSE;
}

void marqueeSetCharDelay(MARQUEE* marquee,TICK_COUNT delay){
	CRITICAL_SECTION_START;
	marquee->delayChar = delay;
	CRITICAL_SECTION_END;
}

void marqueeSetEndDelay(MARQUEE* marquee,TICK_COUNT delay){
	CRITICAL_SECTION_START;
	marquee->delayEnd = delay;
	CRITICAL_SECTION_END;
}
