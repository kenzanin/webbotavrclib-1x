/*
 * $Id: HMC6343.h,v 1.8 2010/09/30 16:50:17 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: HMC6343.h,v $
 * Revision 1.8  2010/09/30 16:50:17  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.7  2010/06/14 18:53:23  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/02/04 19:52:15  clivewebster
 * Added functions to set the refresh rate
 *
 * Revision 1.5  2009/12/11 17:17:57  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.4  2009/11/02 18:20:31  clivewebster
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
 *
 * HMC6343.h
 *
 *  Created on: 21-Sep-2009
 *      Author: Clive Webster
 *
 *  Compass controlled via I2C that provides bearing, roll and tilt
 *
 *  Datasheet: http://www.ssec.honeywell.com/magnetic/datasheets/HMC6343.pdf
 *  Supplier: http://www.sparkfun.com/commerce/product_info.php?products_id=8656
 *
 *  The default I2C address is 0x32 but can be changed using HMC6343_setI2Caddress
 */

#ifndef HMC6343_H_
#define HMC6343_H_

#include "../_compass_common.h"
#include "../../../i2cBus.h"

extern COMPASS_CLASS c_HMC6343;

typedef struct s_HMC6343{
	COMPASS 	 compass;
	I2C_DEVICE	  i2cInfo;
}HMC6343;

#define MAKE_HMC6343() { \
	MAKE_COMPASS_SENSOR(c_HMC6343), \
	MAKE_I2C_DEVICE(0x32) \
	}

#define MAKE_HMC6343_At(i2cAddress) { \
	MAKE_COMPASS_SENSOR(c_HMC6343), \
	MAKE_I2C_DEVICE(i2cAddress) \
	}


// --- Specific to this device ---

// The EEPROM settings
typedef struct s_HMC6343_EEPROM{
	uint8_t	slaveAddress;				// 0
	uint8_t reserved1;					// 1
	uint8_t softwareVersion;			// 2
	uint8_t reserved3;					// 3
	uint8_t opMode1;					// 4
	uint8_t opMode2;					// 5
	uint16_t serialNo;					// 6,7
	uint8_t packageYear;				// 8
	uint8_t packageWeek;				// 9
	uint16_t deviation;					// 10,11 - deviation angle +-1800 in tenths of a degree. Default=0
	uint16_t variation;					// 12,13 - variation angle +-1800 in tenths of a degree. Default=0
	uint16_t xOffset;					// 14,15 - hardiron calibration offset in x axis. Default=0
	uint16_t yOffset;					// 16,17 - hardiron calibration offset in y axis. Default=0
	uint16_t zOffset;					// 18,19 - hardiron calibration offset in z axis. Default=0
	uint16_t filter;					// 20,21 - heading IIR filter. Range 0x00-0x0f typical. Default=0
} HMC6343_EEPROM;


// Set the default refresh rates
extern void HMC6343_1Hz(HMC6343* compass);
extern void HMC6343_5Hz(HMC6343* compass);
extern void HMC6343_10Hz(HMC6343* compass);

// Read the eeprom data at the given offset
//uint8_t _HMC6343_readEEPROM_uint8t(HMC6343* device, size_t offset);
//uint8_t _HMC6343_readEEPROM_uint16t(HMC6343* device, size_t offset);

// Macro to read a field given its name in the HMC6343_EEPROM structure above
//#define HMC6343_readEEPROM_uint8t(device,field) _HMC6343_readEEPROM_uint8t(device, offsetof( ((HMC6343_EEPROM*)0)->field ))
//#define HMC6343_readEEPROM_uint16t(device,field) _HMC6343_readEEPROM_uint16t(device, offsetof( ((HMC6343_EEPROM*)0)->field ))

//#define HMC6343_softwareVersion(device) HMC6343_readEEPROM_uint8t(device,softwareVersion)
//#define HMC6343_serialNumber(device)    HMC6343_readEEPROM_uint16t(device,serialNo)

#endif /* HMC6343_H_ */
