/*
 * $Id: tone.h,v 1.1 2011/07/05 23:40:46 clivewebster Exp $
 * Revision History
 * ================
 * $Log: tone.h,v $
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
 *        File: tone.h
 *  Created on: 8 Oct 2010
 *      Author: Clive Webster
 *
 *  Library for playing tones
 *  This uses a timer in CTC mode where OCRA is used to set the value of top
 *  You need to add a resistor of say 1k in series with the speaker so as not to blow the output pin
 *
 */

#ifndef TONE_H_
#define TONE_H_

#include "../libdefs.h"
#include "../iopin.h"

// Define the frequencies of various musical notes
// NOTE_<note><S=sharp><octave>
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978




typedef struct s_tone_player {
	const Timer* timer;			// The timer to use
	const IOPin* pin;			// The pin to output the pulses to
	// -- work variables --
	volatile uint32_t duration;	// number of pulses remaining
	boolean initialised;
	void*	callback;
	volatile boolean playing;
	// -- implementation specific --
	union{
		struct{
			const prog_char * start;
			const prog_char * volatile pos;
			volatile uint8_t state;
			uint32_t wholenote;
			uint8_t default_dur;
			uint8_t default_oct;
		} rtttl;
	} type;
} TONE_PLAYER;
#define MAKE_TONE_PLAYER( timer, pin ) {timer,pin, 0, FALSE, NULL, FALSE}

// Define a callback for when a tone is finished
typedef void (*ToneCallback)(TONE_PLAYER *player);


// Initialise the tone library
void toneInit(TONE_PLAYER* player);

// Start playing a note
void tonePlay(TONE_PLAYER* player, uint16_t frequency, uint32_t durationMS );

// Stop playing a tone
void toneStop(TONE_PLAYER* player);

// Are we currently playing a tone
static __inline__ boolean toneIsPlaying(const TONE_PLAYER* player){
	return player->playing;
}

// Attach a callback for when the current note has stopped playing
static __inline__ void toneAttach(TONE_PLAYER* player, ToneCallback callback){
	player->callback = callback;
}
static __inline__ void toneDetach(TONE_PLAYER* player){
	player->callback = null;
}

// Support Ring Tone Text Transfer Language RTTTL
// Play a tune from program memory
void tonePlayRTTTL(TONE_PLAYER* player, const prog_char str[]);

#endif /* TONE_H_ */
