
/*
 * $Id: MatrixOrbital.c,v 1.5 2011/07/06 00:14:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MatrixOrbital.c,v $
 * Revision 1.5  2011/07/06 00:14:48  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.4  2011/03/07 01:33:42  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.3  2010/06/14 18:45:04  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/21 19:48:57  clivewebster
 * Make graphing compatible with other LCDs
 *
 * Revision 1.1  2010/02/18 01:45:09  clivewebster
 * Added
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
 * MatrixOrbital.c
 *
 *  Created on: 10-Feb-2010
 *      Author: Clive Webster

 *
 */
#include "MatrixOrbital.h"
#include "../../timer.h"

// Write a raw character to the screen
static void sendRawByte(const DISPLAY* display, uint8_t c){
	MATRIX_ORBITAL* morb = (MATRIX_ORBITAL*)display;
	morb->_uart_->writer(c);
}

//
// y = 255 * percent / 100
// set brightness n (0..255) = 254,153,n
static void setBrightness(DISPLAY* display, uint8_t percent){
	uint16_t temp = 255 * percent;
	temp /= 100;
	sendRawByte(display, 254);
	sendRawByte(display, 153);
	sendRawByte(display, (uint8_t)temp);
}
//
// y = 255 * percent / 100
// set contrast n (0..255) = 254,80,n
static void setContrast(DISPLAY* display, uint8_t percent){
	uint16_t temp = 255 * percent;
	temp /= 100;
	sendRawByte(display, 254);
	sendRawByte(display,  80);
	sendRawByte(display, (uint8_t)temp);
}

// line wrap on = 254,67
// line wrap off = 254,68
static void lineWrap(DISPLAY* display,boolean on){
	sendRawByte(display,254);
	sendRawByte(display, (on) ? 67 : 68);
}

// auto scroll on = 254,81
// auto scroll off = 254,82
static void autoScroll(DISPLAY* display,boolean on){
	sendRawByte(display,254);
	sendRawByte(display, (on) ? 81 : 82);
}

//backlight on  = 254,66,0
//backlight off = 254,70
static void backlight(DISPLAY* display,boolean on){
	sendRawByte(display,254);
	if(on){
		sendRawByte(display, 66);
		sendRawByte(display, 0);	// keep 'on' for unlimited time
	}else{
		sendRawByte(display, 70);
	}
}

static void customChar(DISPLAY* display, const uint8_t* pgm_def, uint8_t charNum){
	sendRawByte(display, 254);sendRawByte(display,78);				// set custom char
	sendRawByte(display, charNum);
	for(uint8_t i=0; i<8; i++){
		sendRawByte(display, pgm_read_byte(&pgm_def[i]));
	}
}



// Initialise the display
static void init(DISPLAY* display){
	MATRIX_ORBITAL* device = (MATRIX_ORBITAL*)display;
	_uartInit(device->_uart_ ,device->baudrate);

	sendRawByte(display, 254);sendRawByte(display,75);					// Turn off underline cursor
	sendRawByte(display, 254);sendRawByte(display,84);					// Turn off blinking block cursor


}

// Clear the screen
static void	cls(DISPLAY* display){
	sendRawByte(display, 254);
	sendRawByte(display, 88);
}

// Goto x,y cursor location
static void	gotoXY(DISPLAY* display,DISPLAY_COLUMN x, DISPLAY_LINE y){
	sendRawByte(display, 254);
	sendRawByte(display, 71);
	sendRawByte(display, x+1);
	sendRawByte(display, y+1);
}


static void hgraph(DISPLAY* display, uint16_t pixels,uint8_t width){
	while(width--){
		uint8_t c;
		c = (pixels>=5) ? 5 : pixels;
		pixels -= c;
		c = (c==0) ? ' ' : c-1;
		_displaySendByte(display,c);
	}


	/*
	write(display,254);write(display,124); // Draw horizontal graph
	write(display,x+1);		// 1s rel column
	write(display,y+1);		// 1s rel line
	write(display,0);		// From left to right
	write(display,pixels);	// The number of pixels
	*/
}

static void vgraph(DISPLAY* display,DISPLAY_COLUMN x,DISPLAY_COLUMN y, uint16_t pixels,uint8_t height){
	while(height--){
		_displayGoto(display,x,y+height);
		uint8_t c;
		c = (pixels>=8) ? 8 : pixels;
		pixels -= c;
		c = (c==0) ? ' ' : c-1;
		_displaySendByte(display,c);
	}
/*
 * Inbuilt cmd uses both lines of the display
	write(display,254);write(display,61);	// Draw vertical graph
	write(display,x+1);						// 1s rel column
	write(display,pixels);					// The number of pixels
*/
}



DISPLAY_CLASS const PROGMEM c_MatrixOrbital = MAKE_DISPLAY_CLASS(&init,&cls,null,&gotoXY, &lineWrap, &autoScroll, &backlight, &setBrightness, &setContrast,&sendRawByte, &hgraph, &vgraph,&customChar);
