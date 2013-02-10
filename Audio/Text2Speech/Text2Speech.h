/*
 * $Id: Text2Speech.h,v 1.1 2011/07/05 23:41:33 clivewebster Exp $
 * $Log: Text2Speech.h,v $
 * Revision 1.1  2011/07/05 23:41:33  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.9  2010/06/14 19:18:25  clivewebster
 * Add copyright license info
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
 *
 * Text2Speech.h
 *
 *  Created on: 30 May 2010
 *      Author: Clive Webster
 */

#ifndef TEXT2SPEECH_H_
#define TEXT2SPEECH_H_

//#define _LOG_
#include "../../libdefs.h"

#ifdef _WINDOWS_
	#if !defined(_LOG_)
	#define _LOG_
	#endif

	// Compiling for windows
	#ifdef _LOG_
		#include <stdio.h>
		#define loggerP(s) logger(s)
		void setLogFile(FILE * file);
	#endif
#else
	#include "../../pwm.h"
#endif


#if !defined(_WINDOWS_)
// Initialise the speech system
void speechInit(const IOPin* pin);
#endif


// Speak some English Text
void sayText(const char * src);

// Speak some Phonemes
boolean sayPhonemes(const char * src);

// Set the default pitch
void setPitch(uint8_t pitch);

// Set the default pitch
uint8_t getPitch(void);

// A writer to speak English text
MAKE_WRITER(speechWriter);

// Get the writer
static __inline__ Writer getSpeechWriter(void){
	return &speechWriter;
}

// A writer to speak English phonemes
MAKE_WRITER(phonemeWriter);


#if defined(BUILDING_LIBRARY) || defined (_WINDOWS_)
extern const char PROGMEM s_vocab[];		// Text to phoneme vocab
extern const char PROGMEM s_phonemes[];		// Phoneme to sound vocab
extern const char PROGMEM s_attenuate[];
void sayNoise(const char* sounds,const int8_t* modifier);

	// Define logging routines
	#if defined(_WINDOWS_)
		// Log to a file under windows
		extern FILE * log_dest;
		#define logger(s) fprintf(log_dest,"%s",s)
		extern void loggerc(char c); 
		extern void loggerSnd(char c); 
		extern void loggerDelay(unsigned long delay);
		#define logger_uint8(n) fprintf(log_dest,"%03d",n)
		#define loggerCRLF() fprintf(log_dest,"\r\n")
		typedef uint8_t (*Writer)(uint8_t byte);

	#else
		// Log to rprintf for a micro processor
		#include "../../rprintf.h"
		#define loggerP(s) rprintfProgStr(s)
		#define logger(s)  rprintfStr((char *)s)
		#define loggerc(c) rprintfChar(c)
		#define loggerSnd(c) rprintfChar(c)
		#define logger_uint8(n) rprintfNum(10, 3, 0, ' ', n)
		#define loggerCRLF() rprintfCRLF()
	#endif
#endif

#endif /* TEXT2SPEECH_H_ */
