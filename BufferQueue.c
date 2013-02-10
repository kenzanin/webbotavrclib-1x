/*
 * $Id: BufferQueue.c,v 1.1 2011/07/06 00:23:30 clivewebster Exp $
 * Revision History
 * ================
 * $Log: BufferQueue.c,v $
 * Revision 1.1  2011/07/06 00:23:30  clivewebster
 * Add bufferQueue
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: BufferQueue.c
 *  Created on: 21 Jun 2011
 *      Author: Clive Webster
 */
#include "buffer.h"

boolean bufferQueue(cBuffer* buffer, const void* src, size_t len){
	boolean rtn = FALSE;
	if(buffer && buffer->size){
		CRITICAL_SECTION{
			// Check there is enough space
			if(buffer->size - buffer->datalength >= len){
				rtn = TRUE;
				const uint8_t* srcBytes = (const uint8_t*)src;

				// Get the current put location
				size_t dstIndex = (buffer->dataindex + buffer->datalength) % buffer->size;
				// Get the final length of the buffer
				buffer->datalength += len;
				// Put out all the bytes
				while(len--){
					buffer->dataptr[dstIndex++] = *srcBytes++;
					if(dstIndex == buffer->size){
						dstIndex = 0;
					}
				}
			}
		}
	}
	return rtn;
}
