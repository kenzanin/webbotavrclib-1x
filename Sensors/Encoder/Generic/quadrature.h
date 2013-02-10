/*
 * $Id: quadrature.h,v 1.9 2011/07/06 00:09:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: quadrature.h,v $
 * Revision 1.9  2011/07/06 00:09:51  clivewebster
 * Use correct class in MAKE_GENERIC_QUADRATUREx2
 *
 * Revision 1.8  2010/09/08 18:26:52  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.7  2010/08/14 21:06:21  clivewebster
 * Added encoder with 2xPCINT pins giving 4 ticks per stripe
 *
 * Revision 1.6  2010/06/14 19:05:35  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/06/13 18:14:37  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/11 17:17:57  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:36:19  clivewebster
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
 * quadrature.h
 *
 *  Created on: 03-Apr-2009
 *      Author: Clive Webster
 *
 *  A generic quadrature encoder.
 *  This uses two input signals called Channel A and Channel B which are 90 degrees out of phase
 *
 *  On a rising edge of Channel A then Channel B will tell us which direction we are moving in
 *
 *  These two signals can be applied to any pin that can generate pin change interrupts. This excludes
 *  the ATMega8 and ATMega32 but the ATMega168 and ATMega640 can support this on most I/O pins.
 *  Look at the device file (in the dev folder) for your processor and look for the PCINT_PINS array.
 *  This  will contain the list of IO pins that could be used.
 *
 *
 */

#ifndef QUADRATURE_H_
#define QUADRATURE_H_

#if !defined(BUILDING_LIBRARY)
// we are building the app
 #include "../../../pinChange.h"
#endif

#include "../_encoder_common.h"

extern const ENCODER_CLASS c_quadrature;
extern const ENCODER_CLASS c_quadraturex2;

typedef struct s_quadrature{
	ENCODER		 encoder;
	const IOPin* channelA;			// The Channel A pin
	const IOPin* channelB;			// The Channel B pin
}QUADRATURE;

#define MAKE_GENERIC_QUADRATURE(channelA, channelB, inverted, numStripes, interpolate) {\
	MAKE_ENCODER_SENSOR(c_quadrature, inverted,numStripes,interpolate), \
	channelA, \
	channelB}

#define MAKE_GENERIC_QUADRATUREx2(channelA, channelB, inverted, numStripes, interpolate) {\
	MAKE_ENCODER_SENSOR(c_quadraturex2, inverted,(numStripes)*2,interpolate), \
	channelA, \
	channelB}



#endif /* QUADRATURE_H_ */
