/*
 * $Id: blackfin_vf.c,v 1.6 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin_vf.c,v $
 * Revision 1.6  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/18 00:02:24  clivewebster
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
 * blackfin_vc.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */

#include "blackfin.h"

#include "../../rprintf.h"


/*
 * Count the number of pixels in a rectangle that match a given color bin
 */

uint32_t blackfinCountPixels(BLACKFIN_CAMERA* camera,uint8_t bin, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2){

	int32_t values[1];

	// Make rprintf go to _blackfin_command
	Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;

	// send 'vf' command
	rprintf("vf%d",(int)bin);//start command, send bin #
	rprintfNum(10,4,FALSE,'0',MIN(x1,x2));
	rprintfNum(10,4,FALSE,'0',MAX(x1,x2));
	rprintfNum(10,4,FALSE,'0',MIN(y1,y2));
	rprintfNum(10,4,FALSE,'0',MAX(y1,y2));

	// process the command

	int args = __blackfinCommand(camera,PSTR("##vf "),null,values,1);

	#ifdef BLACKFIN_DEBUG
	//test code
	if(args==1){
		_blackfin_set_active(camera->debug);	//Send rprintf to the debugger
		rprintf(" pixels = ");
		rprintfNum(10,10,FALSE,' ',values[0]);
		rprintfCRLF();
	}
	#endif

	// Restore rprintf to original position
	rprintfInit(old);

	return (args==1) ? values[0] : 0;
}
