/*
 * $Id: spi.h,v 1.12 2011/02/21 20:29:54 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spi.h,v $
 * Revision 1.12  2011/02/21 20:29:54  clivewebster
 * Added ATMega1284P
 *
 * Revision 1.11  2010/07/19 19:46:27  clivewebster
 * Added ATMega644
 *
 * Revision 1.10  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.9  2010/05/09 22:09:55  clivewebster
 * Add ATMega128
 *
 * Revision 1.8  2010/04/12 23:16:26  clivewebster
 * Add support for ATMega128rfa1
 *
 * Revision 1.7  2010/03/07 20:18:23  clivewebster
 * *** empty log message ***
 *
 * Revision 1.6  2010/02/17 23:33:22  clivewebster
 * Add support for ATMega1280
 *
 * Revision 1.5  2010/02/09 16:45:29  clivewebster
 * Added ATMega2561
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
 * spi.h
 *
 *  Created on: 23-Jun-2009
 *      Author: Clive Webster
 *
 *  An implementation of the SPI interface using the hardware pins MISO,MOSI,and SCLK
 *
 *
 */

#ifndef SPI_H_
#define SPI_H_

// Include stuff that is common to all SPI implementations
#include "_spi_common.h"

#if defined (__AVR_ATmega640__)
#elif defined (__AVR_ATmega644__)
#elif defined (__AVR_ATmega128__)
#elif defined (__AVR_ATmega1280__)
#elif defined (__AVR_ATmega2560__)
#elif defined (__AVR_ATmega2561__)
#elif defined (__AVR_ATmega128RFA1__)
#elif defined (__AVR_ATmega168__)
#elif defined (__AVR_ATmega8__)
#elif defined (__AVR_ATmega32__)
#elif defined (__AVR_ATmega328P__)
#elif defined (__AVR_ATmega1284P__)

#else
#    error "This device is not currently supported"
#endif


// Define the data stored for a harware spi interface
typedef struct s_spi {
	SPI_ABSTRACT_BUS _bus_;	// Include data common for ALL spi interfaces
} SPI;

// Class definition for a hardware SPI
extern const SPI_CLASS c_hw_spi;

// Constructor for a hardware SPI interface
#define MAKE_SPI(devices) {MAKE_SPI_ABSTRACT_BUS(&c_hw_spi, devices) }


#endif /* SPI_H_ */
