/*
 * $Id: rprintfh.c,v 1.5 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfh.c,v $
 * Revision 1.5  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 12:13:32  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/11/02 19:02:47  clivewebster
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
 * rprintfh.c
 *
 *  Created on: 07-May-2009
 *      Author: Clive Webster
 *
 *  Print numbers in hex
 */
#include "libdefs.h"
#include "rprintf.h"


// prints an unsigned 8-bit number in hex (2 digits)
void rprintfu08(uint8_t data)
{
	// print 8-bit hex value
	rprintfu04(data>>4);
	rprintfu04(data);
}

// prints an unsigned 16-bit number in hex (4 digits)
void rprintfu16(uint16_t data)
{
	// print 16-bit hex value
	rprintfu08(data>>8);
	rprintfu08(data);
}

// prints an unsigned 32-bit number in hex (8 digits)
void rprintfu32(uint32_t data)
{
	// print 32-bit hex value
	rprintfu16(data>>16);
	rprintfu16(data);
}


