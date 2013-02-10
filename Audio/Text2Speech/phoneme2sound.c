/*
 * $Id: phoneme2sound.c,v 1.1 2011/07/05 23:41:33 clivewebster Exp $
 * $Log: phoneme2sound.c,v $
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
 * phoneme2sound.c
 *
 *  Created on: 31 May 2010
 *      Author: Clive Webster
 */


#include "Text2Speech.h"


// Lookup user specified pitch changes - common to all speech devices
static const uint8_t PROGMEM PitchesP[]  = { 1, 2, 4, 6, 8, 10, 13, 16 };

// Must both be the same size
static char sounds[128];
static int8_t modifier[128];

static __inline__ char getVocab(const char* vocab, size_t pos){
	return pgm_read_byte(&vocab[pos]);
}


#define isEndOfString(vocab, pos) ((getVocab(vocabEntry,pos) & 0x80)==0x80)


// Set 'numChars' to the number of characters in the source text
// that we match exactly against this phoneme dictionary entry
static size_t countMatches(const char* textp,const char* vocabEntry){
	size_t numChars;

	for(numChars=0;textp[numChars];numChars++){

		// get next input character and make lower case
		char nextChar = textp[numChars];
		if(nextChar>='A' && nextChar<='Z'){
			nextChar = nextChar - 'A' + 'a';
		}

		if(nextChar != getVocab(vocabEntry,numChars) ){
			break;
		}
	}

	if(!isEndOfString(vocabEntry,numChars)){
		numChars = 0;
	}
	return numChars;
}

/**
*
*   Convert phonemes to data string
*   Enter: textp = phonemes string
*   Return: phonemes = string of sound data
*			modifier = 2 bytes per sound data
*
*/
static boolean phonemesToData(const char* textp){

	size_t phonemeOut = 0; // offset into the phonemes array
	size_t modifierOut = 0; // offset into the modifiers array
	uint8_t L81=0; // attenuate
	uint8_t previousL81=16;

#ifdef _WINDOWS_
	memset(modifier,0xAA,sizeof(modifier));
#endif

	while(*textp){

		int numOut;

		// P20: Get next phoneme
		size_t longestMatch=0;
		const char* vocabEntry = s_phonemes;
		const char* bestPhoneme = null;
		uint8_t     bestPhonemeNo=0;


		// Get next phoneme, P2
		uint8_t phonemeNumber;

		for(phonemeNumber = 0; getVocab(vocabEntry,0);phonemeNumber++){

			// Find the exact matching characters
			size_t numChars = countMatches(textp,vocabEntry);

			// if not the longest match so far, or not a complete match, then ignore
			if(numChars > longestMatch){
				// P7: we have matched the whole phoneme
				longestMatch = numChars;
				bestPhoneme = vocabEntry + numChars;
				bestPhonemeNo = phonemeNumber;

			}

			// Move to next phoneme
			while(getVocab(vocabEntry++,0)!=0);		// Skip over sound data
		} // next phoneme

		// p13
		if(!bestPhoneme){
			loggerP(PSTR("Mistake in speech at "));
			logger(textp);
			loggerCRLF();
			return FALSE;
		}


	    L81 = pgm_read_byte(&s_attenuate[bestPhonemeNo]) /*+'0'*/;


		// Get char from text after the phoneme and test if it is a numeric
		if(textp[longestMatch]>='0' && textp[longestMatch]<='9'){
			// Pitch change requested
			modifier[modifierOut] = pgm_read_byte(&PitchesP[textp[longestMatch]-'1'] );
			modifier[modifierOut+1] = L81;
			longestMatch++;
		}else{
			modifier[modifierOut]=-1;
			modifier[modifierOut+1]=0;
		}

		// P10
		if(L81!='0' && L81 != previousL81 && modifierOut!=0 && modifier[modifierOut]>=0){
			modifier[modifierOut - 2] = modifier[modifierOut];
			modifier[modifierOut - 1] = '0';
		}else{
			// P11
			if( (textp[longestMatch-1] | 0x20) == 0x20){
				// end of input string or a space
				modifier[modifierOut] = (modifierOut==0) ? 16 : modifier[modifierOut-2];
			}
		}

		// Copy phoneme data to sound data
		for(numOut=0; getVocab(bestPhoneme,numOut)!= 0; numOut++){
			sounds[phonemeOut++] = getVocab(bestPhoneme,numOut) & 0x7f;
		}

		if(phonemeOut > sizeof(sounds)-16){
			loggerP(PSTR("Line too long\n"));
			return FALSE;
		}

		// P16

		// Copy the modifier setting to each sound data element for this phoneme
		if(numOut > 2){
			int count;
			for(count=0; count <= numOut; count+=2){
				modifier[modifierOut + count + 2] = modifier[modifierOut + count];
				modifier[modifierOut + count + 3] = modifier[modifierOut + count + 1];
			}
		}
		modifierOut += numOut;

		//p21
		textp += longestMatch;
		previousL81 = L81;
	}

	sounds[phonemeOut++]='z';
	sounds[phonemeOut++]='z';
	sounds[phonemeOut++]='z';
	sounds[phonemeOut++]='z';

	while(phonemeOut < sizeof(sounds)){
		sounds[phonemeOut++]=0;
	}

	while(modifierOut < sizeof(modifier)){
		modifier[modifierOut++]=-1;
		modifier[modifierOut++]=0;
	}

	return TRUE;
}




/*
*  Speak a string of phonemes
*/
boolean sayPhonemes(const char* textp){


	boolean rtn;


#ifdef _LOG_
	// Print the phonemes
	loggerCRLF();
	loggerP(PSTR("Phonemes:'"));
	logger(textp);
	loggerc('\'');
	loggerCRLF();
#endif


	rtn = phonemesToData(textp);

	if(rtn){
		sayNoise(sounds,modifier);
	}
	return rtn;
}

