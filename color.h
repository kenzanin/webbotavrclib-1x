/*
 * $Id: color.h,v 1.4 2010/06/14 19:21:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: color.h,v $
 * Revision 1.4  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/02/04 19:40:14  clivewebster
 * Add function to compare two colors
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
 * Color.h
 *
 *  Created on: 12-Aug-2009
 *      Author: Clive Webster
 *
 *  A color in a given color space
 *
 *  The supported color spaces are YUV and RGB
 *
 *  Support is provided to convert from one color space to another
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "libdefs.h"

typedef enum e_colorspace{
	RGB, YUV
} COLOR_SPACE;

// The YUV color space
typedef struct s_yuv{
	uint8_t y;
	uint8_t u;
	uint8_t v;
} COLOR_YUV;

// The RGB color space
typedef struct s_rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} COLOR_RGB;

typedef struct s_color{
	COLOR_SPACE colorSpace;		// The color space that is in use
	union {
		COLOR_YUV yuv;			// Settings if it is a YUV color
		COLOR_RGB rgb;			// Settings if it is an RGB color
	} bands;
} COLOR;

COLOR_RGB* color2rgb(const COLOR * src, COLOR* dest);		// Convert the color to RGB
COLOR_YUV* color2yuv(const COLOR * src, COLOR* dest);		// Convert the color to YUV

static __inline__ void colorSetRGB(COLOR* color, uint8_t r, uint8_t g, uint8_t b ){
	color->colorSpace = RGB;
	color->bands.rgb.r = r;
	color->bands.rgb.g = g;
	color->bands.rgb.b = b;
}

static __inline__ void colorSetYUV(COLOR* color, uint8_t y, uint8_t u, uint8_t v ){
	color->colorSpace = YUV;
	color->bands.yuv.y = y;
	color->bands.yuv.u = u;
	color->bands.yuv.v = v;
}

void colorDump(const COLOR* color);		// Send the color to rprintf
boolean colorEquals(const COLOR* c1, const COLOR* c2);  // Test if two colors are identical
#endif /* COLOR_H_ */
