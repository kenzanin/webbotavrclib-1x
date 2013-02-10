/*
 * $Id: fastquadx2gen.h,v 1.3 2010/09/08 18:26:52 clivewebster Exp $
 * Revision History
 * ================
 * $Log: fastquadx2gen.h,v $
 * Revision 1.3  2010/09/08 18:26:52  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.2  2010/08/14 21:04:35  clivewebster
 * Store encoder timings
 *
 * Revision 1.1  2010/08/10 22:48:51  clivewebster
 * Created
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
 *        File: fastquadx2gen.h
 *  Created on: 2 Aug 2010
 *      Author: Clive Webster
 *
 *  Generate common code for encoders using INTa and INTb
 */

#ifdef BUILDING_LIBRARY

static FAST_QUADRATUREx2* encoder;

static void _init(SENSOR* sensor){
	encoder = (FAST_QUADRATUREx2*)sensor;

	// Make channel A an input pin with pullup
	cbi(INT_A_DDR , INT_A_PIN);						// Make input
	sbi(INT_A_PORT, INT_A_PIN);						// Turn on pullup

	// Make channel B an input pin with pullup
	cbi(INT_B_DDR , INT_B_PIN);						// Make input
	sbi(INT_B_PORT, INT_B_PIN);						// Turn on pullup

	// configure channel A interrupts for any-edge triggering
	sbi(INT_A_ISC, INT_A_ISCL);						// Set low bit
	cbi(INT_A_ISC, INT_A_ISCH);						// Clear high bit

	// configure channel B interrupts for any-edge triggering
	sbi(INT_B_ISC, INT_B_ISCL);						// Set low bit
	cbi(INT_B_ISC, INT_B_ISCH);						// Clear high bit

	// enable interrupts on channel A
	sbi(INT_A_ENABLE_PORT, INT_A_ENABLE_BIT);

	// enable interrupts on channel B
	sbi(INT_B_ENABLE_PORT, INT_B_ENABLE_BIT);

	// enable global interrupts
	sei();

}

ISR(INT_A_VECT){
	// channel A has changed
	// check the relative phase of the input channels
	// and update position accordingly
	// Read channel B first as its the next to change
	if(
			((inb(INT_B_READ) & (1<<INT_B_PIN)) == 0)
				^
			((inb(INT_A_READ) & (1<<INT_A_PIN)) == 0)
	){
		encoder->encoder.counter++;
	}else{
		encoder->encoder.counter--;
	}

	if(encoder->encoder.interpolate){
		_encoder_tick(&encoder->encoder);
	}

}

ISR(INT_B_VECT){
	// channel B has changed
	// check the relative phase of the input channels
	// and update position accordingly
	// Read Channel A first as its the next to change
	if(
			((inb(INT_A_READ) & (1<<INT_A_PIN)) == 0)
				^
			((inb(INT_B_READ) & (1<<INT_B_PIN)) == 0)
	){
		encoder->encoder.counter--;
	}else{
		encoder->encoder.counter++;
	}

	if(encoder->encoder.interpolate){
		_encoder_tick(&encoder->encoder);
	}
}
#endif
