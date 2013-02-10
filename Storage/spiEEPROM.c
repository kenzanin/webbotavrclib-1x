
/*
 * $Id: spiEEPROM.c,v 1.5 2011/07/06 00:16:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spiEEPROM.c,v $
 * Revision 1.5  2011/07/06 00:16:48  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.4  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/04/02 15:59:28  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2010/03/24 19:52:33  clivewebster
 * Alpha release
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
 * spiEEPROM.c
 *
 *  Created on: 18-Feb-2010
 *      Author: Clive Webster
 */
#include "spiEEPROM.h"
#include <stdlib.h>

// The commands that can be issued
#define SPIEEPROM_CMD_READ		0x03	// Read byte(s)
#define SPIEEPROM_CMD_WRITE		0x02	// Write byte(s)
#define SPIEEPROM_CMD_WREN		0x06	// Write Enable
#define SPIEEPROM_CMD_WRDI		0x04	// Write Disable
#define SPIEEPROM_CMD_RDSR		0x05	// Read Status Register
#define SPIEEPROM_CMD_WRSR		0x01	// Write Status Register

// status register bit defines
#define SPIEEPROM_STATUS_WIP	0x01	// Write in progress
#define SPIEEPROM_STATUS_WEL	0x02	// Write enable
#define SPIEEPROM_STATUS_BP0	0x04	// Block Proection 0
#define SPIEEPROM_STATUS_BP1	0x08	// Block Proection 1
#define SPIEEPROM_STATUS_WPEN	0x80	// Software Write Protect Enable


// Read the status register
static uint8_t spiEEPROM_readStatus(SPI_EEPROM* eeprom){

	// Select the device
//	spiDeviceSelect(eeprom,TRUE);

	// send command
	spiDeviceSendByte(eeprom,SPIEEPROM_CMD_RDSR);

	// get status register value
	uint8_t status = spiDeviceReceiveByte(eeprom);

	// Un-select the device
	spiDeviceSelect(eeprom,FALSE);
	return status;
}

// Is the chip busy doing a write?
static boolean spiEEPROM_isBusy(SPI_EEPROM* eeprom){
	uint8_t status = spiEEPROM_readStatus(eeprom);
	return (status & SPIEEPROM_STATUS_WIP) ? TRUE : FALSE;
}


// Enable writes
static void writeEnable(SPI_EEPROM* eeprom){
//	SPI_ABSTRACT_BUS* bus = __spiDeviceGetBus(eeprom->_device_.bus);

	while(spiEEPROM_isBusy(eeprom));

	// Select the device
//	spiDeviceSelect(eeprom,TRUE);

	// send command
	spiDeviceSendByte(eeprom,SPIEEPROM_CMD_WREN);

	// Un-select the device
	spiDeviceSelect(eeprom,FALSE);
}

static void address(SPI_EEPROM* eeprom, EEPROM_ADDR addr){
	for(uint8_t b = eeprom->addrBytes; b > 0; b--){
		EEPROM_ADDR shft = (addr >> (8 * (b-1)));
		spiDeviceSendByte(eeprom, (uint8_t)shft);
	}
}

uint8_t spiEEPROM_readByte(SPI_EEPROM* eeprom, EEPROM_ADDR addr){
	uint8_t rtn;
	spiEEPROM_readBytes(eeprom, addr, &rtn, 1);
	return rtn;
}

// Write a byte to the eeprom - note the write may be delayed
void spiEEPROM_writeByte(SPI_EEPROM* eeprom, EEPROM_ADDR addr, uint8_t data){
	spiEEPROM_writeBytes(eeprom, addr, &data, 1);
}

// Read a sequence of bytes
void spiEEPROM_readBytes(SPI_EEPROM* eeprom, EEPROM_ADDR addr, void* dest, size_t numBytes){
	EEPROM_ADDR src = addr % eeprom->totalBytes;	/* Put address in range				*/
	uint8_t* pos = dest;

	while(numBytes){


		size_t offset = src % eeprom->pageSize;			/* Offset into the page			*/

		while(spiEEPROM_isBusy(eeprom));				/* Wait while busy				*/
		//spiDeviceSelect(eeprom,TRUE);					/* Select the device 			*/
		spiDeviceSendByte(eeprom,SPIEEPROM_CMD_READ);	/* issue read command 			*/
		address(eeprom,src);							/* send the address				*/

		size_t bytesRead = 0;
		while(numBytes--){
			*pos++ = spiDeviceReceiveByte(eeprom);		/* read the next byte			*/
			bytesRead++;								/* one more byte read			*/
			if(++offset==eeprom->pageSize){				/* if at end of page boundary	*/
				break;
			}
		}
		spiDeviceSelect(eeprom,FALSE);					/* De-select the device			*/

		src += bytesRead;								/* update the address			*/
	}
}


// Write a sequence of bytes
void spiEEPROM_writeBytes(SPI_EEPROM* eeprom, EEPROM_ADDR addr, const void* src, size_t numBytes){
	EEPROM_ADDR dst = addr % eeprom->totalBytes;		/* Put address in range				*/
	const uint8_t* pos = src;

	while(numBytes){

		size_t offset = dst % eeprom->pageSize;			/* Offset into the page			*/

		writeEnable(eeprom);							/* Allow writes					*/
		//spiDeviceSelect(eeprom,TRUE);					/* Select the device 			*/
		spiDeviceSendByte(eeprom,SPIEEPROM_CMD_WRITE);	/* issue write command 			*/
		address(eeprom,dst);							/* send the address				*/

		size_t bytesWritten = 0;
		while(numBytes--){
			spiDeviceSendByte(eeprom,*pos++);           /* send the byte				*/
			bytesWritten++;								/* one more byte written		*/
			if(++offset==eeprom->pageSize){				/* if at end of page boundary	*/
				break;
			}
		}
		spiDeviceSelect(eeprom,FALSE);					/* De-select the device			*/

		dst += bytesWritten;							/* update the address			*/
	}

}

static boolean disk_read(void* device,uint32_t absSector,void* dta){
	SPI_EEPROM* eeprom = (SPI_EEPROM*)device;
	EEPROM_ADDR addr = absSector * 512;
	spiEEPROM_readBytes(eeprom,addr,dta,512);
	return TRUE;
}

static boolean disk_write(void* device, uint32_t absSector,const void* dta){
	SPI_EEPROM* eeprom = (SPI_EEPROM*)device;
	EEPROM_ADDR addr = absSector * 512;
	spiEEPROM_writeBytes(eeprom,addr,dta,512);
	return TRUE;
}

static uint32_t disk_total_sectors(void* device){
	SPI_EEPROM* eeprom = (SPI_EEPROM*)device;
	uint32_t sectors = spiEEPROM_totalBytes(eeprom);
	sectors /= 512;
	return sectors;
}


// Create a class with the method overrides for this type of storage class
static const STORAGE_CLASS PROGMEM c_spieeprom_disk = MAKE_STORAGE_CLASS( \
		&disk_read, \
		&disk_write, \
		&disk_total_sectors);

const STORAGE_CLASS* spiEEPROMGetStorageClass(void){
	return &c_spieeprom_disk;
}

