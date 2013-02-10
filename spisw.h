/*
 * $Id: spisw.h,v 1.6 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spisw.h,v $
 * Revision 1.6  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/03/07 20:17:11  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/11/02 19:05:46  clivewebster
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
 * spisw.h
 *
 *  Created on: 24-Jun-2009
 *      Author: Clive Webster
 *
 *  An implementation of the SPI interface using software
 *  where any pins can be used for MOSI,MISO,SCLK etc
 *
 *  Currently only master mode is supported
 */

#ifndef SPISW_H_
#define SPISW_H_

// Include stuff that is common to all SPI implementations
#include "_spi_common.h"
//#include "pinChange.h"		// Define the pin change callbacks - for slave mode

// Define the data stored for a harware spi interface
typedef struct s_spi_sw {
	SPI_ABSTRACT_BUS _bus_;		// Include data common for ALL spi interfaces
	const IOPin*	MOSI;		// The pins to use for the bus
	const IOPin*	MISO;		// The pins to use for the bus
	const IOPin*	SCLK;		// The pins to use for the bus
} SPI_SW;

// Class definition for a hardware SPI
extern const SPI_CLASS c_sw_spi;

// Constructor for a hardware SPI interface
#define MAKE_SW_SPI(devices,mosi,miso,sclk) {MAKE_SPI_ABSTRACT_BUS(&c_sw_spi,devices),mosi,miso,sclk}



#endif /* SPISW_H_ */
