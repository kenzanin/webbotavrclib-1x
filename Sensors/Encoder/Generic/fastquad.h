/*
 * $Id: fastquad.h,v 1.7 2010/09/08 18:26:52 clivewebster Exp $
 *
 * $Log: fastquad.h,v $
 * Revision 1.7  2010/09/08 18:26:52  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.6  2010/08/10 22:47:18  clivewebster
 * Add fastquadx2 stuff
 *
 * Revision 1.5  2010/06/14 19:05:35  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/06/14 18:25:20  clivewebster
 * Initial version
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
 *
 * fastquad.h
 *
 *  Created on: 9 Jun 2010
 *      Author: Clive Webster
 *
 *  If the encoder has a large number of ticks per revolution then the
 *  pinchange interrupts used in quadrature.h may be handled too slowly.
 *  This version is optimised for speed of the IRQ handling
 */

#ifndef FASTQUAD_H_
#define FASTQUAD_H_

#include "../_encoder_common.h"

extern const ENCODER_CLASS c_fastquad_INT0;
extern const ENCODER_CLASS c_fastquad_INT1;
extern const ENCODER_CLASS c_fastquad_INT2;
extern const ENCODER_CLASS c_fastquad_INT3;
extern const ENCODER_CLASS c_fastquad_INT4;
extern const ENCODER_CLASS c_fastquad_INT5;
extern const ENCODER_CLASS c_fastquad_INT6;
extern const ENCODER_CLASS c_fastquad_INT7;

typedef struct s_fastquad{
	ENCODER		 encoder;
	const IOPin* channelB;			// The Channel B pin
}FAST_QUADRATURE;

#define MAKE_GENERIC_FAST_QUADRATURE(channelA, channelB, inverted, numStripes,interpolate) {\
	MAKE_ENCODER_SENSOR(c_fastquad_##channelA, inverted,numStripes,interpolate), \
	channelB}

extern const ENCODER_CLASS c_fastquadx2_INT0_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT0_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT1_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT1_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT2_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT2_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT3_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT3_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT4_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT4_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT5_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT6;
extern const ENCODER_CLASS c_fastquadx2_INT5_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT6_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT6_INT7;

extern const ENCODER_CLASS c_fastquadx2_INT7_INT0;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT1;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT2;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT3;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT4;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT5;
extern const ENCODER_CLASS c_fastquadx2_INT7_INT6;

typedef struct s_fastquadx2{
	ENCODER		 encoder;
}FAST_QUADRATUREx2;


#define MAKE_GENERIC_FAST_QUADRATUREx2(channelA, channelB, inverted, numStripes,interpolate) {\
	MAKE_ENCODER_SENSOR(c_fastquadx2_ ## channelA ## _ ## channelB, inverted,2*(numStripes),interpolate) }

#endif /* FASTQUAD_H_ */
