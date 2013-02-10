
/*
 * $Id: serLCD.c,v 1.6 2011/07/06 00:15:15 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: serLCD.c,v $
 * Revision 1.6  2011/07/06 00:15:15  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.5  2011/03/07 01:33:42  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.4  2011/01/25 23:47:01  clivewebster
 * Fix 20 column displays and on init then set up serLCD for display size
 *
 * Revision 1.3  2010/10/06 12:10:31  clivewebster
 * Implement displaySetBrightness
 *
 * Revision 1.2  2010/06/14 18:45:06  clivewebster
 * Add copyright license info
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
 * serLCD.c
 *
 *  Created on: 10-Feb-2010
 *      Author: Clive Webster
 */
#include "serLCD.h"
#include "../../core.h"

// Write a raw value to the screen
static void sendRawByte(const DISPLAY* display, uint8_t c){
	SPARKFUN_SERLCD* device = (SPARKFUN_SERLCD*)display;
	device->_uart_->writer(c);
}


// 0x7c, 0x01 turn on  backlight
// 0x7c, 0x02 turn off backlight
static void backlight(DISPLAY* display,boolean on){
	sendRawByte(display,0x7c);
	if(on){
		sendRawByte(display, 1);
	}else{
		sendRawByte(display, 2);
	}
}

// Set the backlight brightness
//  0x7c, n where n = 128 to 157
static void brightness(DISPLAY* display, uint8_t percent){
	uint8_t val = interpolateU(percent,0,100, 128,157);
	sendRawByte(display,0x7c);
	sendRawByte(display,val);
}


// Initialise the display
static void init(DISPLAY* display){
	SPARKFUN_SERLCD* device = (SPARKFUN_SERLCD*)display;
	// Initialise the uart
	_uartInit(device->_uart_ ,device->baudrate);

	// Since the serLCD may have been bought without the LCD display then we need to configure
	// it for the correct number of rows and columns
	uint8_t cols = (device->_display_.columns >= 20) ? 0x03 : 0x04;
	uint8_t rows = (device->_display_.rows >= 4) ? 0x05 : 0x06;
	sendRawByte(display,0x7c);sendRawByte(display,cols);
	sendRawByte(display,0x7c);sendRawByte(display,rows);
}


// Clear the screen
static void	cls(DISPLAY* display){
	sendRawByte(display, 0xfe);
	sendRawByte(display, 1);
}

// The DDRAM address for the start of each line on a 20 character display
static uint8_t PROGMEM lineStartAddr20[] = {0x00, 0x40, 0x14, 0x54 };
// The DDRAM address for the start of each line on a 16 character display
static uint8_t PROGMEM lineStartAddr16[] = {0x00, 0x40, 0x10, 0x50 };

// Goto x,y cursor location
static void	gotoXY(DISPLAY* display,DISPLAY_COLUMN x, DISPLAY_LINE y){
	SPARKFUN_SERLCD* lcd = (SPARKFUN_SERLCD*)display;
	uint8_t offset;
	if(lcd->_display_.columns >= 20){
		offset = pgm_read_byte(&lineStartAddr20[y]);
	}else{
		offset = pgm_read_byte(&lineStartAddr16[y]);
	}
	offset += x;

	sendRawByte(display, 0xfe);
	sendRawByte(display, offset + 128);
}




static void hgraph(DISPLAY* display,uint16_t pixels,uint8_t width){
	while(width--){
		uint8_t c;
		c = (pixels>=5) ? 5 : pixels;
		pixels -= c;
		c = (c==5) ? 0xff : ' ';
		_displaySendByte(display,c);
	}

}

static void vgraph(DISPLAY* display,DISPLAY_COLUMN x,DISPLAY_COLUMN y, uint16_t pixels,uint8_t height){
	while(height--){
		_displayGoto(display,x,y+height);
		uint8_t c;
		c = (pixels>=8) ? 8 : pixels;
		pixels -= c;
		c = (c==8) ? 0xff : ' ';
		_displaySendByte(display,c);
	}

}


DISPLAY_CLASS PROGMEM c_SERLCD = MAKE_DISPLAY_CLASS(&init,&cls,null,&gotoXY, null, null, &backlight, &brightness, null, &sendRawByte,&hgraph,&vgraph,null);

