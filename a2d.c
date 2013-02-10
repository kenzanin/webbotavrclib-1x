/*
 * $Id: a2d.c,v 1.10 2010/07/15 14:55:28 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: a2d.c,v $
 * Revision 1.10  2010/07/15 14:55:28  clivewebster
 * a2dSetReference deprecated
 *
 * Revision 1.9  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.8  2010/01/25 19:36:00  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2010/01/24 14:32:30  clivewebster
 * Remove a2dConvert10bitIO and a2dConvert8bitIO
 *
 * Revision 1.6  2009/11/02 18:51:32  clivewebster
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
 * a2d.c
 *
 *  Created on: 15-Mar-2009
 *      Author: Clive Webster
 */

#include "a2d.h"
#include "errors.h"

// A2D voltage reference select
//		*this determines what is used as the
//		full-scale voltage point for A2D conversions
#define ADC_REFERENCE_AREF		0x00	///< 0x00 -> AREF pin, internal VREF turned off
#define ADC_REFERENCE_AVCC		0x01	///< 0x01 -> AVCC pin, internal VREF turned off
#define ADC_REFERENCE_RSVD		0x02	///< 0x02 -> Reserved.   ATMega640=1.1
#define ADC_REFERENCE_INTERNAL	0x03	///< 0x03 -> Internal VREF ATMega8=2.56V, ATMega168=1.1V, ATMega32=2.56V, ATMega640=2.56V
// default value
#ifndef ADC_DEFAULT_REFERENCE
#define ADC_DEFAULT_REFERENCE			ADC_REFERENCE_AVCC
#endif

// compatibility for new Mega processors
// ADCSR hack apparently no longer necessary in new AVR-GCC
#ifdef ADCSRA
# ifndef ADCSR
#   define ADCSR	ADCSRA
# endif
#endif

#ifdef ADATE
	#define ADFR	ADATE
#endif


volatile uint8_t a2dCompleteFlag;

// turn off a2d converter
void a2dOff(void){
	cbi(ADCSR, ADIE);				// disable ADC interrupts
	cbi(ADCSR, ADEN);				// disable ADC (turn off ADC power)
}

// configure A2D converter clock division (prescaling)
void a2dSetPrescaler(ADC_PRESCALE prescale){
	ADCSR =  (ADCSR & ~ADC_PRESCALE_MASK) | (prescale & ADC_PRESCALE_MASK);
}

// configure A2D converter voltage reference
static void a2dSetReference(uint8_t ref){
	ADMUX = (ADMUX & ~ADC_REFERENCE_MASK) | ((ref<<REFS0) & ADC_REFERENCE_MASK);
}



// sets the a2d input channel
static void a2dSetChannel(ADC_CHANNEL ch){
#ifdef MUX5
	if(ch & 0x20){
    	ADCSRB |= _BV(MUX5);
	}else{
    	ADCSRB &= ~_BV(MUX5);
	}
#endif
	ADMUX = (ADMUX & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK);	// set channel
}

// start a conversion on the current a2d input channel
static void a2dStartConvert(void){
	a2dCompleteFlag = FALSE;		// clear conversion complete flag
	sbi(ADCSR, ADIF);				// clear hardware "conversion complete" flag
	sbi(ADCSR, ADSC);				// start conversion
}

static boolean a2dIsComplete(void){
	return bit_is_set(ADCSR, ADSC) ? FALSE : TRUE;
}

// Perform a 10-bit conversion
// starts conversion, waits until conversion is done, and returns result
uint16_t a2dConvert10bit(ADC_CHANNEL ch)
{
	a2dSetChannel(ch);						// set the channel
	a2dStartConvert();						// start the conversion
	while( !a2dIsComplete() );				// wait until conversion complete
	return ADC;
}

// Perform a 8-bit conversion.
// starts conversion, waits until conversion is done, and returns result
uint8_t a2dConvert8bit(ADC_CHANNEL ch){
 	uint16_t rtn = a2dConvert10bit(ch)>>2;
	return (uint8_t)rtn;
}


// Interrupt handler for ADC complete interrupt.
#ifndef ADC_vect
#  error Missing ADC_vect
#endif
ISR(ADC_vect)
{
	// set the a2d conversion flag to indicate "complete"
	a2dCompleteFlag = TRUE;
}


// initialize a2d converter. This is called automatically at startup
void __attribute__ ((constructor)) a2dInit(void){
	sbi(ADCSR, ADEN);				// enable ADC (turn on ADC power)
	cbi(ADCSR, ADFR);				// default to single sample convert mode
	a2dSetPrescaler(ADC_DEFAULT_PRESCALE);	// set default prescaler
	a2dSetReference(ADC_DEFAULT_REFERENCE);	// set default reference
	cbi(ADMUX, ADLAR);				// set to right-adjusted result

	sbi(ADCSR, ADIE);				// enable ADC interrupts
	a2dCompleteFlag = TRUE;			// not doing an a2d
	sei();							// turn on interrupts (if not already on)

	//read all ADC pins once to ensure full internal capacitor charge
	// (fixes poor ADC accuracy on first read)'
	for(int i=NUM_ADC_CHANNELS-1; i>=0; i--){
		a2dConvert10bit(ADC_NUMBER_TO_CHANNEL(i));
	}
}


