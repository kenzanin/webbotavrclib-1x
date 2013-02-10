/*
 * $Id: quadraturex2.c,v 1.2 2010/09/08 18:26:52 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: quadraturex2.c,v $
 * Revision 1.2  2010/09/08 18:26:52  clivewebster
 * Add encoder interpolation
 *
 * Revision 1.1  2010/08/14 21:06:21  clivewebster
 * Added encoder with 2xPCINT pins giving 4 ticks per stripe
 *
 * Revision 1.4  2010/06/14 19:05:35  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/06/13 18:14:37  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2009/11/02 18:36:19  clivewebster
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
 * quadrature.c
 *
 *  Created on: 03-Apr-2009
 *      Author: Clive Webster
 */

#include "quadrature.h"
#include "../../../pinChange.h"


// Called when channel A has changed
static void __encChA(const IOPin* io,boolean val, volatile void* data){
	QUADRATURE* encoder = (QUADRATURE*)data;
	boolean chB = pin_get(encoder->channelB);
	if(val ^ chB){
		encoder->encoder.counter++;
	}else{
		encoder->encoder.counter--;
	}

	if(encoder->encoder.interpolate){
		_encoder_tick(&encoder->encoder);
	}
}

// Called when channel B has changed
static void __encChB(const IOPin* io,boolean val, volatile void* data){
	QUADRATURE* encoder = (QUADRATURE*)data;
	boolean chA = pin_get(encoder->channelA);
	if(val ^ chA){
		encoder->encoder.counter--;
	}else{
		encoder->encoder.counter++;
	}
	if(encoder->encoder.interpolate){
		_encoder_tick(&encoder->encoder);
	}
}

// Initialise the device
static void __quadrature_init(SENSOR* sensor){
	QUADRATURE* encoder = (QUADRATURE*)sensor;
	pin_make_input(encoder->channelA, FALSE);
	pin_make_input(encoder->channelB, FALSE);
	pin_change_attach(encoder->channelA, &__encChA, encoder);
	pin_change_attach(encoder->channelB, &__encChB, encoder);
}



ENCODER_CLASS const c_quadraturex2 = MAKE_ENCODER_CLASS(&__quadrature_init,&__encoder_read,0,0);

