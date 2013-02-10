/*
 * $Id: toneRTTTL.c,v 1.1 2011/07/05 23:40:46 clivewebster Exp $
 * Revision History
 * ================
 * $Log: toneRTTTL.c,v $
 * Revision 1.1  2011/07/05 23:40:46  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.1  2010/10/09 17:19:32  clivewebster
 * *** empty log message ***
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
 *        File: toneRTTTL.c
 *  Created on: 9 Oct 2010
 *      Author: Clive Webster
 */
#include "tone.h"

#define isdigit(n) (n >= '0' && n <= '9')

static uint16_t PROGMEM notes[] = { 0,
NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};


enum{
	SKIP_NAME, GET_DEFAULTS, PLAY, FINISHED
};

static __inline__ char get(const prog_char * str){
	return pgm_read_byte(str);
}


static void nextNote(TONE_PLAYER* player){
	uint16_t num;
	const prog_char * str = player->type.rtttl.pos;

	switch(player->type.rtttl.state){

		case SKIP_NAME: {
			while(get(str) && get(str)!= ':'){
				str++;    // ignore name
			}
			if(get(str)=='\0'){
				// unexpected end of string
				player->type.rtttl.state=FINISHED;
				goto finished;
			}
			str++; 	// skip the ':'
			player->type.rtttl.state=GET_DEFAULTS;
		}
		// .. fall through to ..

		case GET_DEFAULTS:{
			player->type.rtttl.default_dur = 4;
			player->type.rtttl.default_oct = 6;
			uint16_t bpm = 63;

			// get default duration
			if(get(str) == 'd'){
				str++;				// skip d
				str++;				// skip =
			    num = 0;
			    while(isdigit(get(str))){
			      num = (num * 10) + (get(str) & 0x0f);
			      str++;
			    }
			    if(num > 0){
			    	player->type.rtttl.default_dur = num;
			    }
			    str++;                   // skip comma
			}

			// get default octave
			if(get(str) == 'o')  {
			  str++; str++;              // skip "o="
			  num = get(str++) - '0';
			  if(num >= 3 && num <=7){
				  player->type.rtttl.default_oct = num;
			  }
			  str++;                   // skip comma
			}


			// get BPM
			if(get(str) == 'b') {
			  str++; str++;            // skip "b="
			  num = 0;
			  while(isdigit(get(str))){
			    num = (num * 10) + (get(str) & 0x0f);
			    str++;
			  }
			  bpm = num;
			  str++;                   // skip colon
			}

			// BPM usually expresses the number of quarter notes per minute
			player->type.rtttl.wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

			player->type.rtttl.state=PLAY;
		}
		// .. falls through to ...

		case PLAY:{
			while( get(str) ){
				uint8_t note;
				uint8_t scale;
				uint32_t duration;

				// first, get note duration, if available
			    num = 0;
			    while(isdigit(get(str))){
			      num = (num * 10) + (get(str++) & 0x0f);
			    }

			    if(num){
			    	duration = player->type.rtttl.wholenote / num;
			    }else{
			    	duration = player->type.rtttl.wholenote / player->type.rtttl.default_dur;
			    }

			    // now get the note
			    note = 0;
			    switch(get(str++)){
			      case 'c':
			        note = 1;
			        break;
			      case 'd':
			        note = 3;
			        break;
			      case 'e':
			        note = 5;
			        break;
			      case 'f':
			        note = 6;
			        break;
			      case 'g':
			        note = 8;
			        break;
			      case 'a':
			        note = 10;
			        break;
			      case 'b':
			        note = 12;
			        break;
			      default:
			        note = 0;
			    }

			    // now, get optional '#' sharp
			    if(get(str) == '#'){
			      note++;
			      str++;
			    }

			    // now, get optional '.' dotted note
			    if(get(str) == '.'){
			      duration += duration/2;
			      str++;
			    }

			    // now, get scale
			    if(isdigit(get(str)))			    {
			      scale = get(str++) & 0x0f;
			    } else {
			      scale = player->type.rtttl.default_oct;
			    }

			    if(get(str) == ','){
			      str++;       // skip comma for next note (or we may be at the end)
			    }

			    // Play the note
		    	player->type.rtttl.pos = str;
		    	toneAttach(player,nextNote);
		    	uint16_t freq = 0;
			    if(note){
			    	freq = pgm_read_word(&notes[(scale - 4) * 12 + note]);
			    }
		    	tonePlay(player, freq ,duration);
		    	return;
			} // Process next note

			// We are at the end
			player->type.rtttl.state=FINISHED;
		}
		// .. falls through to ...

		case FINISHED:{
finished:
			toneDetach(player);
			player->type.rtttl.pos = player->type.rtttl.start = null;
			break;
		}
	}
}


void tonePlayRTTTL(TONE_PLAYER* player, const prog_char str[]){
	toneDetach(player);			// Detach any call back handler
	toneStop(player);			// Stop any current note
	player->type.rtttl.start = str;
	player->type.rtttl.pos = str;
	player->type.rtttl.state = SKIP_NAME;
	nextNote(player);
}

