/*
 * $Id: buffer.h,v 1.8 2011/07/06 00:23:30 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: buffer.h,v $
 * Revision 1.8  2011/07/06 00:23:30  clivewebster
 * Add bufferQueue
 *
 * Revision 1.7  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/06/07 18:26:03  clivewebster
 * Add bufferFreeSpace
 *
 * Revision 1.5  2010/05/09 22:07:48  clivewebster
 * Use sized variables
 *
 * Revision 1.4  2010/04/25 03:30:59  clivewebster
 * Flush copes with null buffer
 *
 * Revision 1.3  2009/11/02 18:54:01  clivewebster
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "libdefs.h"

// structure/type defs
typedef struct struct_cBuffer{
	uint8_t*	dataptr;			// the physical memory address where the buffer is stored
	size_t 		size;				// the allocated size of the buffer
	size_t 		datalength;			// the length of the data currently in the buffer
	size_t 		dataindex;			// the index into the buffer where the data starts
} cBuffer;

// Constructor
#define MAKE_BUFFER(bytes) {bytes,sizeof(bytes),0,0}


// function prototypes

// initialize a buffer to start at a given address and have given size
void bufferInit(cBuffer* buffer, uint8_t *start, size_t size);
boolean bufferGet(cBuffer* buffer, uint8_t* rtn);


// dump (discard) the first numbytes from the front of the buffer
void bufferDump(cBuffer* buffer, size_t numbytes);

// get a byte at the specified index in the buffer (kind of like array access)
// ** note: this does not remove the byte that was read from the buffer
uint8_t	bufferGetAtIndex(const cBuffer* buffer, size_t index);

// add a byte to the end of the buffer
boolean	bufferPut(cBuffer* buffer, uint8_t data);

boolean bufferIsFull(const cBuffer* buffer);

// flush (clear) the contents of the buffer
void bufferFlush(cBuffer* buffer);

// How many bytes are free in the buffer
size_t bufferFreeSpace(const cBuffer* buffer);

// The number of used bytes in the buffer
size_t bufferBytesUsed(const cBuffer* buffer);

// Attempt to write 'len' bytes from 'src' to the buffer as an all-or-nothing
// Returns FALSE if not enough space, TRUE if it all written
boolean bufferQueue(cBuffer* buffer, const void* src, size_t len);


#endif
