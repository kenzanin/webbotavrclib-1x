
/*
 * $Id: core_interp.c,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: core_interp.c,v $
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2009/10/26 18:50:49  clivewebster
 * *** empty log message ***
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
 *
 * core_interp.c
 *
 *  Created on: 26-Oct-2009
 *      Author: Clive Webster
 */
#include "core.h"

/*
*	Interpolate between two signed numbers
*	value - the current value to be used
*   minVal - the minimum that 'value' can be
*   maxVal - the maximum that 'value' can be
*   minRtn - the return value if 'value = minVal'
*   maxRtn - the return value if 'value = maxVal'
*   return a value in the range minRtn to maxRtn
*/
int16_t interpolate(int16_t value, int16_t minVal, int16_t maxVal, int16_t minRtn, int16_t maxRtn){
	register int32_t  lRtnRange;
	register int32_t lValRange;
	register int32_t lRelVal;

	lRtnRange = maxRtn - minRtn;
	lValRange = maxVal - minVal;
	lRelVal = value - minVal;
	lRtnRange =  minRtn + ( lRtnRange * lRelVal / lValRange );
	return (int16_t)lRtnRange;
}

