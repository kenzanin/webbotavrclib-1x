/*
 * $Id: rprintfn.c,v 1.4 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfn.c,v $
 * Revision 1.4  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 12:13:32  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/11/02 19:02:47  clivewebster
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
 * rprintfn.c
 *
 *  Created on: 07-May-2009
 *      Author: Clive Webster
 */
#include "libdefs.h"
#include "rprintf.h"

// special printf for numbers only
// see formatting information below
//	Print the number "n" in the given "base"
//	using exactly "numDigits"
//	print +/- if signed flag "isSigned" is TRUE
//	use the character specified in "padchar" to pad extra characters
//
//	Examples:
//	uartPrintfNum(10, 6,  TRUE, ' ',   1234);  -->  " +1234"
//	uartPrintfNum(10, 6, FALSE, '0',   1234);  -->  "001234"
//	uartPrintfNum(16, 6, FALSE, '.', 0x5AA5);  -->  "..5AA5"
void rprintfNum(char base, char numDigits, char isSigned, char padchar, long n)
{
	char *p, buf[32];
	unsigned long x;
	unsigned char count;

	// prepare negative number
	if( isSigned && (n < 0) ){
		x = -n;
	}else{
	 	x = n;
	}

	// setup little string buffer
	count = (numDigits-1)-(isSigned?1:0);
  	p = buf + sizeof (buf);
  	*--p = '\0';

	// force calculation of first digit
	// (to prevent zero from not printing at all!!!)
	*--p = hexchar(x % base);
	x /= base;

	// calculate remaining digits
	while(count--){
		if(x != 0){
			// calculate next digit
			*--p = hexchar(x % base);
			x /= base;
		}else{
			// no more digits left, pad out to desired length
			*--p = padchar;
		}
	}

	// apply signed notation if requested
	if( isSigned )	{
		char s = ' ';
		if(n < 0){
   			s = '-';
		}else if(n > 0){
	   		s = '+';
		}
		*--p = s;
	}

	// print the string right-justified
	count = numDigits;
	while(count--){
		rprintfChar(*p++);
	}
}
