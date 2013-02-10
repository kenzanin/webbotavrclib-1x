/*
 * $Id: speech2phoneme.c,v 1.1 2011/07/05 23:41:33 clivewebster Exp $
 * $Log: speech2phoneme.c,v $
 * Revision 1.1  2011/07/05 23:41:33  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.8  2010/06/14 19:18:25  clivewebster
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
 * speech2phoneme.c
 *
 *  Created on: 30 May 2010
 *      Author: Clive Webster
 */


#include "Text2Speech.h"
#include "../../rprintf.h"

static __inline__ char getVocab(const char* vocab, size_t pos){
	return pgm_read_byte(&vocab[pos]);
}


static boolean whitespace(char c){
	return (c==0 || c==' ' || c==',' || c=='.' || c=='?' || c=='\''
		|| c=='!' || c==':' || c=='/') ? TRUE : FALSE;
}



/**
*
*  Find the single character 'token' in 'vocab'
*  and append its phonemes to the output
*
*/

static void copyToken(char token){
	const char* vocabEntry = s_vocab;

	int y=0;
	while(getVocab(vocabEntry,y)){
		if(getVocab(vocabEntry,y) == token  && getVocab(vocabEntry,y+1) & 0x80){
			while(getVocab(vocabEntry,++y)){
				rprintfChar((char)(getVocab(vocabEntry,y) & 0x7f));
			}
			break;
		}
		while(getVocab(vocabEntry,y++)); // Move to end of phoneme
	}

}

/**
*  Enter:
*  src => English text
*  return null if errors, else phoneme string
*/
static void text2Phonemes(const char * src){
//	int outIndex = 0;// Current offset into phonemes
	int inIndex = -1; // Starts at -1 so that a leading space is assumed

	while(inIndex==-1 || src[inIndex]){	// until end of text
		int maxMatch=0;	// Max chars matched on input text
//		int numOut=0;	// Number of characters copied to output stream for the best match
		int maxWildcardPos = 0;

		// Start with first vocab entry
		const char* vocabEntry = s_vocab;

		// Keep track of best match so far
		const char* bestEntry = null;
		int  bestWildCardInPos=0;
		char bestWildCard=0;
		boolean bestHasWhiteSpace=FALSE;
		int wildcardInPos;

		// Get next phoneme, P2
		while(getVocab(vocabEntry,0)){
			int y;
			char wildcard=0; 		// The wildcard character
			boolean hasWhiteSpace=FALSE;
			wildcardInPos=0;	// The index in the vocab where it occurs

			for(y=0;;y++){
				char nextCharIn,nextVocabChar;
					
				// Get next char from user input
				// Make next char upper case and remove control characters
				nextCharIn = (y + inIndex == -1) ? ' ' : src[y + inIndex];
				if(nextCharIn>='a' && nextCharIn<='z'){
					nextCharIn = nextCharIn - 'a' + 'A';
				}else if(nextCharIn<' '){
					nextCharIn = ' ';
				}

				// Get next text char from vocab
				nextVocabChar = getVocab(vocabEntry,y);
				if( (nextVocabChar & 0x80)){
					nextVocabChar = 0;
				}



				// If its a wildcard then save its value and position
				if(nextVocabChar=='#' && nextCharIn >= 'A' && nextCharIn <= 'Z'){
					wildcard = nextCharIn; // The character equivalent to the '#'
					wildcardInPos=y;
					continue;
				}

				// Check if vocab is looking for end of word
				if(nextVocabChar=='_'){
					// try to match against a white space
					hasWhiteSpace=TRUE;
					if(whitespace(nextCharIn)){
						continue;
					}
					y--;
					break;
				}

				// check for end of either string
				if(nextVocabChar==0 || nextCharIn==0){
					break;
				}

				if(nextVocabChar != nextCharIn){
					break;
				}
			}

			// See if its the longest complete match so far
			if(y > maxMatch && ( getVocab(vocabEntry,y) & 0x80) == 0x80){
				// This is the longest complete match
				maxMatch = y;
				maxWildcardPos = 0;

				// Point to the start of the phoneme
				bestEntry = vocabEntry + y;
				bestWildCardInPos = wildcardInPos;
				bestWildCard = wildcard;
				bestHasWhiteSpace = hasWhiteSpace;
			}

			// Move to start of next entry
			while(getVocab(vocabEntry,y++)); // Move to end of phoneme asciiz
			vocabEntry += y;
		}// check next phoneme

		// 15 - end of vocab table

		//16
		if(bestHasWhiteSpace==TRUE){
			maxMatch--;
		}

		//17
		if(maxMatch==0){
			loggerP(PSTR("No token for "));
			logger(&src[inIndex]);
			loggerCRLF();
			return;
		}

		// Copy data for best match
		{
			int y;

			// Copy the matching phrase changing any '#' to the phoneme for the wildcard
			for(y=0;;y++){
				char c = getVocab(bestEntry,y) & 0x7f;	// Get the next phoneme character
				if(c==0){
					y++; 							// move to start of next vocab entry
					break;
				}

				if(c=='#'){
					if(getVocab(bestEntry,y+1)==0){
						// replacement ends in wild card
						maxWildcardPos = bestWildCardInPos;
					}else{
						// Copy the phonemes for the wild card character
						copyToken(bestWildCard);
					}
				}else{
					rprintfChar(c);	// output the phoneme character

				}
			}

		}

		inIndex += (maxWildcardPos>0) ? maxWildcardPos : maxMatch;
	}
}

void sayText(const char * src){
	Writer old;

#ifdef _LOG_
	loggerCRLF();
	loggerP(PSTR("Say:'"));					// Say: '
	logger(src);							//   ... the text ....
	loggerc('\'');							// '
	loggerCRLF();
#endif
	
	old = rprintfInit(&phonemeWriter);		// Push the current writer
	text2Phonemes(src);						// Write the current phonemes
	rprintfChar(0);							// Flush out the current text
	rprintfInit(old);						// Restore the previous writer


}

