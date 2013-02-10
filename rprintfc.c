/*
 * $Id: rprintfc.c,v 1.5 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfc.c,v $
 * Revision 1.5  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
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
 * rprintfc.c
 *
 *  Implements rprintf complex
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "rprintf.h"
#include "libdefs.h"
#include <stdarg.h>
#include <string.h>


#define INF 40
static unsigned char buf[INF + 1];

boolean Isdigit(char c)
{
	return ((c >= '0') && (c <= '9')) ? TRUE : FALSE;
}

int atoiRamRom(boolean stringInRom, char *str)
{
	int num = 0;

	while(Isdigit(READMEMBYTE(stringInRom,str)))
	{
		num *= 10;
		num += ((READMEMBYTE(stringInRom,str++)) - '0');
	}
	return num;
}


// *** rprintf2RamRom ***
// called by rprintf() - does a more powerful printf (supports %d, %u, %o, %x, %c, %s)
// Supports:
// %d - decimal					(int16_t)
// %u - unsigned decimal		(uint16_t)
// %ld - long decimal			(int32_t)
// %lu - unsigned long decimal	(uint32_t)
// %o - octal					(int16_t)
// %lo - octal					(int32_t)
// %x - hex
// %c - character
// %s - strings
// and the width,precision,padding modifiers
// **this printf does not support floating point numbers
int rprintf2RamRom(unsigned char stringInRom, const char *sfmt, ...)
{
	register unsigned char *f, *bp;
	register long l;
	register unsigned long u;
	register int i;
	register int fmt;
	register unsigned char pad = ' ';
	int flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
	int sign = 0;

	va_list ap;
	va_start(ap, sfmt);

	f = (unsigned char *) sfmt;

	for (; READMEMBYTE(stringInRom,f); f++)
	{
		if (READMEMBYTE(stringInRom,f) != '%')
		{	// not a format character
			// then just output the char
			rprintfChar(READMEMBYTE(stringInRom,f));
		}
		else
		{
			f++;						// if we have a "%" then skip it
			if (READMEMBYTE(stringInRom,f) == '-')
			{
				flush_left = 1;	// minus: flush left
				f++;
			}
            if (READMEMBYTE(stringInRom,f) == '0'
				 || READMEMBYTE(stringInRom,f) == '.')
				{
					// padding with 0 rather than blank
					pad = '0';
					f++;
            }
            if (READMEMBYTE(stringInRom,f) == '*')
				{	// field width
					f_width = va_arg(ap, int);
					f++;
            }
            else if (Isdigit(READMEMBYTE(stringInRom,f)))
				{
					f_width = atoiRamRom(stringInRom, (char *) f);
					while (Isdigit(READMEMBYTE(stringInRom,f)))
						f++;        // skip the digits
            }
            if (READMEMBYTE(stringInRom,f) == '.')
				{	// precision
					f++;
					if (READMEMBYTE(stringInRom,f) == '*')
					{
						prec = va_arg(ap, int);
						f++;
					}
					else if (Isdigit(READMEMBYTE(stringInRom,f)))
					{
						prec = atoiRamRom(stringInRom, (char *) f);
						while (Isdigit(READMEMBYTE(stringInRom,f)))
							f++;    // skip the digits
					}
				}
            if (READMEMBYTE(stringInRom,f) == '#')
				{	// alternate form
					hash = 1;
					f++;
            }
            if (READMEMBYTE(stringInRom,f) == 'l')
				{	// long format
					do_long = 1;
					f++;
            }

				fmt = READMEMBYTE(stringInRom,f);
				bp = buf;
				switch (fmt) {		// do the formatting
				case 'd':			// 'd' signed decimal
					if (do_long)
						l = va_arg(ap, long);
					else
						l = (long) (va_arg(ap, int));
					if (l < 0)
					{
						sign = 1;
						l = -l;
					}
					do	{
						*bp++ = l % 10 + '0';
					} while ((l /= 10) > 0);
					if (sign)
						*bp++ = '-';
					f_width = f_width - (bp - buf);
					if (!flush_left)
						while (f_width-- > 0)
							rprintfChar(pad);
					for (bp--; bp >= buf; bp--)
						rprintfChar(*bp);
					if (flush_left)
						while (f_width-- > 0)
							rprintfChar(' ');
					break;
            case 'o':			// 'o' octal number
            case 'x':			// 'x' hex number
            case 'u':			// 'u' unsigned decimal
					if (do_long)
						u = va_arg(ap, unsigned long);
					else
						u = (unsigned long) (va_arg(ap, unsigned));
					if (fmt == 'u')
					{	// unsigned decimal
						do {
							*bp++ = u % 10 + '0';
						} while ((u /= 10) > 0);
					}
					else if (fmt == 'o')
					{  // octal
						do {
							*bp++ = u % 8 + '0';
						} while ((u /= 8) > 0);
						if (hash)
							*bp++ = '0';
					}
					else if (fmt == 'x')
					{	// hex
						do {
							i = u % 16;
							if (i < 10)
								*bp++ = i + '0';
							else
								*bp++ = i - 10 + 'a';
						} while ((u /= 16) > 0);
						if (hash)
						{
							*bp++ = 'x';
							*bp++ = '0';
						}
					}
					i = f_width - (bp - buf);
					if (!flush_left)
						while (i-- > 0)
							rprintfChar(pad);
					for (bp--; bp >= buf; bp--)
						rprintfChar((int) (*bp));
					if (flush_left)
						while (i-- > 0)
							rprintfChar(' ');
					break;
            case 'c':			// 'c' character
					i = va_arg(ap, int);
					rprintfChar((int) (i));
					break;
            case 's':			// 's' string
					bp = va_arg(ap, unsigned char *);
					if (!bp)
						bp = (unsigned char *) "(nil)";
					f_width = f_width - strlen((char *) bp);
					if (!flush_left)
						while (f_width-- > 0)
							rprintfChar(pad);
					for (i = 0; *bp && i < prec; i++)
					{
						rprintfChar(*bp);
						bp++;
					}
					if (flush_left)
						while (f_width-- > 0)
							rprintfChar(' ');
					break;
            case '%':			// '%' character
					rprintfChar('%');
					break;
			}
			flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
			sign = 0;
			pad = ' ';
		}
	}

	va_end(ap);
	return 0;
}


