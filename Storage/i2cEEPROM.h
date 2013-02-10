/*
 * $Id: i2cEEPROM.h,v 1.3 2010/09/30 16:52:36 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: i2cEEPROM.h,v $
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
 * i2cEEPROM.h
 *
 *  Created on: 25 Mar 2010
 *      Author: Clive Webster
 *
 *  An EEPROM on the I2C bus
 *
 *  Pin1: NC	Pin8: Vcc
 *  Pin2: A1	Pin7: WP (Connect to Gnd)
 *  Pin3: A2    Pin6: SCL
 *  Pin4: Gnd	Pin5: SDA
 *
 *
 *
 *  ATmel Example: http://uk.farnell.com/jsp/search/productdetail.jsp?sku=1362646&CMP=i-bf9f-00001000
 *  Supply Vcc: 2v5-5v5
 *
 *  AT24C1024B = 1MBits = 128kb, 256 byte page size @ 2 - 3 mA
 *  In this case the address is D16, + D15-0 ie 17 bits
 *  I2c Addeess = %1010abcY
 *  where a,b = A2/A1 and c=D16 of the address, Y=0/1 for read/write as usual
 *
 *  So its like 2 x 64kb EEPROMs
 *  A2  A1
 *   0   0 = one at Address C0 and the other at C2
 *   0   1 = one at Address C4 and the other at C6
 *   1   0 = one at Address C8 and the other at CA
 *   1   1 = one at Address CC and the other at CE
 *
 * Looking at other devices from ATmel then they sometimes use the A2 and A1 pins in software rather than hardware.
 * ie a device requiring a 10 bit address may put the top two bit into the 'A' fields so that the remaining address
 * is then 8 bit. So this means a given device may use all of the 'A' pins - in which case it has lots of sequential
 * I2C addresses and you can only connect one to the I2C bus !! What a painful way to do it!
 *
 */

#ifndef I2CEEPROM_H_
#define I2CEEPROM_H_

#include "../libdefs.h"
#include "../i2cBus.h"

// And MAY have a FAT file system
#include "FileSystem/FAT.h"

typedef struct s_i2c_eeprom{
	I2C_DEVICE	 i2cInfo;		// The base I2C address. eg C0 for ATmel EEPROMs
	uint8_t		addrBytes;		// The number of bytes in the address
	size_t		pageSize;		// The page size. Normally 128 ro 256, but check datasheet
								// You can only write data to one page at a time
	EEPROM_ADDR totalBytes;		// The size of the EEPROM
} I2C_EEPROM;

// Macro to create a generic EEPROM
#define MAKE_I2C_EEPROM(addr, addrb, pgsz, total) { \
	MAKE_I2C_DEVICE(addr),\
	addrb,pgsz,total}

// Params are TRUE if connected HIGH or, FALSE if connected low

// 01B = 128 bytes uses A2,A1,A0 for I2C address and 8 byte page size
#define MAKE_I2C_EEPROM_AT24C01B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	1, 8, 128}

// 04B = 512 bytes uses A2,A1 for I2C address and 16 byte page size
#define MAKE_I2C_EEPROM_AT24C04B(a2,a1) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) ), \
	1, 16, 512}

// 08B = 1024 bytes uses A2 for I2C address and 16 byte page size
#define MAKE_I2C_EEPROM_AT24C08B(a2) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) ), \
	1, 16, 1024}

// 16B = 2kbytes one device per bus I2C address and 16 byte page size
#define MAKE_I2C_EEPROM_AT24C16B() { \
	MAKE_I2C_DEVICE( 0xC0 ), \
	1, 16, 2*KB}

// 32 = 4kbytes uses A2,A1,A0 for I2C address and 32 byte page size
#define MAKE_I2C_EEPROM_AT24C32(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 32, 4*KB}
#define MAKE_I2C_EEPROM_AT24C32B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 32, 4*KB}

// 64 = 8kbytes uses A2,A1,A0 for I2C address and 32 byte page size
#define MAKE_I2C_EEPROM_AT24C64(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 32, 8*KB}
#define MAKE_I2C_EEPROM_AT24C64B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 32, 8*KB}

// 128 = 16 kbytes uses A2,A1,A0 for I2C address and 64 byte page size
#define MAKE_I2C_EEPROM_AT24C128B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 64, 16*KB}

// 256 = 32 kbytes uses A2,A1,A0 for I2C address and 64 byte page size
#define MAKE_I2C_EEPROM_AT24C256B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 64, 32*KB}

// 512 = 64 kbytes uses A2,A1,A0 for I2C address and 128 byte page size
#define MAKE_I2C_EEPROM_AT24C512B(a2,a1,a0) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) | ((a0 & 1)<<1) ), \
	2, 128, 64*KB}

// 1024B = 128KB uses A2,A1 for I2C address and 256 byte page size
#define MAKE_I2C_EEPROM_AT24C1024B(a2,a1) { \
	MAKE_I2C_DEVICE( 0xC0 | ((a2 & 1)<<3) | ((a1 & 1)<<2) ), \
	2, 256, 128*KB}


// Read a byte from the eeprom
uint8_t i2cEEPROM_readByte(I2C_EEPROM* eeprom, EEPROM_ADDR addr);

// Read a sequence of byte
void i2cEEPROM_readBytes(I2C_EEPROM* eeprom, EEPROM_ADDR addr, void* dest, size_t numBytes);

// Write a byte to the eeprom
void i2cEEPROM_writeByte(I2C_EEPROM* eeprom, EEPROM_ADDR addr, uint8_t data);

// Write a sequence of bytes
void i2cEEPROM_writeBytes(I2C_EEPROM* eeprom, EEPROM_ADDR addr, const void* src, size_t numBytes);

static __inline__ EEPROM_ADDR i2cEEPROM_totalBytes(const I2C_EEPROM* eeprom){
	return eeprom->totalBytes;
}

// Get the storage class for the device to use in diskInit
const STORAGE_CLASS* i2cEEPROMGetStorageClass(void);

#endif /* I2CEEPROM_H_ */
