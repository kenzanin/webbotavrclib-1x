
/*
 * $Id: color3.c,v 1.2 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: color3.c,v $
 * Revision 1.2  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/02/04 19:40:14  clivewebster
 * Add function to compare two colors
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
 *
 * color3.c
 *
 *  Created on: 02-Feb-2010
 *      Author: Clive Webster
 */
#include "color.h"

// Test if two colors are identical
boolean colorEquals(const COLOR* c1, const COLOR* c2){
	if(c1 == c2){
		return TRUE;
	}

	if(c1->colorSpace != c2->colorSpace){
		return FALSE;
	}

	boolean rtn = FALSE;
	if(c1->colorSpace == RGB){
		// RGB
		if(c1->bands.rgb.r == c2->bands.rgb.r &&
		   c1->bands.rgb.g == c2->bands.rgb.g &&
		   c1->bands.rgb.b == c2->bands.rgb.b){
			rtn = TRUE;
		}
	}else{
		// YUV
		if(c1->bands.yuv.y == c2->bands.yuv.y &&
		   c1->bands.yuv.u == c2->bands.yuv.u &&
		   c1->bands.yuv.v == c2->bands.yuv.v){
			rtn = TRUE;
		}
	}
	return rtn;
}

