/*
 * $Id: buffer.c,v 1.9 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: buffer.c,v $
 * Revision 1.9  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/06/07 18:26:03  clivewebster
 * Add bufferFreeSpace
 *
 * Revision 1.7  2010/05/09 22:07:48  clivewebster
 * Use sized variables
 *
 * Revision 1.6  2010/04/25 03:30:59  clivewebster
 * Flush copes with null buffer
 *
 * Revision 1.5  2010/01/24 12:12:03  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 18:54:01  clivewebster
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
 * buffer.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */
#include "buffer.h"

// initialize a buffer to start at a given address and have given size
void bufferInit(cBuffer* buffer, uint8_t* start, size_t size){
	// begin critical section
	CRITICAL_SECTION_START;
		// set start pointer of the buffer
		buffer->dataptr = start;
		buffer->size = size;
		// initialize index and length
		buffer->dataindex = buffer->datalength = 0;
		// end critical section
	CRITICAL_SECTION_END;
}


boolean bufferGet(cBuffer* buffer, uint8_t* rtn){
	boolean ok=FALSE;
	// begin critical section
	CRITICAL_SECTION_START;
	// make sure we have a receive buffer
	if(buffer && buffer->size){
		// make sure we have data
		if(buffer->datalength){
			// get the first character from buffer
			*rtn = buffer->dataptr[buffer->dataindex];
			// move index down and decrement length
			buffer->dataindex = (buffer->dataindex + 1) % buffer->size;
			buffer->datalength--;
			ok = TRUE;
		}
	}

	// end critical section
	CRITICAL_SECTION_END;
	return ok;
}


// add a byte to the end of the buffer
boolean	bufferPut(cBuffer* buffer, uint8_t data){
	boolean rtn=FALSE; // failure

	// begin critical section
	CRITICAL_SECTION_START;

		// make sure the buffer has room
		if(buffer->datalength < buffer->size)
		{
			// save data byte at end of buffer
			buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
			// increment the length
			buffer->datalength++;
			// return success
			rtn = TRUE;
		}
		// end critical section
	CRITICAL_SECTION_END;
	return rtn;
}

size_t bufferFreeSpace(const cBuffer* buffer){
	size_t rtn;
	CRITICAL_SECTION{
		rtn =  (buffer) ? buffer->size - buffer->datalength : 0;
	}
	return rtn;
}

size_t bufferBytesUsed(const cBuffer* buffer){
	size_t rtn;
	CRITICAL_SECTION{
		rtn =  (buffer) ? buffer->datalength : 0;
	}
	return rtn;
}

boolean bufferIsFull(const cBuffer* buffer){
	return (bufferFreeSpace(buffer)==0) ? TRUE : FALSE;
}

// flush (clear) the contents of the buffer
void bufferFlush(cBuffer* buffer){
	if(buffer){
		// begin critical section
		CRITICAL_SECTION{
			// flush contents of the buffer
			buffer->datalength = 0;
			// end critical section
		}
	}
}

