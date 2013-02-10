/*
 * $Id: SOMO14D.c,v 1.1 2010/07/15 14:36:22 clivewebster Exp $
 * Revision History
 * ================
 * $Log: SOMO14D.c,v $
 * Revision 1.1  2010/07/15 14:36:22  clivewebster
 * Added in Version 1.23
 *
 * ================
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
 *        File: SOMO14D.c
 *  Created on: 6 Jul 2010
 *      Author: Clive Webster
 */
#include "SOMO14D.h"
#include "../timer.h"

static void somo14D_send(const SOMO14D* device, uint16_t cmd){
	// Set CLK low and wait 2ms = START bit
	pin_low(device->clk);
	delay_ms(2);

	// Send all 16 bits
	for(uint8_t bit=0; bit<16; bit++){
		// Wait for 100us with Clk low
		pin_low(device->clk);
		delay_us(100);

		// set data bit
		if(cmd & 0x8000U){
			pin_high(device->data);
		}else{
			pin_low(device->data);
		}

		// Wait for 100us with Clk high
		pin_high(device->clk);
		delay_us(100);

		cmd <<= 1;
	}

	// Set CLK high and wait 2ms before next command = STOP bit
	delay_ms(2);
}

void somo14D_init(const SOMO14D* device){
	pin_make_output(device->clk,TRUE);		// set CLK high
	pin_make_output(device->data,FALSE);
	if(device->busy){
		pin_make_input(device->busy,FALSE);
	}

	// Wait for 300ms before using
	delay_ms(300);

	// Make sure nothing is playing and set volume to max
	somo14D_stop(device);
	somo14D_volume(device,7);
}

void somo14D_play(const SOMO14D* device, uint16_t fileNum){
	fileNum = CLAMP(fileNum,0,511);
	somo14D_send(device,fileNum);
}
void somo14D_stop(const SOMO14D* device){
	somo14D_send(device,0xffffU);
}
void somo14D_volume(const SOMO14D* device, uint8_t volume){
	volume = CLAMP(volume,0,7);
	somo14D_send(device,volume | 0xfff0U);
}
boolean somo14D_isBusy(const SOMO14D* device){
	boolean rtn = FALSE;
	if(device->busy){
		rtn = pin_is_high(device->busy);
	}
	return rtn;
}

