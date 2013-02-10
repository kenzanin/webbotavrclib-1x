/*
 * $Id: pinChange.c,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: pinChange.c,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/11/02 19:00:56  clivewebster
 * Added revision log
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
 * pinChange.c
 *
 *  Created on: 03-Apr-2009
 *      Author: Clive Webster
 */
#include "pinChange.h"
#include <avr/interrupt.h>


// Convert an IOPin to the PCINT it uses or -1 if it doesn't have one
int __io_to_pc_index(const IOPin* io){
	for(int i=0; i<NUM_PCINT_PINS;i++){
		const IOPin* aPin = (const IOPin*)pgm_read_word(&PCINT_PINS[i]);
		if(aPin!=null && aPin==io){
			return i;
		}
	}
	return -1;
}

void pin_change_attach(const IOPin* io,PinChangeCallback callback, void* data){
	cli();
	int inx = __io_to_pc_index(io);
	if(inx==-1){
		setError(PIN_HAS_NO_PCINT);
	}else if(pcCallbacks[inx].callback!=null){
		setError(PIN_HAS_CALLBACK);
	}else{
		pcCallbacks[inx].callback = callback;
		pcCallbacks[inx].data = data;
		pcCallbacks[inx].lastValue = pin_is_high(io);
		if(inx<=7){
#ifdef PCMSK0
			PCMSK0 |= BV(inx);		// enable iterrupts for this pin
			PCICR  |= BV(PCIE0);	// enable iterrupts for PCINT0..7
#endif
#ifdef PCMSK1
		}else if(inx<=15){
			PCMSK1 |= BV((inx & 7));// enable iterrupts for this pin
			PCICR  |= BV(PCIE1);	// enable iterrupts for PCINT8..15
#endif
#ifdef PCMSK2
		}else if(inx<=23){
			PCMSK2 |= BV((inx & 7));// enable iterrupts for this pin
			PCICR  |= BV(PCIE2);	// enable iterrupts for PCINT16...23
#endif
#ifdef PCMSK3
		}else if(inx<=31){
			PCMSK3 |= BV((inx & 7));// enable iterrupts for this pin
			PCICR  |= BV(PCIE3);	// enable iterrupts for PCINT24..31
#endif
		}
	}
	sei();						// make sure interrupts are enabled
}

void pin_change_dettach(const IOPin* io){
	CRITICAL_SECTION_START;
	int inx = __io_to_pc_index(io);
	if(inx!=-1){
		pcCallbacks[inx].callback = null;
		pcCallbacks[inx].data = null;
		if(inx<=7){
#ifdef PCMSK0
			PCMSK0 &= ~BV(inx);		// disable iterrupts for this pin
#endif
#ifdef PCMSK1
		}else if(inx<=15){
			PCMSK1 &= ~BV((inx & 7));// disable iterrupts for this pin
#endif
#ifdef PCMSK2
		}else if(inx<=23){
			PCMSK2 &= ~BV((inx & 7));// disable iterrupts for this pin
#endif
#ifdef PCMSK3
		}else if(inx<=31){
			PCMSK3 &= ~BV((inx & 7));// disable iterrupts for this pin
#endif
		}
	}
	CRITICAL_SECTION_END;
}

void pin_change_service(int inx, uint8_t mask){

	for(int i=0; i<8; i++,inx++){
		if(mask & 1){
			PIN_CHANGE* pc = &pcCallbacks[inx];
			const IOPin* io = (const IOPin*)pgm_read_word(&PCINT_PINS[inx]);
			if(io!=null && pc->callback!=null){
				boolean newVal = pin_get(io);
				if(newVal != pc->lastValue){
					// The value has changed
					pc->callback(io,newVal,pc->data); 		// call the callback routine
					pc->lastValue = pin_get(io);	// set the last value
				}
			}
		}
		mask>>=1;
	}
}

#ifdef PCINT0_vect
ISR(PCINT0_vect){
	// Handle it
	pin_change_service(0,PCMSK0);
}
#endif

#ifdef PCINT1_vect
ISR(PCINT1_vect){
	// Handle it
	pin_change_service(8,PCMSK1);
}
#endif

#ifdef PCINT2_vect
ISR(PCINT2_vect){
	// Handle it
	pin_change_service(16,PCMSK2);
}
#endif

#ifdef PCINT3_vect
ISR(PCINT3_vect){
	// Handle it
	pin_change_service(24,PCMSK3);
}
#endif
