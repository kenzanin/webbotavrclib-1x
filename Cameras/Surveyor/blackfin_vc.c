/*
 * $Id: blackfin_vc.c,v 1.7 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin_vc.c,v $
 * Revision 1.7  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/02/18 00:02:24  clivewebster
 * Changed case sensitive Blackfin.h to blackfin.h
 *
 * Revision 1.5  2009/12/27 21:53:17  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 18:34:07  clivewebster
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
 * blackfin_vc.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */

#include "blackfin.h"

#include "../../rprintf.h"


//sets the range of a color bin (0-9)
//example:	vc0090255000255000255      returns ##vc0
//              012345678901234567890
//sets color bin #0 using Y mean, 255, 0, 255, 0, 255 so
//pixels of higher than average brightness will appear in blob search

void blackfinSetBinRange(BLACKFIN_CAMERA* camera,uint8_t bin, const COLOR* min, const COLOR* max){

	int32_t values[1];

	COLOR yuvMin;
	COLOR yuvMax;

	// Convert the colors to the YUV color space
	COLOR_YUV* minPtr = color2yuv(min, &yuvMin);
	COLOR_YUV* maxPtr = color2yuv(max, &yuvMax);

	// Make rprintf go to _blackfin_command
	Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;

	// send 'vc' command
	rprintf("vc%d",(int)bin);//start command, send bin #
	_blackfin_sendYUV(MIN(minPtr->y,maxPtr->y));	// minimum y
	_blackfin_sendYUV(MAX(minPtr->y,maxPtr->y));	// maximum y
	_blackfin_sendYUV(MIN(minPtr->u,maxPtr->u));	// minimum u
	_blackfin_sendYUV(MAX(minPtr->u,maxPtr->u));	// maximum u
	_blackfin_sendYUV(MIN(minPtr->v,maxPtr->v));	// minimum v
	_blackfin_sendYUV(MAX(minPtr->v,maxPtr->v));	// maximum v

	// process the command

	#ifdef BLACKFIN_DEBUG
	int args = __blackfinCommand(camera,PSTR("##vc "),null,values,1);
	//test code
	if(args==1){
		_blackfin_set_active(camera->debug);	//Send rprintf to the debugger
		rprintf(" Bin %d ",(uint8_t)values[0]);
		rprintf(" Min="); colorDump(min);
		rprintf(" Max="); colorDump(max);
		rprintfCRLF();
	}
	#else
	__blackfinCommand(camera,PSTR("##vc"),null,values,1);
	#endif

	// Restore rprintf to original position
	rprintfInit(old);
}
