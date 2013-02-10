/*
 * $Id: core_isqrt.c,v 1.1 2010/08/11 17:19:19 clivewebster Exp $
 * Revision History
 * ================
 * $Log: core_isqrt.c,v $
 * Revision 1.1  2010/08/11 17:19:19  clivewebster
 * Added in Version 1.24
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
 *        File: core_isqrt.c
 *  Created on: 11 Aug 2010
 *      Author: Clive Webster
 *
 *  Perform an integer square root without requiring the floating point library
 *
 */
#include "core.h"

uint32_t isqrt(uint32_t x){
	  register uint32_t xr;  // result register
	  register uint32_t q2;  // scan-bit register
	  register uint8_t  f;   // flag (one bit)

	  xr = 0;                     // clear result
	  q2 = 0x40000000L;           // higest possible result bit
	  do
	  {
	    if((xr + q2) <= x)
	    {
	      x -= xr + q2;
	      f = 1;                  // set flag
	    }
	    else{
	      f = 0;                  // clear flag
	    }
	    xr >>= 1;
	    if(f){
	      xr += q2;               // test flag
	    }
	  } while(q2 >>= 2);          // shift twice
	  if(xr < x){
	    return xr +1;             // add for rounding
	  }
	  else{
	    return xr;
	  }

}
