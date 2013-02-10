/*
 * $Id: fraction.h,v 1.3 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: fraction.h,v $
 * Revision 1.3  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:56:42  clivewebster
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
 * fraction.h
 *
 *  Created on: 02-Sep-2009
 *      Author: Clive Webster
 *
 *  Perform fraction arithmetic
 *
 *  Passed in fraction series are assumed to be a fibonacci sequence ie the numerators are all equal to 1
 *
 */

#ifndef FRACTION_H_
#define FRACTION_H_

#include "libdefs.h"

// Value is the value to multiply by the fraction
// Fraction is the fraction (in Progmem) where the first element is the integer element to the left of
// the decimal point, and the remainding elements are the divisors where 0 denotes the end of the list
// So multiply the value 10 by 3.5 then fraction32(10, (3,2,0} ) ie (val*3)+(val/2)
int32_t fraction32(int16_t value, const uint16_t * fraction );


#endif /* FRACTION_H_ */
