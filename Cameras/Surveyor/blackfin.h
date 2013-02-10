/*
 * $Id: blackfin.h,v 1.8 2010/07/15 14:36:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin.h,v $
 * Revision 1.8  2010/07/15 14:36:57  clivewebster
 * Allow Project Designer to set the initial resolution
 *
 * Revision 1.7  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/02/04 19:46:57  clivewebster
 * Updated a comment
 *
 * Revision 1.5  2009/12/27 21:53:17  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/13 19:21:10  clivewebster
 * Changed MAKE_BLACKFIN_CAMERA to something more intuitive
 *
 * Revision 1.3  2009/12/11 17:09:10  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:34:07  clivewebster
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
 * blackfin.h
 *
 *  Created on: 11-Aug-2009
 *      Author: Clive Webster
 *
 *  Support for the Blackfin camera
 */

#ifndef BLACKFIN_H_
#define BLACKFIN_H_

//#define BLACKFIN_DEBUG

#include "../../_uart_common.h"
#include "../../color.h"
#include "../../libdefs.h"



// Define the different camera resolution
typedef enum {
 	BLACKFIN_160_BY_120,
 	BLACKFIN_320_BY_240,
 	BLACKFIN_640_BY_480,
 	BLACKFIN_1280_BY_1024
} BLACKFIN_RESOLUTION;

// To override the default resolution then add the following to your code:-
// #define BLACKFIN_RESOLUTION_DEFAULT BLACKFIN_1280_BY_1024
#ifndef BLACKFIN_RESOLUTION_DEFAULT
#define BLACKFIN_RESOLUTION_DEFAULT BLACKFIN_160_BY_120
#endif


// Define the structure of blob
typedef struct s_blackfin_blob {
	uint16_t	left;
	uint16_t	right;
	uint16_t	top;			// yMin
	uint16_t	bottom;		// yMax
	uint16_t	xCenter;
	uint16_t	yCenter;
	uint32_t	pixels;
} BLACKFIN_BLOB;
#define MAKE_BLACKFIN_BLOB() { 0,0,0,0,0,0,0 }

typedef struct s_blackfin {
	UART* 				camera;	// The UART used to talk to the camera
	BLACKFIN_RESOLUTION res; 	// The current resolution of the camera
	BLACKFIN_BLOB*		blob;	// The default area to read a blob into
	COLOR*				mean;	// The last read mean value of the image
#ifdef BLACKFIN_DEBUG
	UART*    			debug;	// The UART used to send debug output to
#endif
} BLACKFIN_CAMERA;

// Routines that are private to the library - dont change these as it will mean rebuilding the library
#ifdef BUILDING_LIBRARY
//#define BLACKFIN_DEBUG
int __blackfin_get_args(const BLACKFIN_CAMERA* camera,int32_t * values, size_t numValues, boolean trace);
uint8_t _blackfin_putcmd(uint8_t data);
int __blackfinCommand(const BLACKFIN_CAMERA* camera, const char* response, const char* error, int32_t * values, size_t numValues);
extern size_t _blackfin_index;
void _blackfin_sendYUV(uint8_t c);
#endif




// Define the standard constructor
// The first parameter is the UART used to connect to the camera
// The second parameter is the UART used to write out debug info - or null if not required
#ifdef BLACKFIN_DEBUG
#define MAKE_BLACKFIN_CAMERA_RES(camera,res,debug)  {&((camera)->_uart_),res,null,null,&((debug)->_uart_)}
#define MAKE_BLACKFIN_CAMERA(camera,debug)  MAKE_BLACKFIN_CAMERA_RES(camera,BLACKFIN_RESOLUTION_DEFAULT,debug)
#else
#define MAKE_BLACKFIN_CAMERA_RES(camera,res)  {&((camera)->_uart_),res,null,null}
#define MAKE_BLACKFIN_CAMERA(camera)  MAKE_BLACKFIN_CAMERA_RES(camera,BLACKFIN_RESOLUTION_DEFAULT)
#endif

static __inline__ BLACKFIN_RESOLUTION blackfinGetResolution(const BLACKFIN_CAMERA* camera){
	return camera->res;
};


// Set the resolution of the camera
void blackfinSetResolution(BLACKFIN_CAMERA* camera, BLACKFIN_RESOLUTION res);

// Initialise the camera
void blackfinInit(BLACKFIN_CAMERA* camera);

// Calculate the mean color
const COLOR * blackfinMeanColor(BLACKFIN_CAMERA* camera);

// Get the pixel at a given location and write to the specified COLOR variable
void blackfinGetPixel(BLACKFIN_CAMERA* camera,uint16_t x, uint16_t y, COLOR * color);

// Set the minimum and maximum color values for the specified bin
// bin - is the bin number in the range 0 to 9
// min - is the minimum color
// max - is the maximum color
void blackfinSetBinRange(BLACKFIN_CAMERA* camera, uint8_t bin, const COLOR* min, const COLOR* max);

// Return the number of pixels in a rectangle that match a given color bin
uint32_t blackfinCountPixels(BLACKFIN_CAMERA* camera,uint8_t bin, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);

// Detect the blobs for a given color bin
// Returns the number of blobs - which can be 0 (none) up to 16
//
uint8_t blackfinDetectBlobs(BLACKFIN_CAMERA* camera, uint8_t bin);

// blobNo - starts at zero and goes up to the returned value from detect blobs-1
const BLACKFIN_BLOB* blackfinFetchBlob(BLACKFIN_CAMERA* camera, uint8_t blobNo);


#ifdef BLACKFIN_DEBUG
Writer _blackfin_set_active(UART* uart);
#endif

#endif /* BLACKFIN_H_ */
