/*
 * $Id: _pressure_common.c,v 1.4 2011/08/13 11:30:01 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _pressure_common.c,v $
 * Revision 1.4  2011/08/13 11:30:01  clivewebster
 * Return values in Pa rather than kPa
 *
 * Revision 1.3  2010/06/14 19:09:50  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2009/11/02 18:41:41  clivewebster
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
 * _pressure_common.c
 *
 *  Created on: 25-Sep-2009
 *      Author: Clive Webster
 */
#define RPRINTF_COMPLEX
#include "_pressure_common.h"
#include "../../rprintf.h"

void _pressureDump(const PRESSURE* device){
	rprintf("(Now=%ld Pa Initial=%ld Pa)",device->pa,device->paStart);
}
