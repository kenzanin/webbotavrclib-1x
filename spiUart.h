/*
 * $Id: spiUart.h,v 1.5 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spiUart.h,v $
 * Revision 1.5  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/03/07 20:17:37  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/11/02 19:05:46  clivewebster
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
 * spiUart.h
 *
 *  Created on: 26-Jun-2009
 *      Author: Clive Webster
 *
 *  An SPI interface using a Uart. The Uarts only support master mode.
 *
 *  Note that not all uarts support this mode: notably the ATMega8 and ATMega32 do not
 *  support it at all but the ATMega168 and ATMega640 do.
 */

#ifndef SPIUART_H_
#define SPIUART_H_

// Include stuff that is common to all SPI implementations
#include "_spi_common.h"
#include "uart.h"

// Class definition for a uart SPI
extern const SPI_CLASS c_uart_spi;


// Define the data stored for a uart spi interface
typedef struct s_spi_uart {
	SPI_ABSTRACT_BUS _bus_;		// Include data common for ALL spi interfaces
	HW_UART*		uart;		// The UART hardware to be used
} SPI_UART;



// Constructor for a uart SPI interface
#define MAKE_UART_SPI(devices,uart) { MAKE_SPI_ABSTRACT_BUS(&c_uart_spi,devices),uart}


#endif /* SPIUART_H_ */
