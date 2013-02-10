/*
 * $Id: blackfin_vp.c,v 1.7 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin_vp.c,v $
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
 *
 * blackfin_vc.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */

#include "blackfin.h"

#include "../../rprintf.h"
#include "../../color.h"


/*
 * Fetch the pixel color at a given location
 */

void blackfinGetPixel(BLACKFIN_CAMERA* camera,uint16_t x, uint16_t y, COLOR * color){

	int32_t values[3];

	// Make rprintf go to _blackfin_command
	 Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;

	// send 'vp' command
	rprintf("vp");
	rprintfNum(10,4,FALSE,'0',x);
	rprintfNum(10,4,FALSE,'0',y);

	// process the command

	int args = __blackfinCommand(camera,PSTR("##vp "),null,values,3);

	if(args==3){
		colorSetYUV(color,(uint8_t)(values[0]), (uint8_t)(values[1]),(uint8_t)(values[2]));
		#ifdef BLACKFIN_DEBUG
		_blackfin_set_active(camera->debug);	//Send rprintf to the debugger
		rprintf(" Color = "); colorDump(color);
		rprintfCRLF();
		#endif
	}

	// Restore rprintf to original position
	rprintfInit(old);
}
