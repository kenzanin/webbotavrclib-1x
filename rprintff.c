/*
 * $Id: rprintff.c,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintff.c,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
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
 * rprintff.c
 *
 *  implements rprintf float
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "rprintf.h"
#include "libdefs.h"
#include <stdarg.h>

// floating-point print
void rprintfFloat(char numDigits, double x)
{
	unsigned char firstplace = FALSE;
	unsigned char negative;
	unsigned char i, digit;
	double place = 1.0;

	// save sign
	negative = (x<0);
	// convert to absolute value
	x = (x>0)?(x):(-x);

	// find starting digit place
	for(i=0; i<15; i++)
	{
		if((x/place) < 10.0)
			break;
		else
			place *= 10.0;
	}
	// print polarity character
	if(negative)
		rprintfChar('-');
	else
		rprintfChar('+');

	// print digits
	for(i=0; i<numDigits; i++)
	{
		digit = (x/place);

		if(digit | firstplace | (place == 1.0))
		{
			firstplace = TRUE;
			rprintfChar(digit+0x30);
		}
		else
			rprintfChar(' ');

		if(place == 1.0)
		{
			rprintfChar('.');
		}

		x -= (digit*place);
		place /= 10.0;
	}
}

