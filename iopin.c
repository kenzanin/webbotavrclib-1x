/*
 * $Id: iopin.c,v 1.7 2010/07/01 23:45:29 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: iopin.c,v $
 * Revision 1.7  2010/07/01 23:45:29  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/11/02 18:58:48  clivewebster
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
 *
 * iopin.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "iopin.h"

/******************************************************

	Set a port pin to high

******************************************************/
void pin_high(const IOPin* io){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT port = pgm_read_word(&portDef->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PIN mask = pgm_read_byte(&io->pin);

		// make sure its an output
		if(_SFR_MEM8(ddr) & mask){
			CRITICAL_SECTION_START;
			// set the bit
			_SFR_MEM8(port) |= mask;
			CRITICAL_SECTION_END;
			// Allow any output to be reflected on the input pin
			nop();
		}else{
			setError(PIN_SET_ON_INPUT);
		}
	}
 }

/******************************************************

	Set a port pin to low

******************************************************/
void pin_low(const IOPin* io){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT port = pgm_read_word(&portDef->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PIN mask = pgm_read_byte(&io->pin);

		// make sure its an output
		if(_SFR_MEM8(ddr) & mask){
			CRITICAL_SECTION_START;
			// clear the bit
			_SFR_MEM8(port) &= ~mask;
			CRITICAL_SECTION_END;
			// Allow any output to be reflected on the input pin
			nop();
		}else{
			setError(PIN_SET_ON_INPUT);
		}
	}
}

/******************************************************

	Toggle an output pin ie if low then make high,
	if high then make low
	eg
	pin_toggle(B2); // Toggle PortB pin 2

******************************************************/
void pin_toggle(const IOPin* io) {
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT port = pgm_read_word(&portDef->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PIN mask = pgm_read_byte(&io->pin);

		// make sure its an output
		if(_SFR_MEM8(ddr) & mask){
			CRITICAL_SECTION_START;
			// toggle the bit
			_SFR_MEM8(port) ^= mask;
			CRITICAL_SECTION_END;
			// Allow any output to be reflected on the input pin
			nop();
		}else{
			setError(PIN_SET_ON_INPUT);
		}
	}
}


/******************************************************

	Read the value of an I/O pin and
	return TRUE if it is high or FALSE if low

******************************************************/
boolean pin_is_high(const IOPin* io){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT pin = pgm_read_word(&portDef->pin);
		PIN mask = pgm_read_byte(&io->pin);

		return  (_SFR_MEM8(pin) & mask) ? TRUE : FALSE;
	}
	return FALSE;
}



/******************************************************

	Set an output pin high or low
	eg
	pin_set(B2, TRUE); // To set PortB pin 2 high
******************************************************/
void pin_set(const IOPin* io, boolean val){
	if(val){
		pin_high(io);
	}else{
		pin_low(io);
	}
}







/******************************************************

	Set the pin as an input, specifying if you need
	a pullup resistor (TRUE/FALSE)

	ie pin_make_input(B2, TRUE); // to  make B2 an input with pullup
******************************************************/
void pin_make_input(const IOPin* io, boolean pullup){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PORT port = pgm_read_word(&portDef->port);
		PIN mask = pgm_read_byte(&io->pin);

		CRITICAL_SECTION_START;
			// Clear pin in DDR
			_SFR_MEM8(ddr) &= ~ mask;	// make an input pin

			if(pullup){
				_SFR_MEM8(port) |= mask; // turn on pullup if needed
			}else{
				_SFR_MEM8(port) &= ~ mask;	 // turn off pullup
			}
		CRITICAL_SECTION_END;
	}
}


/******************************************************

	Set the pin as an output pin

******************************************************/
void pin_make_output(const IOPin* io, boolean val){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PORT port = pgm_read_word(&portDef->port);
		PIN mask = pgm_read_byte(&io->pin);

		CRITICAL_SECTION_START;
			if(val){
				_SFR_MEM8(port) |= mask;
			}else{
				_SFR_MEM8(port) &= ~ mask;
			}
			_SFR_MEM8(ddr) |= mask;	// make an output pin
		CRITICAL_SECTION_END;
	}
}



/******************************************************

	Is the pin an output pin
	return TRUE if it is high or FALSE if low


******************************************************/

boolean pin_is_output(const IOPin* io){
	if(io){
		const IOPort* portDef = (const IOPort*)pgm_read_word(&io->port);
		PORT ddr = pgm_read_word(&portDef->ddr);
		PIN mask = pgm_read_byte(&io->pin);

		return (_SFR_MEM8(ddr) & mask) ? TRUE : FALSE;
	}
	return FALSE;
}

