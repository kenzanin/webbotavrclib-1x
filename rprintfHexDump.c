/*
 * $Id: rprintfHexDump.c,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: rprintfHexDump.c,v $
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
 * rprintfHexDump.c
 *
 *  Created on: 22-Aug-2009
 *      Author: Clive Webster
 */

#include "rprintf.h"
#include "libdefs.h"


void rprintfMemoryDump(const void* data, size_t off, size_t len){
    int x;
    int c;
    int line;
    const uint8_t * b= (const uint8_t *)data;

    rprintf("ADDR|  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | ASCII\n");
    rprintfCharN('-',72); rprintfCRLF();
    for(line = 0; line < ((len % 16 != 0) ? (len / 16) + 1 : (len / 16)); line++){
    	int line16 = line * 16;
    	rprintfu16(line16);
        rprintf("| ");

        // put hex values
        for(x = 0; x < 16; x++){
            if(x + line16 < len){
                c = b[off + x + line16];
                rprintfu08(c);
                rprintfChar(' ');
            }else{
                rprintfCharN(' ',3);
            }
        }
        rprintfChar('|');rprintfChar(' ');

        // put ascii values
        for(x = 0; x < 16; x++){
            if(x + line16 < len){
                c = b[off + x + line16];
                if((c > 0x1f) && (c < 0x7f)){
                	// ascii
                    rprintfChar((unsigned char)c);
                }else{
                	// non ascii
                	rprintfChar('.');
                }
             }else{
             	rprintfChar(' ');
             }
        }
        rprintfCRLF();
    }
    rprintfCharN('-',72); rprintfCRLF();
}
