/*
 * $Id: a2dReadMv.c,v 1.1 2010/07/15 14:55:41 clivewebster Exp $
 * Revision History
 * ================
 * $Log: a2dReadMv.c,v $
 * Revision 1.1  2010/07/15 14:55:41  clivewebster
 * Added in Version 1.23
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
 *        File: a2dReadMv.c
 *  Created on: 14 Jul 2010
 *      Author: Clive Webster
 *
 *  Read a voltage on an a2d input in millivolts
 *  This requires a specification of what AVcc actually is ie 100%
 */
#include "a2d.h"
#include "core.h"
uint16_t a2dReadMv(ADC_CHANNEL channel){

	uint16_t val10 = a2dConvert10bit(channel);	// 0..1023
	uint16_t avcc = a2dGetAVcc();
	return interpolateU(val10,0,1023, 0,avcc);

}


