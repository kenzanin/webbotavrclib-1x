/*
 * $Id: LCD03.c,v 1.2 2011/07/06 00:12:54 clivewebster Exp $
 * Revision History
 * ================
 * $Log: LCD03.c,v $
 * Revision 1.2  2011/07/06 00:12:54  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.1  2011/03/07 01:32:51  clivewebster
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
 *        File: LCD03.c
 *  Created on: 6 Mar 2011
 *      Author: Clive Webster
 */
#include "lcd03.h"

#include "../../timer.h"

// I2C REGISTERS
//
//  0 - Read = no of bytes free in FIFO buffer, Write = Command Number
#define REG_CMD 0
//
//  Command Numbers
#define LCD_HOME 1
#define LCD_GOTO 3
#define LCD_CURSOR_OFF 4
#define LCD_CURSOR_UNDERLINE 5
#define LCD_CURSOR_BLINKING 6
#define LCD_CLS 12
#define LCD_BACKLIGHT_ON 19
#define LCD_BACKLIGHT_OFF 20
#define LCD_CUSTOM 27

// Custom chars are 128 to 135
#define CUSTOM_START 128


// Wait until it has the required amount of space
static boolean wait(LCD03_I2C* device, uint8_t bytes){
	const I2C_DEVICE* i2c = &(device->i2cInfo);

	while(device->fifo < bytes){
		// Read the amount of space in the fifo queue
		if(!i2cMasterReadRegisters(i2c,REG_CMD,1,&device->fifo)){
			return false;
		}
	}
	return true;
}


//
static void lcd_command(LCD03_I2C* device, uint8_t cmd){
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	if(wait(device,1)){
		i2cMasterWriteRegister(i2c,REG_CMD,cmd);
		if(device->fifo!=0){
			device->fifo --;
		}
	}
}


// Intialise the display
static void init(DISPLAY* display){
	LCD03_I2C* device = (LCD03_I2C*) display;
	// wait for splash screen to show
	delay_ms(500);
	// try to establish connection
	for(uint8_t i=0; i < 50; i++){
		if(wait(device,64)){
			break;
		}
		delay_ms(10);
	}
	lcd_command(device,LCD_CURSOR_OFF);
}

// Clear screen and home cursor
static void cls(DISPLAY* display){
	LCD03_I2C* device = (LCD03_I2C*) display;
	lcd_command(device,LCD_CLS);
}

// Move cursor to home position
static void home(DISPLAY*display){
	LCD03_I2C* device = (LCD03_I2C*) display;
    lcd_command(device,LCD_HOME);
}

static void sendRawByte(const DISPLAY*display,uint8_t data){
	LCD03_I2C* device = (LCD03_I2C*) display;
	lcd_command(device,data);
}

static void gotoXY(DISPLAY* display,uint8_t x, uint8_t y){
	LCD03_I2C* device = (LCD03_I2C*) display;
	if(wait(device,3)){
		lcd_command(device,LCD_GOTO);
	    sendRawByte(display,y+1);
	    sendRawByte(display,x+1);
	}
}

static void hgraph(DISPLAY* display,uint16_t pixels,uint8_t width){
	while(width--){
		uint8_t c;
		c = (pixels>=5) ? 5 : pixels;
		pixels -= c;
		c = (c==0) ? ' ' : c-1+CUSTOM_START;
		_displaySendByte(display,c);
	}
}

static void vgraph(DISPLAY* display,DISPLAY_COLUMN x,DISPLAY_COLUMN y, uint16_t pixels,uint8_t height){
	while(height--){
		_displayGoto(display,x,y+height);
		uint8_t c;
		c = (pixels>=8) ? 8 : pixels;
		pixels -= c;
		c = (c==0) ? ' ' : c-1+CUSTOM_START;
		_displaySendByte(display,c);
	}
}

static void backlight(DISPLAY* display,boolean on){
	LCD03_I2C* device = (LCD03_I2C*) display;
    lcd_command(device, (on) ? LCD_BACKLIGHT_ON : LCD_BACKLIGHT_OFF);
}

static void customChar(DISPLAY* display, const uint8_t* pgm_def, uint8_t charNum){
	LCD03_I2C* device = (LCD03_I2C*) display;

	if(wait(device,10)){
		lcd_command(device, LCD_CUSTOM);
		sendRawByte(display, CUSTOM_START + charNum);
		for(uint8_t i=0; i<8; i++){
			sendRawByte(display, CUSTOM_START | pgm_read_byte(&pgm_def[i]));
		}
	}
}

DISPLAY_CLASS const PROGMEM c_lcd03_i2c = MAKE_DISPLAY_CLASS(&init,&cls,&home,&gotoXY, null, null, &backlight, null, null, &sendRawByte,&hgraph,&vgraph,&customChar);


