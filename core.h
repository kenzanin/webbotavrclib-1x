/*
 * $Id: core.h,v 1.8 2010/08/11 17:19:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: core.h,v $
 * Revision 1.8  2010/08/11 17:19:37  clivewebster
 * Added isqrt in Version 1.24
 *
 * Revision 1.7  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2009/11/01 20:18:57  clivewebster
 * *** empty log message ***
 *
 * Revision 1.5  2009/10/26 18:50:49  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/10/09 17:15:37  clivewebster
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
 *
 */
#ifndef _CORE_H_
#define _CORE_H_

#include "libdefs.h"


extern void sysInitHardware(void);
extern void appInitHardware(void);
extern TICK_COUNT appInitSoftware(TICK_COUNT loopStart);
extern TICK_COUNT appControl(LOOP_COUNT loopCount, TICK_COUNT loopStart);// defined by the user as their main loop
extern void configure_ports(void);		// defined by the system eg sys/axon.h
extern void registerLED(void);

//  This is the main loop for the application
int main(void);

// Define macros to help with IO where the pin is known at compile time
// These should be defined as PORT_THING and PIN_THING
// Use of these will prevent logging of input pin info

//#define PIN_HIGH(p) sbi(PORT_##p,PIN_##p)
//#define PIN_LOW(p)  cbi(PORT_##p,PIN_##p)
//#define PIN_SET(p,v)    { if(v) PIN_HIGH(p); else PIN_LOW(p);}
//#define PIN_MAKE_INPUT(p,pullup) {cbi(DDR(PORT_##p),PIN_##p); PIN_SET(p,pullup);}
//#define PIN_MAKE_OUTPUT(p) sbi(DDR(PORT_##p),PIN_##p)
//#define PIN_IS_INPUT(p) { (DDR(PORT_##p) & BV(PIN_##p)) ? FALSE: TRUE}
//#define PIN_IS_OUTPUT(p) { (DDR(PORT_##p) & BV(PIN_##p)) ? TRUE: FALSE}
//#define PIN_IS_HIGH(p) ((PIN(PORT_##p) & PIN_##p)!=0) ? TRUE : FALSE
//#define PIN_IS_LOW(p) ((PIN(PORT_##p) & PIN_##p)==0) ? TRUE : FALSE


/*
*	Interpolate between two numbers
*	value - the current value to be used
*   minVal - the minimum that 'value' can be
*   maxVal - the maximum that 'value' can be
*   minRtn - the return value if 'value = minVal'
*   maxRtn - the return value if 'value = maxVal'
*   return a value in the range minRtn to maxRtn
*/
int16_t interpolate(int16_t value, int16_t minVal, int16_t maxVal, int16_t minRtn, int16_t maxRtn);
/*
*	Interpolate between two numbers
*	value - the current value to be used
*   minVal - the minimum that 'value' can be
*   maxVal - the maximum that 'value' can be
*   minRtn - the return value if 'value = minVal'
*   maxRtn - the return value if 'value = maxVal'
*   return a value in the range minRtn to maxRtn
*/
uint16_t interpolateU(int16_t value, int16_t minVal, int16_t maxVal, uint16_t minRtn, uint16_t maxRtn);
void __portMaskSet(const PORT_MASK* pm);
void __portMaskClear(const PORT_MASK* pm);
boolean __portMaskGet(const PORT_MASK* pm);

/*
 * Calculate an integer square root without requiring
 * the floating point routines
 */
uint32_t isqrt(uint32_t x);

void breathe(void);
#endif
