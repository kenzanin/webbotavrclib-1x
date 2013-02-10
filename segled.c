
/*
 * $Id: segled.c,v 1.7 2010/07/01 23:53:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: segled.c,v $
 * Revision 1.7  2010/07/01 23:53:59  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/21 19:56:29  clivewebster
 * Improve handling for packed fields
 *
 * Revision 1.4  2010/01/17 02:42:28  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/12/29 12:38:33  clivewebster
 * Move short marquee calls into segledMarquee.c
 *
 * Revision 1.2  2009/12/28 19:20:56  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2009/12/27 18:23:01  clivewebster
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
 *
 *
 * segled.c
 *
 *  Created on: 27-Dec-2009
 *      Author: Clive Webster
 *
 * 				---A---
 * 				|     |
 * 				F     B
 * 				|     |
 * 				---G---
 * 				|     |
 * 				E     C
 * 				|     |
 * 				---D--- H
 *
 *
 */
#include "segled.h"
#include "libdefs.h"


static const uint8_t PROGMEM digits[] = {
	  //  ABCDEFGH
		0b11111100,	// 0
		0b01100000,	// 1
		0b11011010,	// 2
		0b11110010,	// 3
		0b01100110,	// 4
		0b10110110,	// 5
		0b00111110,	// 6
		0b11100000,	// 7
		0b11111110,	// 8
		0b11100110	// 9
};

static const uint8_t PROGMEM atoz[] = {
	  //  ABCDEFGH
		0b11101110,	// A
		0b00111110,	// B
		0b10011100, // C
		0b01111010, // D
		0b10011110, // E
		0b10001110, // F
		0b11110110, // G
		0b00101110, // H
		0b00001100, // I
		0b01111000, // J
		0b10101110, // K
		0b00011100, // L
		0b10101010, // M
		0b00101010, // N
		0b00111010, // O
		0b11001110, // P
		0b11100110, // Q
		0b00001010, // R
		0b10110110, // S
		0b00011110, // T
		0b00111000, // U
		0b01111100, // V
		0b10111000, // W
		0b01101110, // X
		0b01110110, // Y
		0b11011010  // Z
};


static void init(SEGLED* led){
	if(!led->initialised){
		for(uint8_t i=0; i<8;i++){
			const IOPin* pin = led->segment[i];
			pin_make_output(pin,FALSE);
		}
		led->initialised=TRUE;

	}
}

void segled_set(SEGLED* led, SEGLED_SEGMENT segment, boolean value){
	init(led);
	if(!led->activeHigh){
	 	value = (value) ? FALSE : TRUE;
	}
	const IOPin* pin = led->segment[segment];
	pin_set(pin,value);
}

uint8_t segled_put_char(SEGLED* led, uint8_t ch){
	init(led);
	uint8_t mask=0;	// default is a space
	if(ch>='A' && ch<='Z'){
		mask=pgm_read_byte(&atoz[ch-'A']);
	}else if(ch>='a' && ch<='z'){
		mask=pgm_read_byte(&atoz[ch-'a']);
	}else if(ch>='0' && ch<='9'){
		mask=pgm_read_byte(&digits[ch-'0']);
	}else if(ch>=0 && ch<=9){
		mask=pgm_read_byte(&digits[ch]);
	}else if(ch=='-'){
		mask = 0b00000010;
	}else if(ch=='[' || ch=='{' || ch=='('){
		mask = 0b10011100;
	}else if(ch==']' || ch=='}' || ch==')'){
		mask = 0b11110000;
	}else if(ch=='.'){
		if(led->segment[7]){
			// it has a full stop
			mask = 0b00000001;
		}else{
			// no full stop - so just light segment D
			mask = 0b00010000;
		}
	}
	if(!led->activeHigh){
	 	mask ^= 0b11111111;
	}

	for(uint8_t i=0; i<8;i++){
		const IOPin* pin = led->segment[i];
		if(mask & 0b10000000){
			pin_high(pin);
		}else{
			pin_low(pin);
		}
		mask <<= 1;
	}
	return ch;
}

