/*
 * $Id: a2d.h,v 1.8 2010/07/15 14:54:45 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: a2d.h,v $
 * Revision 1.8  2010/07/15 14:54:45  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/01/25 19:35:59  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2010/01/24 17:49:06  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/01/24 14:32:30  clivewebster
 * Remove a2dConvert10bitIO and a2dConvert8bitIO
 *
 * Revision 1.3  2009/11/02 18:51:32  clivewebster
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
 */
#ifndef _A2D_H_
#define _A2D_H_

#ifdef A2D_H
#error Do not include a2d.h from AVRlib
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include "libdefs.h"
#include "iopin.h"

// defines
// A2D clock prescaler select
//		*selects how much the CPU clock frequency is divided
//		to create the A2D clock frequency
//		*lower division ratios make conversion go faster
//		*higher division ratios make conversions more accurate
typedef enum {
	ADC_PRESCALE_DIV2 = 0,	//< 0x01,0x00 -> CPU clk/2
	ADC_PRESCALE_DIV4 = 2,  //< 0x02 -> CPU clk/4
	ADC_PRESCALE_DIV8,		//< 0x03 -> CPU clk/8
	ADC_PRESCALE_DIV16,		//< 0x04 -> CPU clk/16
	ADC_PRESCALE_DIV32,		//< 0x05 -> CPU clk/32
	ADC_PRESCALE_DIV64,		//< 0x06 -> CPU clk/64
	ADC_PRESCALE_DIV128		//< 0x07 -> CPU clk/128
} ADC_PRESCALE;
#define ADC_PRESCALE_MASK		0x07

// default value
#ifndef ADC_DEFAULT_PRESCALE
#define ADC_DEFAULT_PRESCALE			ADC_PRESCALE_DIV64
#endif



// do not change the mask value
#define ADC_REFERENCE_MASK		0xC0

// bit mask for A2D channel multiplexer
#define ADC_MUX_MASK			0x1F

// channel defines (for reference and use in code)
// these channels supported by all AVRs with A2D
#define ADC_CH_ADC0				0x00
#define ADC_CH_ADC1				0x01
#define ADC_CH_ADC2				0x02
#define ADC_CH_ADC3				0x03
#define ADC_CH_ADC4				0x04
#define ADC_CH_ADC5				0x05
#define ADC_CH_ADC6				0x06
#define ADC_CH_ADC7				0x07

// ATMega8     0x0e=1.3V,  0x0f=AGND
// ATMega168   0x0e=1.1V   0x0f=AGND
// ATMega32    0x1e=1.22V, 0x1f=AGND
// ATMega640   0x1e=1.1V,  0x1f=AGND
//#define ADC_CH_122V				0x1E	///< 1.22V voltage reference
//#define ADC_CH_AGND				0x1F	///< AGND



#ifdef MUX4
// ATMega32 and ATMega640
// differential with gain
#define ADC_CH_0_0_DIFF10X		0x08
#define ADC_CH_1_0_DIFF10X		0x09
#define ADC_CH_0_0_DIFF200X		0x0A
#define ADC_CH_1_0_DIFF200X		0x0B
#define ADC_CH_2_2_DIFF10X		0x0C
#define ADC_CH_3_2_DIFF10X		0x0D
#define ADC_CH_2_2_DIFF200X		0x0E
#define ADC_CH_3_2_DIFF200X		0x0F

// differential
#define ADC_CH_0_1_DIFF1X		0x10
#define ADC_CH_1_1_DIFF1X		0x11
#define ADC_CH_2_1_DIFF1X		0x12
#define ADC_CH_3_1_DIFF1X		0x13
#define ADC_CH_4_1_DIFF1X		0x14
#define ADC_CH_5_1_DIFF1X		0x15
#define ADC_CH_6_1_DIFF1X		0x16
#define ADC_CH_7_1_DIFF1X		0x17

#define ADC_CH_0_2_DIFF1X		0x18
#define ADC_CH_1_2_DIFF1X		0x19
#define ADC_CH_2_2_DIFF1X		0x1A
#define ADC_CH_3_2_DIFF1X		0x1B
#define ADC_CH_4_2_DIFF1X		0x1C
#define ADC_CH_5_2_DIFF1X		0x1D
#endif


#ifdef MUX5
// ATMega640
#define ADC_CH_ADC8				0x20
#define ADC_CH_ADC9				0x21
#define ADC_CH_ADC10			0x22
#define ADC_CH_ADC11			0x23
#define ADC_CH_ADC12			0x24
#define ADC_CH_ADC13			0x25
#define ADC_CH_ADC14			0x26
#define ADC_CH_ADC15			0x27
#define ADC_CH_8_8_DIFF10X		0x28
#define ADC_CH_9_8_DIFF10X		0x29
#define ADC_CH_8_8_DIFF200X		0x2A
#define ADC_CH_9_8_DIFF200X		0x2B
#define ADC_CH_10_10_DIFF10X	0x2C
#define ADC_CH_11_10_DIFF10X	0x2D
#define ADC_CH_10_10_DIFF200X	0x2E
#define ADC_CH_11_10_DIFF200X	0x2F
#define ADC_CH_8_9_DIFF1X		0x30
#define ADC_CH_9_9_DIFF1X		0x31
#define ADC_CH_10_9_DIFF1X		0x32
#define ADC_CH_11_9_DIFF1X		0x33
#define ADC_CH_12_9_DIFF1X		0x34
#define ADC_CH_13_9_DIFF1X		0x35
#define ADC_CH_14_9_DIFF1X		0x36
#define ADC_CH_15_9_DIFF1X		0x37
#define ADC_CH_8_10_DIFF1X		0x38
#define ADC_CH_9_10_DIFF1X		0x39
#define ADC_CH_10_10_DIFF1X		0x3A
#define ADC_CH_11_10_DIFF1X		0x3B
#define ADC_CH_12_10_DIFF1X		0x3C
#define ADC_CH_13_10_DIFF1X		0x3D
#define ADC_NUMBER_TO_CHANNEL(x) ( ((x)>=8) ? (x)-8+ADC_CH_ADC8 : (x) )
#else
#define ADC_NUMBER_TO_CHANNEL(x) (x)
#endif



// Variables defined by sys/*.h or by Project Designer
extern const uint8_t NUM_ADC_CHANNELS;
extern const uint16_t AVcc_MV;

// functions

// turn off a2d converter
void a2dOff(void);

// configure A2D converter clock division (prescaling)
void a2dSetPrescaler(ADC_PRESCALE prescale);

// initialize a2d converter
void a2dInit(void);

// Perform a 10-bit conversion for a given channel
// starts conversion, waits until conversion is done, and returns result
uint16_t a2dConvert10bit(ADC_CHANNEL channel);

// Perform a 8-bit conversion for a given channel
// starts conversion, waits until conversion is done, and returns result
uint8_t a2dConvert8bit(ADC_CHANNEL channel);

// Get the input as a percentage from 0 to 100 % of the possible input (0...AVcc)
static __inline__ uint8_t a2dReadPercent(ADC_CHANNEL channel){
	return ((100U*a2dConvert8bit(channel))+128U)/256U;
}

// Read the input voltage in mV ie 1000 = 1V based on  AVcc
uint16_t a2dReadMv(ADC_CHANNEL channel);

// Get AVcc in mV. This is constant.
static __inline__ uint16_t a2dGetAVcc(void){
	return pgm_read_word(&AVcc_MV);
}


#endif
