/*
 * $Id: color1.c,v 1.6 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: color1.c,v $
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
 * color1.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */
#include "color.h"
#include <string.h>		// for memcpy


// Convert the color to RGB
COLOR_RGB* color2rgb(const COLOR * src, COLOR* dest){		// Convert the color to RGB
	switch(src->colorSpace){

	case RGB:{
		// Nothing to do
		if(dest!=src){
			memcpy(dest, src, sizeof(COLOR));
		}
		break;
		}
	case YUV:{

		/* Convert yuv to rgb
		r = (int) (( 298.082 * Y               + 408.583 * V  ) / 256 - 222.921);
		g = (int) (( 298.082 * Y - 100.291 * U - 208.120 * V  ) / 256 + 135.576);
		b = (int) (( 298.082 * Y + 516.412 * U                ) / 256 - 276.836);
		*/

		int Y = src->bands.yuv.y;
		int Cb = src->bands.yuv.u;
		int Cr = src->bands.yuv.v;

		int r,g,b;
		r = (int) (( 298.082 * Y                + 408.583 * Cr ) / 256 - 222.921);
		g = (int) (( 298.082 * Y - 100.291 * Cb - 208.120 * Cr ) / 256 + 135.576);
		b = (int) (( 298.082 * Y + 516.412 * Cb                ) / 256 - 276.836);

		r = CLAMP(r,0,255);
		g = CLAMP(g,0,255);
		b = CLAMP(b,0,255);
		colorSetRGB(dest,r,g,b);
		break;
		}
	}
	return &dest->bands.rgb;
}

