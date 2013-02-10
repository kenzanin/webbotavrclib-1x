/*
 * $Id: _display_common.h,v 1.5 2011/07/06 00:11:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _display_common.h,v $
 * Revision 1.5  2011/07/06 00:11:48  clivewebster
 * Rename variable class to sclass to avoid C++ reserved word
 *
 * Revision 1.4  2011/03/07 01:33:21  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.3  2010/10/06 12:09:55  clivewebster
 * Set backlight brightness to 50% at startup
 *
 * Revision 1.2  2010/06/14 18:45:07  clivewebster
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
 * _display_common.h
 *
 *  Created on: 07-Feb-2010
 *      Author: Clive Webster
 *
 *  Defines structures common to all Displays as well as the common API for all Displays
 *  After initialisation then:
 *  	auto scroll is off
 *  	line wrap is off
 *  	backlight is off
 */

#ifndef _DISPLAY_COMMON_H_
#define _DISPLAY_COMMON_H_
#include "../libdefs.h"

typedef uint8_t DISPLAY_COLUMN;
typedef uint8_t DISPLAY_LINE;

// Define the data for a generic display
typedef struct s_display {
	const struct PROGMEM c_display*	sclass;  // The constant definition of the display
	const DISPLAY_COLUMN  columns;			// The number of columns across
	const DISPLAY_LINE    rows;				// The number of rows down
	Writer 				  writer;			// Get the writer routine for the display
	boolean				  linewrap:1;		// Is auto line wrap on
	boolean				  autoscroll:1;		// Is auto scroll on
	boolean				  hgraph:1;			// TRUE if horiz graph defined, FALSE if vert graph defined
	DISPLAY_COLUMN 		  cursorX;			// The current cursor column
	DISPLAY_LINE          cursorY;			// The current cursor row
	uint8_t*  			  buffer;			// The display map
} DISPLAY;
#define MAKE_DISPLAY(class, columns, rows, writer) { class, columns, rows, writer, FALSE,FALSE,FALSE,0,0,null }


// Define the API for display commands
typedef struct PROGMEM c_display{
	void    (*init)(DISPLAY* display);				// Initialise the display
	void	(*cls)(DISPLAY* display);				// Clear the screen
	void	(*home)(DISPLAY* display);				// Move cursor to top left
	void	(*gotoXY)(DISPLAY* display, 			/* Move cursor to location */
			DISPLAY_COLUMN x,
			DISPLAY_LINE y);
	void    (*lineWrap)(DISPLAY* display,boolean on);
	void 	(*autoScroll)(DISPLAY* display,boolean on);
	void 	(*backlight)(DISPLAY* display,boolean on);
	void 	(*setBrightness)(DISPLAY* display, uint8_t percent);
	void 	(*setContrast)(DISPLAY* display, uint8_t percent);
	void	(*sendRawByte)(const DISPLAY* display, uint8_t byte);
	void	(*hgraph)(DISPLAY* display, uint16_t pixels,uint8_t width);
	void	(*vgraph)(DISPLAY* display,DISPLAY_COLUMN x,DISPLAY_LINE y,uint16_t pixels,uint8_t width);
	void	(*customChar)(DISPLAY* display, const uint8_t* pgm_def, uint8_t ch);
} DISPLAY_CLASS;

#define MAKE_DISPLAY_CLASS(init,cls,home,gotoXY,lineWrap,autoScroll,backlight,setBrightness,setContrast,sendRaw,hgraph,vgraph, custom) { \
	init,cls,home,gotoXY,lineWrap,autoScroll,backlight,setBrightness,setContrast ,sendRaw,hgraph,vgraph, custom \
}

// Declare the public methods that can be called on a given display instance
// These pass the call onto the matching abstrat method starting with an '_'
#define displayColumns(display) 				_displayColumns(&((display)->_display_))
#define displayLines(display) 					_displayLines(&((display)->_display_))
#define displayGetWriter(display) 				_displayGetWriter(&((display)->_display_))
#define displayInit(display) 					_displayInit(&((display)->_display_))
#define displayClear(display) 					_displayClear(&((display)->_display_))
#define displayHome(display) 					_displayHome(&((display)->_display_))
#define displayGoto(display,x,y) 				_displayGoto(&((display)->_display_),x,y)
#define displayLineWrap(display,on) 			_displayLineWrap(&((display)->_display_),on)
#define displayAutoScroll(display,on) 			_displayAutoScroll(&((display)->_display_),on)
#define displayBacklight(display,on) 			_displayBacklight(&((display)->_display_),on)
#define displayBrightness(display,percent) 		_displayBrightness(&((display)->_display_),percent)
#define displayContrast(display,percent) 		_displayContrast(&((display)->_display_),percent)
#define displaySendByte(display,byte) 			_displaySendByte(&((display)->_display_),byte)
#define displayHorizGraph(display,x,y,val,max,count) \
												_displayHorizGraph(&((display)->_display_),x,y,val,max,count)
#define displayVertGraph(display,x,y,val,max,count) \
												_displayVertGraph(&((display)->_display_),x,y,val,max,count)
#define displayCustomChar(display, pgm_def, ch) _displayCustomChar(&((display)->_display_), pgm_def, ch);
//#define displayScrollUp(display) 				_displayScrollUp(&((display)->_display_))
//#define displayNewLine(display)					_displayNewLine(&((display)->_display_));
//#define displayReturn(display)					_displayReturn(&((display)->_display_));


// Declare the private methods that can be called on an abstract display instance
static __inline__ DISPLAY_COLUMN _displayColumns(const DISPLAY* display){ return display->columns;}
static __inline__ DISPLAY_LINE   _displayLines(const DISPLAY* display) {return display->rows;}
static __inline__ Writer 	_displayGetWriter(const DISPLAY* display) {return display->writer;}
extern void      _displayInit(DISPLAY* display);
extern void      _displayClear(DISPLAY* display);
extern void      _displayHome(DISPLAY* display);
extern void	     _displayGoto(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y);
extern void	 	 _displayLineWrap(DISPLAY* display,boolean on);
extern void	 	 _displayAutoScroll(DISPLAY* display,boolean on);
extern void	 	 _displayBacklight(DISPLAY* display,boolean on);
extern void	 	 _displayBrightness(DISPLAY* display,uint8_t percent); // Brightness of backlight
extern void	 	 _displayContrast(DISPLAY* display,uint8_t percent);
extern uint8_t 	 _displaySendByte(DISPLAY* display,uint8_t byte);
extern void 	 _displayHorizGraph(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y, uint16_t val, uint16_t max, uint8_t count);
extern void 	 _displayVertGraph(DISPLAY* display, DISPLAY_COLUMN x, DISPLAY_LINE y, uint16_t val, uint16_t max, uint8_t count);
extern void	     _displayCustomChar(DISPLAY* display, const uint8_t* pgm_def, uint8_t customChar);
//extern void	     _displayScrollUp(DISPLAY* display);
//extern void 	 _displayNewLine(DISPLAY* display);
//extern void 	 _displayReturn(DISPLAY* display);

#if defined(BUILDING_LIBRARY)
// Define methods only to be used by DISPLAY driver code - dont call from your own code
#endif

#endif /* _DISPLAY_COMMON_H_ */
