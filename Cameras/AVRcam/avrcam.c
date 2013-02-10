
/*
 * $Id: avrcam.c,v 1.3 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: avrcam.c,v $
 * Revision 1.3  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/21 19:46:29  clivewebster
 * Fix trackStart to return proper TRUE/FALSE
 *
 * Revision 1.1  2010/02/04 22:27:22  clivewebster
 * Added
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
 * avrcam.c
 *
 *  Created on: 31-Jan-2010
 *      Author: Clive Webster
 *
 *  Camera implementation for an AVRcam camera
 *  Fixed resolution of 176 x 144
 */
#include "avrcam.h"
#include "../../timer.h"
#include "../../rprintf.h"
#include <stdlib.h>

// Forward definition of implementations
static void 	init(CAMERA* camera);		// Iniitialse a camera
static uint16_t  xres(CAMERA* camera);		// Get the x resolution
static uint16_t  yres(CAMERA* camera);		// Get the y resolution
static boolean   setBin(CAMERA* camera,		// Set a colour bin and send to the camera when needed
		uint8_t binNo, const COLOR* min, const COLOR* max);

// Forward definition of internal methods
static boolean 	trackStop(AVRCAM* camera);
static boolean 	trackStart(AVRCAM* camera);
static boolean 	ping(AVRCAM* camera);
static boolean 	sendBins(AVRCAM* camera);
static boolean  sendCmd(AVRCAM* camera, const char* cmd);
static size_t 	getResponse(AVRCAM* camera, char chStart, char chEnd, size_t minLen, size_t maxLen);
static uint8_t	getBlobs(CAMERA* camera, uint8_t bin); // Read blobs
static boolean setCameraReg(AVRCAM* avrcam, uint8_t reg, uint8_t val);
static char*	getVersion(CAMERA* camera);
static boolean sampleRectangle(AVRCAM* avrcam, uint16_t minX, uint16_t minY, uint16_t width, uint16_t height,
		COLOR* rtnMin, COLOR *rtnMax, COLOR* rtnMean);


// The AVRcam supports 8 colour bins and 8 blobs
#define AVRCAM_BINS 8
#define AVRCAM_BLOBS 8

// Max time to wait for line to complete 300ms
#define	TIMEOUT	(TICK_COUNT)300000

// Create a shared buffer for receiving responses
#define	MAX_REPLY 255
static char	response[MAX_REPLY + 1];


#define flush(camera) __uartFlushReceiveBuffer(camera->_camera_.uart)


static UART* active;

static MAKE_WRITER(sendChar) {
	_uartSendByte(active,byte);
	return byte;
}

static Writer setActive(AVRCAM* camera){
	// Set the active uart
	active = camera->_camera_.uart;
	return rprintfInit(&sendChar);
}


static void init(CAMERA* camera){
	AVRCAM* avrcam = (AVRCAM*)camera;

	// set camera to 115200 baud - this is fixed by the camera
	_uartInit(avrcam->_camera_.uart, (BAUD_RATE) 115200);
	flush(avrcam);

	// Make sure tracking mode is turned off
	trackStop(avrcam);

	// Make RAM space for the colour bins
	if(camera->bins == NULL){
		camera->bins = malloc(AVRCAM_BINS * sizeof(CAMERA_BIN));
	}

	// Zap all colour bins to RGB(0,0,0) except bin#0
	if(camera->bins != NULL){
		for(uint8_t i = 0; i < AVRCAM_BINS; i++){
			CAMERA_BIN* bin = &camera->bins[i];
			bin->active = bin->dirty = FALSE;
			if(i==0){
				COLOR min,max;
				colorSetRGB(&min,180,180,180);
				colorSetRGB(&max,255,255,255);
				setBin(camera,i,&min,&max);
			}else{
				colorSetRGB(&(bin->min),0,0,0);
				colorSetRGB(&(bin->max),0,0,0);
			}
		}
	}

	// Allocate space for blobs
	if(camera->blobs == null){
		camera->blobs = malloc(AVRCAM_BLOBS * sizeof(CAMERA_BLOB));
	}

	// Try an initial ping of the camera
	uint8_t cnt = 15;
	while(!ping(avrcam) && --cnt);

	if(cnt){
		// We got a reply to our ping
		delay_us(TIMEOUT);
		flush(avrcam);

		setCameraReg(avrcam,0x12,0x28);					// AGC=on, RGB mode, AWB=off
//		setCameraReg(avrcam,0x12,0x2C);					// AGC=on, RGB mode, AWB=on

		setCameraReg(avrcam,0x13,0);					// Turn off auto mode adjust
//		setCameraReg(avrcam,0x13,1);					// Turn on auto mode adjust

		setCameraReg(avrcam,0x2D,3);					// Turn off flourescent lighting filer
//		setCameraReg(avrcam,0x2D,7);					// Turn on flourescent lighting filer

		// Update camera with our initial colour bins
//		sendBins(avrcam);
	}

}

static uint16_t  xres(CAMERA* camera){
//	AVRCAM* avrcam = (AVRCAM*)camera;
	return (uint16_t)176;
}

static uint16_t  yres(CAMERA* camera){
//	AVRCAM* avrcam = (AVRCAM*)camera;
	return (uint16_t)144;
}

// Turn on tracking mode
// return TRUE if tracking mode enabled
static boolean trackStart(AVRCAM* camera) {
	if(!camera->tracking) {
		// Send any colour bin changes
		if(sendBins(camera)){
			// Now put into tracing mode
			if(sendCmd(camera, "ET")){
				camera->tracking = TRUE;
			}
		}
	}
	return (camera->tracking) ? TRUE : FALSE;
}

// Turn off tracking mode
// return TRUE if tacking mode disabled
static boolean trackStop(AVRCAM* camera) {
	if(camera->tracking) {
		if(sendCmd(camera, "DT")){
			camera->tracking = FALSE;
		}
	}
	return (camera->tracking) ? FALSE : TRUE;
}

// ping the camera
// return TRUE if ok, FALSE if not
static boolean ping(AVRCAM* camera){
	return sendCmd(camera,"PG");
}

// Set the value of a colour bin to RGB
static boolean setBin(CAMERA* camera, uint8_t binNo, const COLOR* min, const COLOR* max){
//	AVRCAM* avrcam = (AVRCAM*)camera;
	boolean rtn = FALSE;

	if(binNo < AVRCAM_BINS){
		CAMERA_BIN* bin = &camera->bins[binNo];

		COLOR _min;
		COLOR _max;
		COLOR_RGB *_minRGB;
		COLOR_RGB *_maxRGB;

		// Convert min to rgb
		_minRGB = color2rgb(min, &_min);

		// Convert max to rgb
		_maxRGB = color2rgb(max, &_max);

		if(_minRGB->r > _maxRGB->r){
			// swap the red values
			uint8_t c = _minRGB->r;
			_minRGB->r = _maxRGB->r;
			_maxRGB->r = c;
		}

		if(_minRGB->g > _maxRGB->g){
			// swap the green values
			uint8_t c = _minRGB->g;
			_minRGB->g = _maxRGB->g;
			_maxRGB->g = c;
		}

		if(_minRGB->b > _maxRGB->b){
			// swap the blue values
			uint8_t c = _minRGB->b;
			_minRGB->b = _maxRGB->b;
			_maxRGB->b = c;
		}

		if(colorEquals(&_min, &bin->min)==FALSE || colorEquals(&_max, &bin->max)==FALSE){
			// The colours have changed

			// Put into color bank table
			color2rgb(&_min, &bin->min);
			color2rgb(&_max, &bin->max);
			bin->dirty = TRUE;
		}

		rtn = bin->active = TRUE;

	}
	return rtn;
}


// Send the color bins to the camera
// This will only do something if the color bins have changed
static boolean sendBins(AVRCAM* camera){
	// See if anything is dirty
	boolean dirty = FALSE;
	for (uint8_t index=0; index<AVRCAM_BINS; index++) {
		CAMERA_BIN* bin = &camera->_camera_.bins[index];
		dirty |= bin->dirty;
	}
	if(dirty==FALSE){
		return TRUE;
	}

	// Send rprintf output to the camera
	Writer old = setActive(camera);

	// Command for set colour maps
	rprintf("SM");

	// 0=red, 1=green, 2=blue
	for(uint8_t color=0; color<3; color++) {
		rprintf(" 0");							// First entry is always 0

		for(uint8_t i=1; i<16; i++) {			// Step through bits of the grid
			uint8_t entry=0;

			// Step through the color map entries
			for (uint8_t index=0; index<AVRCAM_BINS; index++) {

				// Get the min and max colours
				uint8_t high,low;

				CAMERA_BIN* bin = &camera->_camera_.bins[index];
				COLOR min,max;
				COLOR_RGB* minRGB = color2rgb(&bin->min, &min);
				COLOR_RGB* maxRGB = color2rgb(&bin->max, &max);

				if (color==0) {
					// Red
					high=maxRGB->r;
					low =minRGB->r;
				} else if (color==1) {
					// Green
					high=maxRGB->g;
					low =minRGB->g;
				} else if (color==2) {
					// Blue
					high=maxRGB->b;
					low =minRGB->b;
				}

				if ((low<=i*16) && (high>=i*16) ) {	// Is value between the high and low?
					entry |= 1<<index;
				}											// Set bit
			}
			rprintf(" %d",entry);							// Send byte
		}
	}
	// Restore rprintf destination
	rprintfInit(old);

	// Send a null command
	boolean rtn = sendCmd(camera, NULL);

	// If success - then clear all dirty flags
	if(rtn){
		for (uint8_t index=0; index<AVRCAM_BINS; index++) {
			CAMERA_BIN* bin = &camera->_camera_.bins[index];
			bin->dirty = FALSE;
		}
	}

	return rtn;
}



// Send a null terminated string and await response
// Return TRUE if successfull
static boolean  sendCmd(AVRCAM* camera, const char* cmd){
	boolean rtn = FALSE;

	Writer old = setActive(camera);
	flush(camera);							// Flush receive buffer
	rprintf("%s\r",cmd);					// Send the command
	rprintfInit(old);

	// Read the response
	size_t len=getResponse(camera, 0,'\r',0,0);

	if (len>0) {
		char * ack=response+len-3;			// Look at last 3 characters

		// Test for an ACK response
		if(ack[0]=='A' && ack[1]=='C' && ack[2]=='K'){
			rtn = TRUE;
		}
	}
	return rtn;
}

// Get a line of response and return the length (0=error)
static size_t getResponse(AVRCAM* camera, char chStart, char chEnd, size_t minLen, size_t maxLen) {
	size_t		counter=0;		// Counts the length of the string

	// Have we located the start character yet?
	boolean		inLine= (chStart) ? FALSE : TRUE;

	if (! maxLen) {
		maxLen=MAX_REPLY;
	}

	// Save the start time
	TICK_COUNT startTime=clockGetus();
	uint16_t noclock = 60000;
	while(noclock) {
		if(g_heartbeat){
			if(clockHasElapsed(startTime, TIMEOUT)){
				// We've timed out
				return 0;
			}
		}else{
			noclock--;
		}

		// Get next byte, or -1 if none
		int temp=__uartGetByte(camera->_camera_.uart);

		if(temp!=-1){
			noclock = 60000;
		}

		if(inLine){
			// We are processing the line
			if(temp==chEnd && counter>=minLen) {
				// Found the end of the line
				response[counter]='\0';
				return counter;
			}else if (temp != -1) {
				// Save the next character
				response[counter++]=temp;

				// Check if the line is too long
				if (counter>=maxLen) {
					return 0;
				}
			}
		}else{
			// we are waiting for start of line
			if(temp == chStart){
				inLine = TRUE;
			}
		}
	}
	return 0;
}

// Read the blobs for a given color bin
static uint8_t	getBlobs(CAMERA* camera, uint8_t bin){
	AVRCAM* avrcam = (AVRCAM*)camera;
	camera->numBlobs = 0;

	if(trackStart(avrcam)){
		// ok we are in tracking mode

		// Read the next tracking line
		size_t len=getResponse(avrcam, 0x0A,0xFF,0,0);
		if (len>0) {
			uint8_t ptr=0;
			uint8_t lineBlobs = response[ptr++];					// Get the number of returned blobs
			for (uint8_t i=0; i<lineBlobs; i++) {					// Iterate through the blobs
				CAMERA_BLOB aBlob;									// Build the blob info here

				aBlob.bin = response[ptr++];						// Get the color bin
				aBlob.left = response[ptr++];						// Get left
				aBlob.top  = response[ptr++];						// Get top
				aBlob.right = response[ptr++];						// Get right
				aBlob.bottom  = response[ptr++];					// Get bottom

				// Ignore if it is not for the correct bin
				if(bin == CAMERA_ALL_BINS || aBlob.bin == bin){
					_cameraInsertBlob(camera,&aBlob);
				}
			}
		}
	}
	// Make sure tracking is off
	trackStop(avrcam);
	return camera->numBlobs;
}

// Get the version of camera software
static char *getVersion(CAMERA* camera) {
	return(sendCmd((AVRCAM*)camera,"GV")) ? response : null;
}


// A frame DUMP can take 4 seconds to transmit all of its data so very slow!
static boolean sampleRectangle(AVRCAM* avrcam, uint16_t minX, uint16_t minY, uint16_t width, uint16_t height,
		COLOR* rtnMin, COLOR *rtnMax, COLOR* rtnMean
		) {
	uint8_t	cnt=yres(&avrcam->_camera_) / 2;
	uint8_t rectMinX = minX / 2;
	uint8_t rectMinY = minY / 2;
	uint8_t rectMaxX = rectMinX + (width+1)/2;
	uint8_t rectMaxY = rectMinY + (height+1)/2;
	boolean first = TRUE;
	COLOR  clrMin;
	COLOR  clrMax;
	uint32_t totalRed,totalGreen,totalBlue;
	uint16_t numRed,numGreen,numBlue;
	boolean rtn = TRUE;

	if(sendCmd(avrcam, "DF")) {
		totalRed=totalGreen=totalBlue=0;
		numRed=numGreen=numBlue = 0;

		// Takes a while for data to start
		delay_ms(1000);
		while(cnt--) {
			// NB line numbers can be sent in any order
			size_t len=getResponse(avrcam,0x0B,0x0F,2,0);
			uint8_t line=response[0];
			if(len==177) {
				if(line>=rectMinY && line<=rectMaxY) {
					// The line number is in range
					// So process the required X pixels
					for(uint8_t i=rectMinX;i<=rectMaxX;i++) {
						uint8_t ptr=1+i*2;
						// Get the rgb colours for the pixel
						uint8_t x1=response[ptr];
						uint8_t x2=response[ptr+1];
						uint8_t	g1=x1&0xF0;
						uint8_t	b=(x1&0x0F)<<4;
						uint8_t	g2=x2&0xF0;
						uint8_t	r=(x2&0x0F)<<4;
						uint8_t gH=MAX(g1,g2);
						uint8_t gL=MIN(g1,g2);

						if(first) {
							// Its the first line so initlialise
							colorSetRGB(&clrMin,r,gL,b);
							colorSetRGB(&clrMax,r,gH,b);
							first=false;
						} else {
							// keep the min/max values
							clrMin.bands.rgb.r = MIN(r ,clrMin.bands.rgb.r);
							clrMin.bands.rgb.g = MIN(gL,clrMin.bands.rgb.g);
							clrMin.bands.rgb.b = MIN(b ,clrMin.bands.rgb.b);

							clrMax.bands.rgb.r = MAX(r ,clrMax.bands.rgb.r);
							clrMax.bands.rgb.g = MAX(gH,clrMax.bands.rgb.g);
							clrMax.bands.rgb.b = MAX(b ,clrMax.bands.rgb.b);

							totalRed += r; ++numRed;
							totalGreen += gL;
							totalGreen += gH; numGreen+=2;
							totalBlue += b; ++numBlue;
						}
					}
				}
			} else {
				// Line is not the correct length
				rtn = FALSE;
				break;
			}
		} // Next line

	} else {
		// Send DF failed
		rtn = FALSE;
	}

	if(rtn){
		if(rtnMin){
			colorSetRGB(rtnMin,clrMin.bands.rgb.r,clrMin.bands.rgb.g,clrMin.bands.rgb.b);
		}
		if(rtnMax){
			colorSetRGB(rtnMax,clrMax.bands.rgb.r,clrMax.bands.rgb.g,clrMax.bands.rgb.b);
		}
		if(rtnMean){
			totalRed = (numRed==0) ? 0 : totalRed / numRed;
			totalGreen = (numGreen==0) ? 0 : totalGreen / numGreen;
			totalBlue = (numBlue==0) ? 0 : totalBlue / numBlue;
			colorSetRGB(rtnMean, (uint8_t)totalRed, (uint8_t)totalGreen, (uint8_t)totalBlue);
		}
	}

	// Flush out anything left over
	flush(avrcam);
	return rtn;
}

// Set a register
static boolean setCameraReg(AVRCAM* avrcam, uint8_t reg, uint8_t val) {
	Writer old = setActive(avrcam);
	rprintf("CR %d %d",reg,val);
	boolean rtn = sendCmd(avrcam, NULL);
	rprintfInit(old);
	return rtn;
}

static boolean getPixel(CAMERA* camera,uint16_t x, uint16_t y, COLOR * color){
	return sampleRectangle((AVRCAM*)camera, x,y,1,1,NULL,NULL,color);
}


// Create the class for the AVRcam
CAMERA_CLASS c_avrcam = MAKE_CAMERA_CLASS( &init, &xres, &yres, AVRCAM_BINS, AVRCAM_BLOBS,&setBin, &getBlobs, &getPixel, &getVersion);

