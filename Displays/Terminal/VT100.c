/*
 * $Id: VT100.c,v 1.2 2011/07/06 00:15:44 clivewebster Exp $
 * Revision History
 * ================
 * $Log: VT100.c,v $
 * Revision 1.2  2011/07/06 00:15:44  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.1  2011/03/07 01:32:35  clivewebster
 * Added in Version 1.30
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
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
 *        File: VT100.c
 *  Created on: 3 Mar 2011
 *      Author: Clive Webster
 */
#include "VT100.h"
#include "../../rprintf.h"

// Write a raw value to the screen
static void sendRawByte(const DISPLAY* display, uint8_t c){
	VT100* device = (VT100*)display;
	device->_uart_->writer(c);
}

static DISPLAY* disp;
static MAKE_WRITER(ctrl){
	sendRawByte(disp,byte);
	return byte;
}

// Initialise the display
static void init(DISPLAY* display){
	VT100* device = (VT100*)display;
	// Initialise the uart
	_uartInit(device->_uart_ ,device->baudrate);

	disp = display;
	Writer old = rprintfInit(ctrl);
	rprintf("\x1B\x63");	// reset to power on
//	rprintf("\x1B[?25l");	// Turn cursor off
	rprintf("\x1B[?50l");	// Turn cursor off
	rprintfInit(old);
}


// Clear the screen
static void	cls(DISPLAY* display){
	sendRawByte(display, 0x1b); // esc
	sendRawByte(display, '['); 	// [
	sendRawByte(display, '2'); 	// 2
	sendRawByte(display, 'J'); 	// J
}


// Goto x,y cursor location
static void	gotoXY(DISPLAY* display,DISPLAY_COLUMN x, DISPLAY_LINE y){
	disp = display;
	Writer old = rprintfInit(ctrl);
	rprintf("\x1B[%d;%dH",y+1,x+1);
	rprintfInit(old);
}




static void hgraph(DISPLAY* display,uint16_t pixels,uint8_t width){
	while(width--){
		uint8_t c;
		c = (pixels>=5) ? 5 : pixels;
		pixels -= c;
		c = (c==5) ? '#' : ' ';
		_displaySendByte(display,c);
	}

}

static void vgraph(DISPLAY* display,DISPLAY_COLUMN x,DISPLAY_COLUMN y, uint16_t pixels,uint8_t height){
	while(height--){
		_displayGoto(display,x,y+height);
		uint8_t c;
		c = (pixels>=8) ? 8 : pixels;
		pixels -= c;
		c = (c==8) ? '#' : ' ';
		_displaySendByte(display,c);
	}

}

static void lineWrap(DISPLAY* display,boolean on){
	disp = display;
	Writer old = rprintfInit(ctrl);
	rprintf("\x1B[7");
	rprintfChar( (on) ? 'h' : 'l'  );
	rprintfInit(old);
}


DISPLAY_CLASS PROGMEM c_VT100 = MAKE_DISPLAY_CLASS(&init,&cls,null,&gotoXY, &lineWrap, null, null, null, null, &sendRawByte,&hgraph,&vgraph,null);


