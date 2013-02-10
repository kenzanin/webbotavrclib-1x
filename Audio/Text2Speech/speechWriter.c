/*
 * $Id: speechWriter.c,v 1.1 2011/07/05 23:41:33 clivewebster Exp $
 * $Log: speechWriter.c,v $
 * Revision 1.1  2011/07/05 23:41:33  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.2  2010/06/14 19:18:25  clivewebster
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
 * speechWriter.c
 *
 *  Created on: 7 Jun 2010
 *      Author: Clive Webster
 *
 *  A writer that accepts English text and speaks it
 *  Text must be terminated by a '\n'
 */

#include "Text2Speech.h"
#include "../../buffer.h"

// The average longest english word
#define MAX_WORD_LENGTH 20

// The longest phrase before the text is spoken
#define MAX_PHRASE_LENGTH 60


// The current phrase of english words
static uint8_t phrase[MAX_PHRASE_LENGTH];
static cBuffer phraseBuffer = MAKE_BUFFER(phrase);

// The current word in phonemes
static uint8_t word[MAX_WORD_LENGTH];
static cBuffer wordBuffer = MAKE_BUFFER(word);


/*
 * Flush the buffer
 *
 * This will send the current contents up to and including the last space
 */
static void flushPhrase(void){

	if(phraseBuffer.datalength){
		// Write up to the last space
		phrase[phraseBuffer.datalength] = 0;
		sayText((char*)phrase);

		// Reset buffer to start
		bufferFlush(&phraseBuffer);
		phraseBuffer.dataindex=0;
	}
}


// Flush the current word buffer
// out to the phraseBuffer
static void flushWord(void){
	uint8_t data;
	size_t wordLen = wordBuffer.datalength;

	// If the phrase cannot accept the entire
	// word then say the current phrase first
	if(bufferFreeSpace(&phraseBuffer) <= wordLen){
		flushPhrase();
	}

	// Write the word to the phrase buffer
	while(bufferGet(&wordBuffer,&data)){
		bufferPut(&phraseBuffer,data);
	}
}

/**
 * Write out the text character
 *
 * byte = The data to be written
 */
MAKE_WRITER(speechWriter){

	// If end of word or word buffer is full then flush it
	if(byte<=' ' || bufferIsFull(&wordBuffer)){
		flushWord();
	}


	// If its a control character then say the phrase so far
	if(byte < ' ' ){
		flushWord();
		flushPhrase();
	}else{
		bufferPut(&wordBuffer,byte);
	}
	return byte;
}
