
/*
 * $Id: buffer2.c,v 1.3 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: buffer2.c,v $
 * Revision 1.3  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/05/09 22:08:13  clivewebster
 * Use sized variables
 *
 * Revision 1.1  2010/01/24 12:12:03  clivewebster
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
 * buffer2.c
 *
 *  Created on: 21-Jan-2010
 *      Author: Clive Webster
 */
#include "buffer.h"

// dump (discard) the first numbytes from the front of the buffer
void bufferDump(cBuffer* buffer, size_t numbytes){
	// begin critical section
	CRITICAL_SECTION_START;
		// dump numbytes from the front of the buffer
		// are we dumping less than the entire buffer?
		if(numbytes < buffer->datalength){
			// move index down by numbytes and decrement length by numbytes
			buffer->dataindex = (buffer->dataindex + numbytes) % buffer->size;
			buffer->datalength -= numbytes;
		}else{
			// flush the whole buffer
			buffer->datalength = 0;
		}
		// end critical section
	CRITICAL_SECTION_END;
}

// get a byte at the specified index in the buffer (kind of like array access)
// ** note: this does not remove the byte that was read from the buffer
uint8_t	bufferGetAtIndex(const cBuffer* buffer, size_t index){
	uint8_t data;

	// begin critical section
	CRITICAL_SECTION{
		// return character at index in buffer
		data = (buffer) ? buffer->dataptr[(buffer->dataindex+index)%(buffer->size)] : 0;
		// end critical section
	}
	return data;
}


