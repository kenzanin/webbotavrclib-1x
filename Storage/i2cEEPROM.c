
/*
 * $Id: i2cEEPROM.c,v 1.5 2011/07/06 00:16:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: i2cEEPROM.c,v $
 * Revision 1.5  2011/07/06 00:16:48  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.4  2010/10/01 13:16:25  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.3  2010/09/30 16:52:36  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.2  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/04/02 15:59:04  clivewebster
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
 * 
 *
 * i2cEEPROM.c
 *
 *  Created on: 27 Mar 2010
 *      Author: Clive Webster
 */

#include "i2cEEPROM.h"



uint8_t i2cEEPROM_readByte(I2C_EEPROM* eeprom, EEPROM_ADDR addr){
	uint8_t rtn;
	i2cEEPROM_readBytes(eeprom, addr, &rtn, 1);
	return rtn;
}

// Write a byte to the eeprom
void i2cEEPROM_writeByte(I2C_EEPROM* eeprom, EEPROM_ADDR addr, uint8_t data){
	i2cEEPROM_writeBytes(eeprom, addr, &data, 1);
}


static uint8_t address(I2C_EEPROM* eeprom,EEPROM_ADDR addr, uint8_t* buf){
	uint8_t rtn = eeprom->i2cInfo.addr;

	// Find any left over bits to merge into the I2C address
	EEPROM_ADDR hiBits = addr;
	for(uint8_t n = 1; n <= eeprom->addrBytes; n++){
		buf[eeprom->addrBytes -n] = hiBits & 0xff;
		hiBits >>= 8;
	}
	uint8_t mask = hiBits & 0xff;
	rtn |= (mask<<1);

	return rtn;
}

// Read a sequence of byte
void i2cEEPROM_readBytes(I2C_EEPROM* eeprom, EEPROM_ADDR addr, void* dest, size_t numBytes){
	EEPROM_ADDR src = addr % eeprom->totalBytes;	/* Put address in range				*/
	uint8_t* pos = dest;
	uint8_t  buf[4];


	while(numBytes){
		size_t offset = src % eeprom->pageSize;			/* Offset into the page						*/
		size_t bytesToRead = eeprom->pageSize - offset;	/* The number of bytes left in page 	  	*/
		if(bytesToRead > numBytes){
			bytesToRead = numBytes;						/* The number of bytes to read from page  	*/
		}
		// Create dummy i2c device
		I2C_DEVICE i2c = MAKE_I2C_DEVICE(address(eeprom,src,buf));
		i2c.bus = eeprom->i2cInfo.bus;

		// Keep sending the data as it may fail if eeprom is busy writing
		// Write the 'address' then reads the data
		while(!i2cMasterTransfer(&i2c, eeprom->addrBytes, buf, bytesToRead, pos));

		src += bytesToRead;
		pos += bytesToRead;
		numBytes -= bytesToRead;
	}
}

// Write a sequence of bytes
void i2cEEPROM_writeBytes(I2C_EEPROM* eeprom, EEPROM_ADDR addr, const void* src, size_t numBytes){
	EEPROM_ADDR dst = addr % eeprom->totalBytes;	/* Put address in range				*/
	const uint8_t* pos = src;
	uint8_t  buf[4];

	while(numBytes){
		size_t offset = dst % eeprom->pageSize;			/* Offset into the page						*/
		size_t bytesToWrite = eeprom->pageSize - offset;/* The number of bytes left in page 	  	*/
		if(bytesToWrite > numBytes){
			bytesToWrite = numBytes;					/* The number of bytes to write to the page	*/
		}

		// Create dummy i2c device
		I2C_DEVICE i2c = MAKE_I2C_DEVICE(address(eeprom,dst,buf));
		i2c.bus = eeprom->i2cInfo.bus;

		// Keep sending the data as it may fail if eeprom is busy writing
		// Writes the 'address' and then writes the data
		while(!i2cMasterSendWithPrefix(&i2c, eeprom->addrBytes, buf, bytesToWrite, pos));

		dst += bytesToWrite;
		pos += bytesToWrite;
		numBytes -= bytesToWrite;
	}
}

static boolean disk_read(void* device,uint32_t absSector,void* dta){
	I2C_EEPROM* eeprom = (I2C_EEPROM*)device;
	EEPROM_ADDR addr = absSector * 512;
	i2cEEPROM_readBytes(eeprom,addr,dta,512);
	return TRUE;
}

static boolean disk_write(void* device, uint32_t absSector,const void* dta){
	I2C_EEPROM* eeprom = (I2C_EEPROM*)device;
	EEPROM_ADDR addr = absSector * 512;
	i2cEEPROM_writeBytes(eeprom,addr,dta,512);
	return TRUE;
}

static uint32_t disk_total_sectors(void* device){
	I2C_EEPROM* eeprom = (I2C_EEPROM*)device;
	uint32_t sectors = i2cEEPROM_totalBytes(eeprom);
	sectors /= 512;
	return sectors;
}


// Create a class with the method overrides for this type of storage class
static STORAGE_CLASS PROGMEM c_i2ceeprom_disk = MAKE_STORAGE_CLASS( \
		&disk_read, \
		&disk_write, \
		&disk_total_sectors);

const STORAGE_CLASS* i2cEEPROMGetStorageClass(void){
	return &c_i2ceeprom_disk;
}

