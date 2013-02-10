/*
 * $Id: rprintfStrLen.c,v 1.3 2010/10/15 12:17:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfStrLen.c,v $
 * Revision 1.3  2010/10/15 12:17:04  clivewebster
 * Change rprintfStrLen to use size_t
 *
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/01/24 12:13:32  clivewebster
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
 * rprintfStrLen.c
 *
 *  Created on: 21-Jan-2010
 *      Author: Clive Webster
 */
#include "rprintf.h"


// prints a section of a string stored in RAM
// begins printing at position indicated by <start>
// prints number of characters indicated by <len>
void rprintfStrLen(char str[], size_t start, size_t len)
{
	if(str){
		register size_t i=0;

		// spin through characters up to requested start
		// keep going as long as there's no null
		while( (i++<start) && (*str) ){
			str++;
		}

		// then print exactly len characters
		while(len--)
		{
			// print data out of the string as long as we haven't reached a null yet
			// at the null, start printing spaces
			rprintfChar( (*str) ? *str++ : ' ');
		}
	}
}
