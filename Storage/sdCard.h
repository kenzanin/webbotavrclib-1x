/*
 * $Id: sdCard.h,v 1.4 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: sdCard.h,v $
 * Revision 1.4  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/03/19 01:50:50  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2010/03/08 16:15:47  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/08 03:10:08  clivewebster
 * Added
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
 * sdCard.h
 *
 *  Created on: 02-Jul-2009
 *      Author: Clive Webster

	      _________________
	     /  1 2 3 4 5 6 78 |	<- view of SD card looking at contacts
	    / 9                |	Pins 8 and 9 are present only on SD cards and not MMC cards
	    |        SD Card   |
	    |                  |
	    /\/\/\/\/\/\/\/\/\/\
 	    1 - CS   (chip select)          - wire to any available I/O pin(*)
	    2 - DIN  (data in, card<-host)  - wire to SPI MOSI pin
	    3 - VSS  (ground)               - wire to ground
	    4 - VDD  (power)     			- wire to 3.3V power
	    5 - SCLK (data clock)           - wire to SPI SCK pin
	    6 - VSS  (ground)               - wire to ground
	    7 - DOUT (data out, card->host) - wire to SPI MISO pin

	    (*) you must define this chip select I/O pin in MAKE_SD_CARD

	    The only other parameter you need for MAKE_SD_CARD is the master spi bus that the device is
	    attached to which may be either hardware/software/UART spi implementation

		To format a card as a FAT file system then use an application like:-
		http://www.sdcard.org/consumers/formatter/

 */

#ifndef SDCARD_H_
#define SDCARD_H_

// The SD card is an spi device
#include "../_spi_common.h"
// And MAY have a FAT file system
#include "FileSystem/FAT.h"

/* Card type flags (CardType) */
#define CT_MMC				0x01			/* MMC ver 3 */
#define CT_SD1				0x02			/* SD ver 1 */
#define CT_SD2				0x04			/* SD ver 2 */
#define CT_SDC				(CT_SD1|CT_SD2)	/* Any kind of SD */
#define CT_BLOCK			0x08			/* Uses Block addressing */


typedef struct s_sd_card {
	SPI_ABSTRACT_DEVICE  _device_;
	boolean		initialised; // Is it initialised ok - return from last sdCardInit
	uint32_t	numSectors;	// Total number of sectors on the card
	uint8_t		cardType;	// The card type
} SD_CARD;


// Define a new SD Card. These use Mode 0 in MSB order
// Parameters:
// select - is the IOPin used to select the device
#define MAKE_SD_CARD(select) {MAKE_SPI_DEVICE(SPI_MODE_0,SPI_DATA_ORDER_MSB,select,0xFF),FALSE,0,0 }

// Initialise the card
// Returns FALSE if there was a problem, or TRUE if OK
boolean sdCardInit(SD_CARD* card);

// Read sectors to memory
boolean sdCardRead(SD_CARD* card,uint32_t absSector,void* dta,uint8_t numSectors);
// Write sectors from memory
boolean sdCardWrite(SD_CARD* card, uint32_t absSector,const void* dta,uint8_t numSectors);

const STORAGE_CLASS* sdCardGetStorageClass(void);

#endif /* SDCARD_H_ */
