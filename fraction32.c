/*
 * $Id: fraction32.c,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: fraction32.c,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2009/11/02 18:56:42  clivewebster
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
 * fraction32.c
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 */
#include "fraction.h"
#include "libdefs.h"

// Value is the value to multiply by the fraction
// Fraction is the fraction (in Progmem) where the first element is the integer element to the left of
// the decimal point, and the remainding elements are the divisors where 0 denotes the end of the list
// So multiply the value 10 by 3.5 then fraction32(10, (3,2,0} ) ie (val*3)+(val/2)
int32_t fraction32(int16_t value, const uint16_t * fraction ){
	int32_t rtn = 0;
	if(value != 0){
		uint16_t term;
		boolean isNeg = FALSE;

		// The unsigned value
		uint16_t absValue;

		if(value<0){
			int32_t tmp = value;
			tmp = 0 - tmp;
			absValue=tmp;
			isNeg=TRUE;
		}else{
			absValue = value;
		}

		// first parameter is whole number
		rtn = ((uint16_t)(pgm_read_word(fraction))) * absValue;

		// Remainder are the fraction
		do{
			fraction++;

			term= ( (uint16_t)(pgm_read_word(fraction) ));
			if(term != 0){
				uint16_t frac = absValue / term;
				rtn += frac;
			}
		}while(term!=0);

		if(isNeg){
			rtn = -rtn;
		}

	}
	return rtn;
}
