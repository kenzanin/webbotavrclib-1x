/*
 * $Id: MPX5100a.h,v 1.6 2011/08/13 11:29:21 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MPX5100a.h,v $
 * Revision 1.6  2011/08/13 11:29:21  clivewebster
 * Return values in Pa rather than kPa
 *
 * Revision 1.5  2010/06/14 19:09:50  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/01/24 17:22:24  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
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
 *
 * Pressure sensor that can be used to measure depth or altitude
 *
 * See: http://www.datasheetcatalog.org/datasheet/motorola/MPX5100D.pdf
 *
 * Requires 1 ADC channel
 *
 *  Created on: 23-Sep-2009
 *      Author: Clive Webster
 *
 *
 *  Specs summary:
 *  5v supply
 *  Power on time: 20ms
 *  Sensitivity: 45mV per kPa
 *
 *
 */

#ifndef MPX5100A_H_
#define MPX5100A_H_
#include "../_pressure_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern PRESSURE_CLASS c_MPX5100A;

typedef struct s_MPX5100A{
	PRESSURE	 pressure;
	ADC_CHANNEL  adcPin;
} MPX5100A;

#define MAKE_MPX5100A(adcPin) { \
	MAKE_PRESSURE_SENSOR(c_MPX5100A), \
	adcPin\
	}

#endif /* MPX5100A_H_ */
