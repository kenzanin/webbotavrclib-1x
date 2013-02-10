/*
 * $Id: core.c,v 1.9 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: core.c,v $
 * Revision 1.9  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2009/11/01 20:18:57  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/10/26 18:50:49  clivewebster
 * *** empty log message ***
 *
 * Revision 1.6  2009/10/09 17:15:37  clivewebster
 * Add sysInitHardware
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
 * core.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "core.h"
#include "timer.h"


void breathe(void){
	INTERRUPTABLE_SECTION{
		nop();
		nop();
		nop();
		nop();
		nop();
		nop();
		nop();
	}
}
//  This is the main loop for the application
int main(void){
	LOOP_COUNT loopCount=0;
	TICK_COUNT loopStart;
	TICK_COUNT delay;


	// Configure the states of the initial ports according to the system ie axon.h
	configure_ports();
	registerLED();

	// Initialise the general timers
	initTimers();

	// Initialise the uarts - dont do it - causes a glitch byte to be sent to Sabertooth
//	for(uint8_t i=0;i < NUM_UARTS;i++){
//		uartInit(&(Uarts[i]) ,UART_DEFAULT_BAUD_RATE);
//	}


	// Initialise the hardware declared by the board
	sysInitHardware();
	// Initialise the hardware declared by the application
	appInitHardware();

	// Initialise the heartbeat with whatever is left
	__clockInit();

	// Initialise the software
	loopStart = clockGetus();
	delay = appInitSoftware(loopStart);

	do{
		breathe();
		if(delay!=0){
			while(!clockHasElapsed(loopStart,delay)){
				// Allow interrupts to breathe
				breathe();
			}
		}
		loopStart = clockGetus();
		delay = appControl(++loopCount,loopStart);
	}while(1);


}



void __portMaskSet(const PORT_MASK* pm){
	register PORT port    = pgm_read_word(&pm->port);
	register PIN  mask    = pgm_read_byte(&pm->mask);
	_SFR_MEM8(port) |= mask;
}

boolean __portMaskGet(const PORT_MASK* pm){
	register PORT port    = pgm_read_word(&pm->port);
	register PIN  mask    = pgm_read_byte(&pm->mask);
	return (_SFR_MEM8(port) & mask) ? TRUE : FALSE;
}

void __portMaskClear(const PORT_MASK* pm){
	register PORT port    = pgm_read_word(&pm->port);
	register PIN  mask = pgm_read_byte(&pm->mask);
	_SFR_MEM8(port) &= ~mask;
}
