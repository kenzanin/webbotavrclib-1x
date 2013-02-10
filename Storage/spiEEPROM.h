/*
 * $Id: spiEEPROM.h,v 1.5 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spiEEPROM.h,v $
 * Revision 1.5  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/04/02 15:59:28  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2010/03/24 19:52:33  clivewebster
 * Alpha release
 *
 * Revision 1.2  2010/03/20 00:49:34  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/19 01:50:50  clivewebster
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
 * spiEEPROM.h
 *
 *  Created on: 18-Feb-2010
 *      Author: Clive Webster
 *
 *  Support serial (SPI) EEPROMs such as
 *
 *  http://uk.farnell.com:80/jsp/search/productdetail.jsp?sku=1556152&CMP=i-bf9f-00001000
 *  Part: 25LC512-I/P
 *  Voltage: 1v8 - 5v5 5-10mA
 *  Page size: 128 byte
 *
 *
 *
 *  No validation is done that you are writing to write-protected areas.
 *
 *  If you attempt to read/write beyond the end of memory then it will wrap
 *  around to the start of memory
 *
 *  The chips are often sold as 'x kilo bits' - so you need to divide by 8 to find the
 *  number of bytes
 *
 */

#ifndef SPIEEPROM_H_
#define SPIEEPROM_H_

#include "../libdefs.h"
#include "../_spi_common.h"

// And MAY have a FAT file system
#include "FileSystem/FAT.h"


// Create the datatype for the eeprom and define a constructor
typedef struct s_spi_eeprom {
	SPI_ABSTRACT_DEVICE	_device_;		// Its an SPI device
	uint8_t		addrBytes;		// The number of bytes in the address
								// Normally 1 for up to 256 bytes
								//          2 for more than 256 bytes and up to 64K bytes
								//          3 for more than 64k and up to 16Mb
								//          4 for more than 16Mb
	size_t		pageSize;		// The page size. Normally 128 ro 256, but check datasheet
								// You can only write data to one page at a time
	EEPROM_ADDR totalBytes;		// The size of the EEPROM
	} SPI_EEPROM;

#define MAKE_SPI_EEPROM(select, addrBytes,pageSize,totalBytes) { \
	MAKE_SPI_DEVICE(SPI_MODE_0,SPI_DATA_ORDER_MSB,select,0xFF), \
	addrBytes, pageSize,totalBytes}


// Some shortcuts for Microchip technology devices
#define MAKE_SPI_EEPROM_25LC512(select)  MAKE_SPI_EEPROM(select, 2, 128,  64 * KB)
#define MAKE_SPI_EEPROM_25LC1024(select) MAKE_SPI_EEPROM(select, 3, 256, 128 * KB)

// Read a byte from the eeprom
uint8_t spiEEPROM_readByte(SPI_EEPROM* eeprom, EEPROM_ADDR addr);

// Read a sequence of byte
void spiEEPROM_readBytes(SPI_EEPROM* eeprom, EEPROM_ADDR addr, void* dest, size_t numBytes);

// Write a byte to the eeprom
void spiEEPROM_writeByte(SPI_EEPROM* eeprom, EEPROM_ADDR addr, uint8_t data);

// Write a sequence of bytes
void spiEEPROM_writeBytes(SPI_EEPROM* eeprom, EEPROM_ADDR addr, const void* src, size_t numBytes);

static __inline__ EEPROM_ADDR spiEEPROM_totalBytes(const SPI_EEPROM* eeprom){
	return eeprom->totalBytes;
}

// Get the storage class for the device to use in diskInit
const STORAGE_CLASS* spiEEPROMGetStorageClass(void);

#endif /* SPIEEPROM_H_ */
