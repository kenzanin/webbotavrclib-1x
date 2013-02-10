/*
 * $Id: rprintfs.c,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfs.c,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/24 12:13:32  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 19:02:47  clivewebster
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
 * rprintfs.c
 *
 *  rprintf simple
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "rprintf.h"
#include "libdefs.h"
#include <stdarg.h>

// called by rprintf() - does a simple printf (supports %d, %x, %c)
// Supports:
// %d - decimal
// %x - hex
// %c - character
int rprintf1RamRom(unsigned char stringInRom, const char *format, ...)
{
	// simple printf routine
	char format_flag;
	char* str;
	unsigned int u_val, div_val, base;
	va_list ap;

	va_start(ap, format);
	for (;;)
	{
		while ((format_flag = READMEMBYTE(stringInRom,format++) ) != '%')
		{	// Until '%' or '\0'
			if (!format_flag)
			{
				va_end(ap);
				return(0);
			}
			rprintfChar(format_flag);
		}

		switch (format_flag = READMEMBYTE(stringInRom,format++) )
		{
			case 's': str = va_arg(ap,char*); rprintfStr(str);continue;
			case 'c': format_flag = va_arg(ap,int);
					  rprintfChar(format_flag);
					  continue;
			default:  rprintfChar('%');
			case '%':
					  rprintfChar(format_flag);
					  continue;
			case 'u': case 'd': base = 10; div_val = 10000; goto CONVERSION_LOOP;
			case 'x': base = 16; div_val = 0x1000;

			CONVERSION_LOOP:
			u_val = va_arg(ap,int);
			if (format_flag == 'd')
			{
				if (((int)u_val) < 0)
				{
					u_val = - u_val;
					rprintfChar('-');
				}
			}
			if(format_flag == 'u' || format_flag == 'd'){
				// Strip leading zeroes
				while (div_val > 1 && div_val > u_val) div_val /= 10;
			}
			do
			{
				rprintfu04(u_val/div_val);
				u_val %= div_val;
				div_val /= base;
			} while (div_val);
		}
	}
	va_end(ap);
}


