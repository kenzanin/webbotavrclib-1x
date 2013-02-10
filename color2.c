/*
 * $Id: color2.c,v 1.6 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: color2.c,v $
 * Revision 1.6  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/04 19:40:14  clivewebster
 * Add function to compare two colors
 *
 * Revision 1.4  2009/11/02 18:54:58  clivewebster
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
 * color2.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */
#include "color.h"
#include <string.h>		// for memcpy

// Convert the color to YUV
COLOR_YUV* color2yuv(const COLOR * src, COLOR* dest){
	switch(src->colorSpace){
	case YUV:
		// Nothing to do
		if(src != dest){
			memcpy(dest, src, sizeof(COLOR));
		}
		break;
	case RGB:{
			/*
			Y  =  16 + 1/256 * (  65.738 * R'd + 129.057 * G'd +  25.064 * B'd)
			Cb = 128 + 1/256 * (- 37.945 * R'd -  74.494 * G'd + 112.439 * B'd)
			Cr = 128 + 1/256 * ( 112.439 * R'd -  94.154 * G'd -  18.285 * B'd)
			*/

			int r = src->bands.rgb.r;
			int g = src->bands.rgb.g;
			int b = src->bands.rgb.b;


			int y =  16.0 + 1.0/256.0 * (  65.738 * r + 129.057 * g +  25.064 * b);
			int u = 128.0 + 1.0/256.0 * (- 37.945 * r -  74.494 * g + 112.439 * b);
			int v = 128.0 + 1.0/256.0 * ( 112.439 * r -  94.154 * g -  18.285 * b);

			y = CLAMP(y,0,255);
			u = CLAMP(u,0,255);
			v = CLAMP(v,0,255);

			// Convert rgb to yuv
			colorSetYUV(dest,y,u,v);
			break;
		}
	}
	return &dest->bands.yuv;

}

