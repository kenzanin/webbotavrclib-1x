
/*
 * $Id: _display_common.c,v 1.6 2011/07/06 00:11:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _display_common.c,v $
 * Revision 1.6  2011/07/06 00:11:48  clivewebster
 * Rename variable class to sclass to avoid C++ reserved word
 *
 * Revision 1.5  2011/03/07 01:33:21  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.4  2010/10/06 12:09:54  clivewebster
 * Set backlight brightness to 50% at startup
 *
 * Revision 1.3  2010/06/14 18:45:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/21 19:47:11  clivewebster
 * *** empty log message ***
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
 * _display_common.c
 *
 *  Created on: 07-Feb-2010
 *      Author: Clive Webster
 *
 *  Implement the abstract methods for all displays
 */
#include "_display_common.h"
#include "../core.h"
#include <stdlib.h>

static void	     _displayScrollUp(DISPLAY* display);
static void 	 _displayNewLine(DISPLAY* display);
static void 	 _displayReturn(DISPLAY* display);

// Custom chars are 5 pixels across and 8 pixels down
static uint8_t PROGMEM HorizCustomChar[] = {
		// Horizontal bar graph
		0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, // 0. 1/5 full progress block
		0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, // 1. 2/5 full progress block
		0x00, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x00, // 2. 3/5 full progress block
		0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, // 3. 4/5 full progress block
		0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 4. 5/5 full progress block
};

// Vertical bar graph
static uint8_t PROGMEM VertCustomChar[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E
};

static void setHGraph(DISPLAY* display){
	for(uint8_t c = 0; c<5; c++){
		_displayCustomChar(display, &HorizCustomChar[c<<3], c);
	}

	display->hgraph = TRUE;
}

static void setVGraph(DISPLAY* display){
    // Set up vertical graph chars
    for(uint8_t c=0; c<8; c++){
    	_displayCustomChar(display, &VertCustomChar[c], c);
    }
    display->hgraph = FALSE;
}

void _displayCustomChar(DISPLAY* display, const uint8_t* def, uint8_t customChar){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, const uint8_t*, uint8_t) = (void (*)(DISPLAY*,const uint8_t*, uint8_t))pgm_read_word(&class->customChar);
	if(fn){
		fn(display,def,customChar);
	}
}


// Initialise a display
void _displayInit(DISPLAY* display){
	// Get the method implementation for this class of camera
	const DISPLAY_CLASS* class = display->sclass;

	// Initialise the buffer
	if(display->buffer==null){
		size_t numBytes = display->columns * display->rows;
		display->buffer = malloc(numBytes);
	}

	// Call any specific init required by the device
	void (*fn)(DISPLAY*) = (void (*)(DISPLAY*))pgm_read_word(&class->init);
	if(fn){
		fn(display);
	}
//	_displayBrightness(display,50);		// 50% backlight
//	_displayBacklight(display,FALSE);	// Turn backlight off
	_displayClear(display);				// Clear the screen
	_displayAutoScroll(display,FALSE);	// Turn auto scroll off
	_displayLineWrap(display,FALSE);	// Turn line wrap off

    // Set up horizontal graph chars
	setHGraph(display);
}

// Clear the screen
void _displayClear(DISPLAY* display){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*) = (void (*)(DISPLAY*))pgm_read_word(&class->cls);
	if(fn){
		fn(display);
	}

	if(display->buffer){
		uint8_t* pos = display->buffer;
		uint8_t count = display->columns * display->rows;
		while(count--){
			*pos++ = ' ';
		}
	}
	_displayHome(display);

}

// Move cursor to home position
void _displayHome(DISPLAY* display){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*) = (void (*)(DISPLAY*))pgm_read_word(&class->home);
	if(fn){
		fn(display);
	}
	_displayGoto(display,0,0);
}

// Move to a given x,y
void _displayGoto(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y){
	const DISPLAY_CLASS* class = display->sclass;

	// Put into valid range
	x %= display->columns;
	y %= display->rows;
	display->cursorX = x;
	display->cursorY = y;

	void (*fn)(DISPLAY*, DISPLAY_COLUMN, DISPLAY_LINE) =
		(void (*)(DISPLAY*, DISPLAY_COLUMN, DISPLAY_LINE))pgm_read_word(&class->gotoXY);
	if(fn){
		fn(display,x,y);
	}
}

void _displayLineWrap(DISPLAY* display,boolean on){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, boolean) = (void (*)(DISPLAY*,boolean))pgm_read_word(&class->lineWrap);
	if(fn){
		fn(display,on);
	}
	display->linewrap = on;
}

// Turn autoscroll on or off
void _displayAutoScroll(DISPLAY* display,boolean on){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, boolean) = (void (*)(DISPLAY*,boolean))pgm_read_word(&class->autoScroll);
	if(fn){
		fn(display,on);
	}
	display->autoscroll = on;
}

// Turn the backlight on or off
void _displayBacklight(DISPLAY* display,boolean on){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, boolean) = (void (*)(DISPLAY*,boolean))pgm_read_word(&class->backlight);
	if(fn){
		fn(display,on);
	}
}

// Set the display brightness
void _displayBrightness(DISPLAY* display,uint8_t percent){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, uint8_t) = (void (*)(DISPLAY*,uint8_t))pgm_read_word(&class->setBrightness);
	if(fn){
		if(percent>100) percent=100;
		fn(display,percent);
	}
}


// Set the display contrast if it is supported
void _displayContrast(DISPLAY* display,uint8_t percent){
	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, uint8_t) = (void (*)(DISPLAY*,uint8_t))pgm_read_word(&class->setContrast);
	if(fn){
		if(percent>100) percent=100;
		fn(display,percent);
	}
}

// Send an ASCII character to the display
uint8_t _displaySendByte(DISPLAY* display,uint8_t byte){
	if (byte=='\n'){
		_displayNewLine(display);
	} else if( byte=='\r'){
		_displayReturn(display);
	} else  {
		// If at end of line then move to next line down
		if(display->cursorX == display->columns){
			if(display->linewrap){
				_displaySendByte(display, '\r');
				_displaySendByte(display, '\n');
				_displaySendByte(display, byte);
			}else{
				// throw away the character
			}
		}else{
			// In middle of line so just write it
			if(display->buffer){
				display->buffer[display->cursorY * display->columns + display->cursorX] = byte;
			}
			display->cursorX++;

			const DISPLAY_CLASS* class = display->sclass;
			void (*fn)(const DISPLAY*, uint8_t) = (void (*)(const DISPLAY*,uint8_t))pgm_read_word(&class->sendRawByte);
			fn(display,byte);
		}
	}
	return byte;
}

// Display a horizontal graph with its left point at x,y
// val is the value to be shown - where max is the largest possible value
// count is the number of character cells across to show
void _displayHorizGraph(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y, uint16_t val, uint16_t max, uint8_t count){

	// Make sure horizontal graph mode is enabled
	if(!display->hgraph){
		setHGraph(display);
	}


	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, uint16_t,uint8_t) = (void (*)(DISPLAY*,uint16_t, uint8_t))pgm_read_word(&class->hgraph);
	if(fn){
		_displayGoto(display, x, y);				// Move to start of graph
		val = CLAMP(val,0,max);						// make sure value is in range
		val = interpolateU(val,0,max,0,5*count);	// convert to number of pixels
		fn(display,val,count);						// Draw the graph
	}
}

// Display a vertical graph with its top point at x,y
// val is the value to be shown - where max is the largest possible value
// count is the number of character cells across to show
void _displayVertGraph(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y, uint16_t val, uint16_t max, uint8_t count){

	// Make sure vertical graph mode is enabled
	if(display->hgraph){
		setVGraph(display);
	}

	const DISPLAY_CLASS* class = display->sclass;
	void (*fn)(DISPLAY*, DISPLAY_COLUMN, DISPLAY_LINE,uint16_t,uint8_t) = (void (*)(DISPLAY*,DISPLAY_COLUMN,DISPLAY_LINE,uint16_t, uint8_t))pgm_read_word(&class->vgraph);
	if(fn){
		val = CLAMP(val,0,max);						// make sure value is in range
		val = interpolateU(val,0,max,0,8*count);	// convert to number of pixels
		fn(display,x,y,val,count);						// Draw the graph
	}
}

static void _displayScrollUp(DISPLAY* display){
	// Save current settings
	boolean old1 = display->autoscroll;
	boolean old2 = display->linewrap;

	// Set new values
	display->autoscroll = FALSE;
	display->linewrap = TRUE;

	// Move cursor to the home position
	_displayHome(display);

	// move lines up
	uint8_t count = (display->rows-1) * display->columns;
	uint8_t *pos = display->buffer + display->columns;
	while(count--){
		_displaySendByte(display, *pos++);
	}
	// Fill last line with spaces
	count = display->columns;
	while(count--){
		_displaySendByte(display, ' ');
		display->linewrap = FALSE;
	}

	// Restore original settings
	display->autoscroll = old1;
	display->linewrap = old2;
}

static void _displayNewLine(DISPLAY* display){
	// Move to the next line
	display->cursorY ++;

	if(display->cursorY == display->rows){
		// Gone past end of screen
		if(display->autoscroll){
			// Scroll the existing text up
			_displayScrollUp(display);
			display->cursorY=display->rows-1;
			display->cursorX=0;
		}else{
			// Just wrap around
			display->cursorY %= display->rows;
		}
	}
	_displayGoto(display,display->cursorX,display->cursorY);

}

static void _displayReturn(DISPLAY* display){
	display->cursorX = 0;
	// Move to start of current line
	_displayGoto(display,display->cursorX,display->cursorY);

}

