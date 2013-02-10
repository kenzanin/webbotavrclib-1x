/*
 * $Id: blackfin_vm.c,v 1.6 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin_vm.c,v $
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
 * Find Mean color
 *
 * Response will be:
 * printf("##vmean %d %d %d\r\n", mean[0], mean[1], mean[2]);
 * 		   01234567
 *
 *
 */
#include "blackfin.h"

#include "../../rprintf.h"

const COLOR * blackfinMeanColor(BLACKFIN_CAMERA* camera){
	int32_t values[3];

	// Make rprintf go to _blackfin_command
	Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;

	// send 'vm' command to camera
	rprintf("vm");

	// process the command
	int args = __blackfinCommand(camera,PSTR("##vmean "),null,values,3);

	#ifdef BLACKFIN_DEBUG
	_blackfin_set_active(camera->debug);	//Send rprintf to the debugger
	#endif

	if(args == 3){
		//save it away - stay within limits of 0 to 255 for each band
		CRITICAL_SECTION_START;
		camera->mean->colorSpace = YUV;
		camera->mean->bands.yuv.y = (uint8_t)CLAMP(values[0],0,255);
		camera->mean->bands.yuv.u = (uint8_t)CLAMP(values[1],0,255);
		camera->mean->bands.yuv.v = (uint8_t)CLAMP(values[2],0,255);
		CRITICAL_SECTION_END;

		#ifdef BLACKFIN_DEBUG
		//output for testing
		colorDump(camera->mean);				// Dump the color
		rprintfCRLF();
		#endif
	}else{
		#ifdef BLACKFIN_DEBUG
		rprintf("vm got %d values!\n",args);
		#endif
	}

	// Restore rprintf to original position
	rprintfInit(old);

	return camera->mean;
}
