/*
 * $Id: fastquad7.c,v 1.4 2010/08/10 22:48:09 clivewebster Exp $
 * $Log: fastquad7.c,v $
 * Revision 1.4  2010/08/10 22:48:09  clivewebster
 * Put hardware INT info into seperate INTx.h files
 *
 * Revision 1.3  2010/07/19 19:53:48  clivewebster
 * Added ATMega644
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
 * fastquad6.c
 *
 *  Created on: 9 Jun 2010
 *      Author: Clive Webster
 *
 *  INT7 service routine
 *
 */
#include "fastquad.h"
#include "../../../iopin.h"
#include "int7.h"

#if defined(VECT_INT7)
#define INT_VECT 		VECT_INT7
#define INT_DDR  		DDR_INT7
#define INT_PORT  		PORT_INT7
#define INT_PIN  		PIN_INT7
#define INT_READ  		READ_INT7
#define INT_ISC			ISC_INT7
#define INT_ISCL		ISCL_INT7
#define INT_ISCH		ISCH_INT7
#define INT_ENABLE_PORT	ENABLE_PORT_INT7
#define INT_ENABLE_BIT	ENABLE_BIT_INT7
#include "fastquadgen.h"
#else
// Not available
static void _init(SENSOR* sensor){}
#endif

ENCODER_CLASS c_fastquad_INT7 = MAKE_ENCODER_CLASS(&_init,&__encoder_read,0,0);




