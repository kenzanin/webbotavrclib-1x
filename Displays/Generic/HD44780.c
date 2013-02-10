
/*
 * $Id: HD44780.c,v 1.7 2011/07/06 00:13:17 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: HD44780.c,v $
 * Revision 1.7  2011/07/06 00:13:17  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.6  2011/03/07 01:33:41  clivewebster
 * Refactor in Version 1.30
 *
 * Revision 1.5  2011/02/21 20:26:59  clivewebster
 * Fix bug with 16 vs 20 column displays
 *
 * Revision 1.4  2010/07/01 23:57:19  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.3  2010/06/14 18:45:05  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/02/21 19:48:10  clivewebster
 * Fix issue when changing between Vertical and Horizontal graphs
 *
 * Revision 1.1  2010/02/18 01:45:09  clivewebster
 * Added
 *
 * ===========
 *
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
 * HD44780.c
 *
 *  Created on: 09-Feb-2010
 *      Author: Clive Webster
 *
 *  Implement a generic parallel display based on the HD44780 or SED1278 controller chips
 */

#include "HD44780.h"
#include "../../timer.h"
#include <stdlib.h>

#define LCD_CLR   			0b00000001		// Clear screen

#define LCD_HOME  			0b00000010		// Home

#define LCD_ENTRY 			0b00000100		// Set the Entry mode to:
#define LCD_ENTRY_INC   	0b00000010		// Set=increment, Clear=decrement
#define LCD_ENTRY_SHIFT 	0b00000001		// Set=display shift on, Clear=display shift off

#define LCD_ON 				0b00001000		// Turn LCD on using:
#define LCD_ON_DISPLAY		0b00000100		// Turn LCD display on
#define LCD_ON_CURSOR		0b00000010		// Turn cursor on
#define LCD_ON_BLINK		0b00000001		// Turn cursor blink on

#define LCD_MOVE			0b00010000		// What moves?
#define LCD_MOVE_DISP		0b00001000		//  Scroll the display
#define LCD_MOVE_RIGHT		0b00000100		//  Move the cursor right

#define LCD_FUNCTION		0b00100000		// Function set
#define LCD_FUNCTION_8BIT	0b00010000		// - Set for 8 bit mode, Clear for 4 bit mode
#define LCD_FUNCTION_2LINES	0b00001000		// - Set for 2 lines, Clear for 1 line
#define LCD_FUNCTION_10DOTS	0b00000100		// - Set for 5x10 font, Clear for 5x7 font

#define LCD_CGRAM			0b01000000		// Set CG ram address

#define LCD_DDRAM			0b10000000		// Set DD ram address

#define LCD_BUSY			0b10000000		// Is the LCD busy?

// #define LCD_CONTROLLER_KS0073 0  /**< Use 0 for HD44780 controller, 1 for KS0073 controller */

//#define LCD_FUNCTION_4BIT_1LINE  0x20   /* 4-bit interface, single line, 5x7 dots */
//#define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
//#define LCD_FUNCTION_8BIT_1LINE  0x30   /* 8-bit interface, single line, 5x7 dots */
//#define LCD_FUNCTION_8BIT_2LINES 0x38   /* 8-bit interface, dual line,   5x7 dots */

//#define LCD_START_LINE1  0x00     /**< DDRAM address of first char of line 1 */
//#define LCD_START_LINE2  0x40     /**< DDRAM address of first char of line 2 */
//#define LCD_START_LINE3  0x14     /**< DDRAM address of first char of line 3 */
//#define LCD_START_LINE4  0x54     /**< DDRAM address of first char of line 4 */


// The DDRAM address for the start of each line on a 20 character display
static const uint8_t PROGMEM lineStartAddr20[] = {0x00, 0x40, 0x14, 0x54 };
// The DDRAM address for the start of each line on a 16 character display
static const uint8_t PROGMEM lineStartAddr16[] = {0x00, 0x40, 0x10, 0x50 };


#define lcd_e_delay()   __asm__ __volatile__( "rjmp 1f\n 1:" );
#define lcd_e_high(display) pin_high(display->ctrlE)
#define lcd_e_low(display)  pin_low(display->ctrlE)


static uint8_t lcd_waitbusy(const HD44780* device);

static void toggle_e(const HD44780* device){
	lcd_e_high(device);
    delay_us(10);
	lcd_e_low(device);
}

// Make all databus pins into outputs
static void databus_output(const HD44780* device){
	for(uint8_t i=0; i<8; i++){
		pin_make_output(device->data[i],TRUE);
	}
}

// Make all databus pins into inputs with pullups
static void databus_input(const HD44780* device){
	for(uint8_t i=0; i<8; i++){
		pin_make_input(device->data[i],TRUE);
	}
}

static uint8_t readNibble(const HD44780* device, uint8_t pos){
	uint8_t rtn = 0;
	uint8_t mask = 1;
	for(uint8_t i=0; i<4; i++){
		if(pin_is_high(device->data[pos++])){
			// Pin is high
			rtn |= mask;
		}
		mask <<= 1;
	}
	return rtn;
}

static void writeNibble(const HD44780* device, const uint8_t data, uint8_t pos){
	uint8_t mask = 1;
	for(uint8_t i=0; i<4; i++){

		boolean val = (data & mask) ? TRUE : FALSE;
		pin_set(device->data[pos++],val);
		mask <<= 1;
	}
}

// rs = TRUE to write data, FALSE to write to command
static void lcd_write(const HD44780* device,uint8_t data,boolean rs){

    lcd_waitbusy(device);

    pin_set(device->ctrlRS, rs);
    pin_low(device->ctrlRW);		// write mode

	/* configure data pins as output */
    databus_output(device);

    if(device->fourBit){
    	// High nibble first
    	writeNibble(device,data>>4,4);
    	toggle_e(device);
    	// Low nibble last
    	writeNibble(device,data,4);
    	toggle_e(device);

    	/* all data pins high (inactive) */
    	for(uint8_t pin=4; pin<8; pin++){
    		pin_high(device->data[pin]);
    	}
    }else{
    	// Write high nibble first
    	writeNibble(device,data>>4,4);
    	// Low nibble last
    	writeNibble(device,data   ,0);
    	toggle_e(device	);
    	/* all data pins high (inactive) */
    	for(uint8_t pin=0; pin<8; pin++){
    		pin_high(device->data[pin]);
    	}
    }
}


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     TRUE: read data
                 FALSE: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/
static uint8_t lcd_read(const HD44780* device, boolean rs){
    uint8_t data=0;

    // Set rs high/low
    pin_set(device->ctrlRS, rs);	// set RS high/low
    pin_high(device->ctrlRW);		// read mode

	/* configure data pins as input */
    databus_input(device);

	lcd_e_high(device);
	lcd_e_delay();

	if(device->fourBit){
		// read 2 lots of four bits
		/* read high nibble first */
		data |= readNibble(device,4) << 4;
		lcd_e_low(device);
		lcd_e_delay();                       /* Enable 500ns low       */

		/* read low nibble */
		lcd_e_high(device);
		lcd_e_delay();
		data |= readNibble(device,4);
	}else{
		// read all 8 bits
		data |= readNibble(device,4) << 4;
		data |= readNibble(device,0);
	}

	lcd_e_low(device);

	return data;
}



static uint8_t lcd_waitbusy(const HD44780* device){
    register uint8_t c;

    /* wait until busy flag is cleared */
   while ( (c=lcd_read(device,FALSE)) & (LCD_BUSY)) {    }

    /* the address counter is updated 4us after the busy flag is cleared */
    delay_us(4);

    /* now read the address counter */
    return (lcd_read(device,FALSE));  // return address counter

}/* lcd_waitbusy */


// Covert an addr into an X,Y position
/*
static void addrToXY(const HD44780* device, uint8_t addr, uint8_t* x, uint8_t* y){
	uint8_t best=255;
	*x = *y = 0;
	for(uint8_t line = 0; line<device->_display_.rows; line++){
		uint8_t start;
		if(device->_display_.columns >= 20){
			start = pgm_read_byte(&lineStartAddr20[line]);
		}else{
			start = pgm_read_byte(&lineStartAddr16[line]);
		}

		if(addr >= start){
			uint8_t thisDiff = addr - start;
			if(line==0 || thisDiff<best){
				// best so far
				best = thisDiff;
				*x = best;
				*y = line;
			}
		}
	}
}
*/

// Send a command to the LCD
static void lcd_command(const HD44780* device, uint8_t cmd){
    lcd_write(device,cmd,FALSE);
}


// Send a data byte to the LCD
static void lcd_data(const HD44780*device,uint8_t data){
    lcd_write(device, data,TRUE);
}

static void sendRawByte(const DISPLAY*display,uint8_t data){
	lcd_data((const HD44780*)display,data);
}

// x,y are zero relative and are within range
static void gotoXY(DISPLAY* display,uint8_t x, uint8_t y){
	HD44780* device = (HD44780*) display;
	uint8_t addr;
	if(display->columns >= 20){
		addr = pgm_read_byte(&lineStartAddr20[y]);
	}else{
		addr = pgm_read_byte(&lineStartAddr16[y]);
	}

	addr += (LCD_DDRAM) + x;
	lcd_command(device,addr);
}

// Clear screen and home cursor
static void cls(DISPLAY* display){
	HD44780* device = (HD44780*) display;
	lcd_command(device,LCD_CLR);
}

// Move cursor to home position
static void home(DISPLAY*display){
	HD44780* device = (HD44780*) display;
    lcd_command(device,LCD_HOME);
}




/*************************************************************************
Display character at current cursor position
Input:    character to be displayed
Returns:  none
*************************************************************************/

/**
 * Create a custom char from a definition in program space
 */
static void customChar(DISPLAY* display, const uint8_t* pgm_def, uint8_t charNum){
	HD44780* device = (HD44780*)display;
//	uint8_t pos = lcd_waitbusy(device);
//	uint8_t x,y;
//	addrToXY(device,pos,&x,&y);

	uint8_t addr = charNum * 8;

	for(uint8_t i=0; i<8; i++,addr++){
		lcd_command(device, LCD_CGRAM | addr);  		/* set CG RAM start address 0 */
		lcd_data(device, pgm_read_byte(&pgm_def[i]));
	}

//	gotoXY(&device->_display_,x,y);
}


// Intialise the display
static void init(DISPLAY* display){
	HD44780* device = (HD44780*) display;

	// Set all control pins as outputs
	pin_make_output(device->ctrlRS,FALSE);
	pin_make_output(device->ctrlRW,FALSE);
	pin_make_output(device->ctrlE,FALSE);

	// Make all data pins as outputs
	databus_output(device);

	// Wait for 16 ms
	delay_ms(16);

	// Use 8 bit mode. Make D4 and D5 high
	pin_high(device->data[4]);
	pin_high(device->data[5]);
	toggle_e(device);
	delay_ms(5);

	// repeat last command a few times
	for(uint8_t i=0; i<2; i++){
		toggle_e(device);
		delay_us(64);
	}

	// Configure for 4 bit mode if required
	if(device->fourBit){
		pin_low(device->data[4]);
		toggle_e(device);
		delay_us(64);
	}

	// Configure the number of lines
	uint8_t cmd = LCD_FUNCTION;						// Assume 1 line display
	if(device->_display_.rows >= 2){
		cmd |= LCD_FUNCTION_2LINES;					// Set as 2 line display
	}
    lcd_command(device,cmd);

    lcd_command(device,LCD_ON);      	        	/* display off                  */
//    cls(&device->_display_);                      	/* display clear                */
    lcd_command(device, LCD_ENTRY | LCD_ENTRY_INC); /* set entry mode = increment cursor */
    lcd_command(device, LCD_ON + LCD_ON_DISPLAY);   /* display on                   */

}

static void hgraph(DISPLAY* display,uint16_t pixels,uint8_t width){
	while(width--){
		uint8_t c;
		c = (pixels>=5) ? 5 : pixels;
		pixels -= c;
		c = (c==0) ? ' ' : c-1;
		_displaySendByte(display,c);
	}

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

}


DISPLAY_CLASS const PROGMEM c_HD44780 = MAKE_DISPLAY_CLASS(&init,&cls,&home,&gotoXY, null, null, null, null, null, &sendRawByte,&hgraph,&vgraph,&customChar);
