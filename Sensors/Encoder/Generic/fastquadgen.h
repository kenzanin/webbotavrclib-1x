/*
 * $Id: fastquadgen.h,v 1.5 2010/09/08 18:26:52 clivewebster Exp $
 * $Log: fastquadgen.h,v $
 * Revision 1.5  2010/09/08 18:26:52  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.4  2010/08/14 21:04:35  clivewebster
 * Store encoder timings
 *
 * Revision 1.3  2010/08/10 22:46:44  clivewebster
 * Fix fastquad by reading INT pin rather that INT port
 *
 * Revision 1.2  2010/06/14 19:05:35  clivewebster
 * Add copyright license info
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
 * fastquadgen.h
 *
 *  Created on: 9 Jun 2010
 *      Author: Clive Webster
 *
 *  Code template for fastquad code generation
 */

#ifdef BUILDING_LIBRARY

static FAST_QUADRATURE* encoder;
static PORT channelB_port;
static PIN  channelB_mask;

static void _init(SENSOR* sensor){
	encoder = (FAST_QUADRATURE*)sensor;

	// Get port and mask for channel B
	const IOPin* chB = encoder->channelB;
	const IOPort* portDef = (const IOPort*)pgm_read_word(&chB->port);
	channelB_port = pgm_read_word(&portDef->pin);
	channelB_mask = pgm_read_byte(&chB->pin);


	// Make channel A an input pin with pullup
	cbi(INT_DDR , INT_PIN);						// Make input
	sbi(INT_PORT, INT_PIN);						// Turn on pullup

	// Make channel B an input pin with pullup
	pin_make_input(encoder->channelB, TRUE);

	// configure interrupts for any-edge triggering
	sbi(INT_ISC, INT_ISCL);						// Set low bit
	cbi(INT_ISC, INT_ISCH);						// Clear high bit

	// enable interrupts
	sbi(INT_ENABLE_PORT, INT_ENABLE_BIT);

	// enable global interrupts
	sei();

}

ISR(INT_VECT){
	// encoder has generated a pulse on channel A
	// check the relative phase of the input channels
	// and update position accordingly

	// Read channel B first as its the next one to change
	if(
			((_SFR_MEM8(channelB_port) & (channelB_mask)) == 0)
					^
			((inb(INT_READ) & (1<<INT_PIN)) == 0)
	   ){
		encoder->encoder.counter++;
	}else{
		encoder->encoder.counter--;
	}

	if(encoder->encoder.interpolate){
		_encoder_tick(&encoder->encoder);
	}
}
#endif
