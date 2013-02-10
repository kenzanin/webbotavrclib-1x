/*
 * $Id: blackfin_vb.c,v 1.6 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin_vb.c,v $
 * Revision 1.6  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/18 00:02:23  clivewebster
 * Changed case sensitive Blackfin.h to blackfin.h
 *
 * Revision 1.4  2009/12/27 21:53:17  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/11/02 18:34:07  clivewebster
 * Added revision log
 *
 * ===========
 *
 * Copyright (C) 2010 Clive Webster (Webbot)
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
 * blackfin_vb.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */

#include "blackfin.h"

#include "../../rprintf.h"
#include <stdlib.h>

#include "../../timer.h"

#define MAX_BLOBS 4

/*
 vb

	Returns multiple lines one for each blob

             ix = vblob((unsigned char *)FRAME_BUF, (unsigned char*)FRAME_BUF3, ch1);
             printf("##vb%c %d\r\n", ch2, ix);
             for (iy=0; iy<ix; iy++) {
                 printf(" %d - %d %d %d %d  \r\n",
                     blobcnt[iy], blobx1[iy], blobx2[iy], bloby1[iy],bloby2[iy]);
             }

 */
uint8_t blackfinDetectBlobs (BLACKFIN_CAMERA* camera, uint8_t bin){
	int32_t values[5];
	BLACKFIN_BLOB* dest;
	uint8_t actual = 0;

	// allocate space for blob storage
	if(camera->blob==null){
		camera->blob = malloc(MAX_BLOBS * sizeof(BLACKFIN_BLOB)) ;
	}
	dest = camera->blob;

	// Make rprintf go to _blackfin_command
	Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;

	rprintf("vb%d",bin);//start command, send bin #

	// process the command
	int args = __blackfinCommand(camera,PSTR("##vb"),PSTR("  vblob #"),values,2);

	#ifdef BLACKFIN_DEBUG
	_blackfin_set_active(camera->debug);	//Send rprintf to the debugger
	#endif

	if(args==2 && values[0]==bin){
		int8_t numBlobs = values[1];		// The number of blobs
		if(numBlobs > MAX_BLOBS){
			numBlobs = MAX_BLOBS;
		}

		#ifdef BLACKFIN_DEBUG
		rprintf(" %d blobs\n",(int)numBlobs);
		#endif

		// Get each blob
		for(int8_t num = 0; num < numBlobs; num++){
			args = __blackfin_get_args(camera, values, 5, TRUE);
			#ifdef BLACKFIN_DEBUG
			rprintf(" #%d=",(int)num);
			#endif
			if(args==5){
				dest->pixels = values[0];
				dest->left 	 = values[1];
				dest->right  = values[2];
				dest->top  	 = values[3];
				dest->bottom = values[4];
				dest->xCenter = dest->left + ((dest->right - dest->left)>>1);
				dest->yCenter = dest->top + ((dest->bottom - dest->top)>>1);
				#ifdef BLACKFIN_DEBUG
					rprintf("Left,Top=");
					rprintfNum(10,4,FALSE,'0',dest->left);
					rprintfChar(',');
					rprintfNum(10,4,FALSE,'0',dest->top);
					rprintf(" Right,Bottom=");
					rprintfNum(10,4,FALSE,'0',dest->right);
					rprintfChar(',');
					rprintfNum(10,4,FALSE,'0',dest->bottom);
					rprintf(" Pixels=");rprintfNum(10,10,FALSE,' ',dest->pixels);
					rprintfCRLF();
				#endif
				dest++;
				actual++;
			}else{
				#ifdef BLACKFIN_DEBUG
					rprintf("Expected 5 args but got %d\n",args);
				#endif
			}
			// Read up to linefeed
			while(__uartGetByte(camera->camera)!='\n');
		}
//exit:
	}else{
		#ifdef BLACKFIN_DEBUG
		rprintf("vb - missing num blobs\n");
		#endif
	}

	// Restore rprintf to original position
	rprintfInit(old);

	return actual;
}


const BLACKFIN_BLOB* blackfinFetchBlob(BLACKFIN_CAMERA* camera, uint8_t blobNo){
	return &camera->blob[blobNo];
}
