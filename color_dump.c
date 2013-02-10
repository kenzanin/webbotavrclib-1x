/*
 * $Id: color_dump.c,v 1.3 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: color_dump.c,v $
 * Revision 1.3  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:54:58  clivewebster
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
 * color_dump.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */
#include "color.h"
#include "rprintf.h"

void colorDump(const COLOR* color){
	switch(color->colorSpace){
	case RGB:
		rprintf("RGB(%d,%d,%d)",(int)(color->bands.rgb.r), (int)(color->bands.rgb.g), (int)(color->bands.rgb.b));
		break;
	case YUV:
		rprintf("YUV(%d,%d,%d)",(int)(color->bands.yuv.y), (int)(color->bands.yuv.u), (int)(color->bands.yuv.v));
		break;
	}
}


