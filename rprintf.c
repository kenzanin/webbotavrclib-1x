/*
 * $Id: rprintf.c,v 1.10 2011/07/09 16:44:31 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintf.c,v $
 * Revision 1.10  2011/07/09 16:44:31  clivewebster
 * rputchar is volatile and changed in CRITICAL_SECTION
 *
 * Revision 1.9  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/01/24 12:13:32  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/11/02 19:02:47  clivewebster
 * Added revision log
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
 */
#include "libdefs.h"
#include "rprintf.h"


// Lookup for hex characters
const char PROGMEM HexChars[] = "0123456789ABCDEF";


MAKE_WRITER(rprintfIgnore){
	return byte;
}

// function pointer to single character output routine
//uint8_t (*rputchar)(uint8_t c) = &rprintfIgnore;
static volatile Writer rputchar = &rprintfIgnore;

// you must call this function once and supply the character output
// routine before using other functions in this library
Writer rprintfInit(Writer putchar_func){
	Writer rtn;
	CRITICAL_SECTION{
		rtn = rputchar;
		rputchar = putchar_func;
	}
	return rtn;
}


// send a character/byte to the current output device
void rprintfChar(uint8_t c)
{
	// do LF -> CR/LF translation
	if(c == '\n')
		rputchar('\r');
	// send character
	rputchar(c);
}

void rprintfCharN(uint8_t c, size_t repeat){
	while(repeat-- > 0){
		rprintfChar(c);
	}
}


// prints a null-terminated string stored in RAM
void rprintfStr(char str[])
{
	// send a string stored in RAM
	if(str){
		// print the string until a null-terminator
		while (*str)
			rprintfChar(*str++);
	}
}


// prints a null-terminated string stored in program ROM
void rprintfProgStr(const prog_char str[])
{
	if(str){
		// print a string stored in program memory
		register char c;

		// print the string until the null-terminator
		while((c = pgm_read_byte(str++)))
			rprintfChar(c);
	}
}

// prints carriage return and line feed
void rprintfCRLF(void)
{
	// print CR/LF
	//rprintfChar('\r');
	// LF -> CR/LF translation built-in to rprintfChar()
	rprintfChar('\n');
}

// prints an unsigned 4-bit number in hex (1 digit)
void rprintfu04(uint8_t data)
{
	// print 4-bit hex value
	rprintfChar(hexchar(data));
}
