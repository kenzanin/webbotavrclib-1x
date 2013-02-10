/*
 * $Id: _spi_common.h,v 1.8 2011/07/06 00:20:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _spi_common.h,v $
 * Revision 1.8  2011/07/06 00:20:25  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.7  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2010/05/28 18:05:09  clivewebster
 * Added functions to get/set clock, data order and mode as well as the generic device
 *
 * Revision 1.5  2010/03/08 01:43:32  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/02/21 20:06:41  clivewebster
 * Add spiReadByte
 *
 * Revision 1.3  2009/11/02 18:50:27  clivewebster
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
 * _spi_common.h
 *
 *  Created on: 23-Jun-2009
 *      Author: Clive Webster
 *
 *  The abstract class for all SPI interfaces whether they are implemented in hardware or software
 */

#ifndef _SPI_COMMON_H_
#define _SPI_COMMON_H_

#include "libdefs.h"
#include "iopin.h"

typedef enum{
	SPI_DATA_ORDER_MSB=0,	// Data is sent MSB first
	SPI_DATA_ORDER_LSB=1	// Data is sent LSB first
} SPI_DATA_ORDER;

typedef enum{
	SPI_MODE_0 = 0,	// CPOL=0, CPHA =0
	SPI_MODE_1 = 1,	// CPOL=0, CPHA =1
	SPI_MODE_2 = 2,	// CPOL=1, CPHA =0
	SPI_MODE_3 = 3	// CPOL=1, CPHA =1
} SPI_MODE;

// Set the clock speed to the CPU speed divided by 2, 4, 8 etc
typedef enum{
	SPI_CLOCK_DIV2 = 2,
	SPI_CLOCK_DIV4 = 4,
	SPI_CLOCK_DIV8 = 8,
	SPI_CLOCK_DIV16 = 16,
	SPI_CLOCK_DIV32 = 32,
	SPI_CLOCK_DIV64 = 64,
	SPI_CLOCK_DIV128 = 128
} SPI_CLOCK;


struct s_spi_abstract_bus;

// ------- Define the structure for any attached SPI Device --------
// ------- This is is for internal use only
// ------- End users should use MAKE_SPI_GENERICE_DEVICE
typedef struct s_spi_abstract_device{
	struct s_spi_abstract_bus* bus;	// The spi bus it is connected to
	SPI_MODE		mode;		// The mode used by this device - Normally SPI_MODE_0
	SPI_DATA_ORDER	order;		// The bit order used by this device - Normally SPI_DATA_ORDER_MSB
	const IOPin *   select;		// The pin used to select the device, can be null if there is only one device
	boolean			initialised;// Has the device been initialised
	uint8_t			fillerByte;	// The byte to send when just receiving - often 0xFF
} SPI_ABSTRACT_DEVICE;

#define MAKE_SPI_DEVICE(mode,order,select,filler) { null,mode,order,select,FALSE,filler}


// -------- Define the structure for a generic SPI Device -------
typedef struct s_spi_generic_device{
	SPI_ABSTRACT_DEVICE _device_;
} SPI_GENERIC_DEVICE;

#define MAKE_SPI_GENERIC_DEVICE(mode,order,select,filler) { MAKE_SPI_DEVICE(mode,order,select,filler) }



/*-------------------------------------------------------------
   Things you can do with an abstract SPI device
-------------------------------------------------------------*/
// find the bus it uses
#define spiAbstractDeviceGetBus(device) device->bus

// You can select it
void spiAbstractDeviceSelect(const SPI_ABSTRACT_DEVICE* device, boolean active);

// Send a byte with no response
void spiAbstractDeviceSendByte(const SPI_ABSTRACT_DEVICE* device, uint8_t data);

// Send a byte and receive a response
uint8_t spiAbstractDeviceXferByte(const SPI_ABSTRACT_DEVICE* device, uint8_t data);

// Get a byte from the device
uint8_t spiAbstractDeviceReceiveByte(const SPI_ABSTRACT_DEVICE* device);

// Send a 16 bit word with no response
void spiAbstractDeviceSendWord(const SPI_ABSTRACT_DEVICE* device, uint16_t data);

// Send a 16 bit word and receive a response
uint16_t spiAbstractDeviceXferWord(const SPI_ABSTRACT_DEVICE* device, uint16_t data);

// Receive a 16 bit word
uint16_t spiAbstractDeviceReceiveWord(const SPI_ABSTRACT_DEVICE* device);

// Receive a number of bytes into a buffer
void spiAbstractDeviceReceiveBytes(const SPI_ABSTRACT_DEVICE* device, void* buff, size_t size);

// Write a number of bytes from a buffer
void spiAbstractDeviceSendBytes(const SPI_ABSTRACT_DEVICE* device, const void* buff, size_t size);


/*-------------------------------------------------------------
   Things you can do with a concrete SPI device
-------------------------------------------------------------*/
// Convert a concrete SPI device into an SPI_ABSTRACT_DEVICE
#define spiGetAbstractDevice(device) (&((device)->_device_))

// Select or deselect this device
#define spiDeviceSelect(device, active) (spiAbstractDeviceSelect(spiGetAbstractDevice(device),active))

// Find the SPI_ABSTRACT_BUS for this device
#define spiDeviceGetBus(device) (spiGetAbstractDevice(device)->bus)

// Send a byte with no response
#define spiDeviceSendByte(device, data) spiAbstractDeviceSendByte(spiGetAbstractDevice(device),data)

// Send a byte and receive a response
#define spiDeviceXferByte(device, data) spiAbstractDeviceXferByte(spiGetAbstractDevice(device),data)

// Get a byte from the device
#define spiDeviceReceiveByte(device) spiAbstractDeviceReceiveByte(spiGetAbstractDevice(device))

// Send a 16 bit word with no response
#define spiDeviceSendWord(device, data) spiAbstractDeviceSendWord(spiGetAbstractDevice(device),data)

// Send a 16 bit word and receive a response
#define spiDeviceXferWord(device, data) spiAbstractDeviceXferWord(spiGetAbstractDevice(device),data)

// Get a 16 bit word from the device
#define spiDeviceReceiveWord(device) spiAbstractDeviceReceiveWord(spiGetAbstractDevice(device))

#define spiDeviceReceiveBytes(device, buff, size) spiAbstractDeviceReceiveBytes(spiGetAbstractDevice(device),buff,size)
#define spiDeviceSendBytes(device, buff, size) spiAbstractDeviceSendBytes(spiGetAbstractDevice(device),buff,size)


//----------------------------------------------------------------------------



// A datatype that store a list of pointers to SPI_ABSTRACT_DEVICEs
// eg SPI_ABSTRACT_DEVICE list[] = {
//				spiGetAbstractDevice(&dev1),
//				spiGetAbstractDevice(&dev2),
//				spiGetAbstractDevice(&dev3),
//				spiGetAbstractDevice(&dev4) };
typedef SPI_ABSTRACT_DEVICE* /*PROGMEM*/  SPI_DEVICE_LIST;

//----------------------------------------------------------------------------

//-------- Define the variables stored for each SPI bus  ---
typedef struct s_spi_abstract_bus {
	const struct /*PROGMEM*/ c_spi*	sclass;	// The type of implementation
	SPI_DEVICE_LIST* const devices;		// The list of attached devices
	size_t			numDevices;			// The number of devices
	SPI_CLOCK		clock;				// The clock value
	SPI_MODE		mode;				// The mode
	SPI_DATA_ORDER	order;				// The bit order
	boolean			initialised;		// Has it been initialised
	SPI_ABSTRACT_DEVICE* selectedDevice;// The currently selected device
} SPI_ABSTRACT_BUS;
#define MAKE_SPI_ABSTRACT_BUS(class,devices) { \
		class,devices,(uint8_t)(sizeof(devices)/sizeof(SPI_ABSTRACT_DEVICE*)), \
		SPI_CLOCK_DIV2,SPI_MODE_0,SPI_DATA_ORDER_MSB, FALSE,NULL}

//---------------- Define virtual methods for a bus ---------

typedef struct /*PROGMEM*/ c_spi{
	void     (*init)  (SPI_ABSTRACT_BUS* spi,boolean master);		// Initialise the bus
	void     (*off)   (SPI_ABSTRACT_BUS* spi);						// Turn off the bus
	void     (*clock) (SPI_ABSTRACT_BUS* spi, SPI_CLOCK clock);		// Set clock speed for bus
	void 	 (*order) (SPI_ABSTRACT_BUS* spi,SPI_DATA_ORDER order);	// Set the bit order for bus
	void 	 (*mode)  (SPI_ABSTRACT_BUS* spi,SPI_MODE mode);		// Set the SPI mode for bus
	uint8_t	 (*xfer)  (SPI_ABSTRACT_BUS* spi,uint8_t data);			// Xfer a byte over the bus
	void	 (*readMany)(const SPI_ABSTRACT_DEVICE* device,void* buf, size_t size);
																	// Read many bytes to buffer
																	// Can be null if nothing special
	void	(*writeMany)(const SPI_ABSTRACT_DEVICE* device,const void* buf, size_t size);
																	// Write many bytes from buffer
																	// Can be null if nothing special
	void	 (*sel)  (const SPI_ABSTRACT_DEVICE* device,boolean selected);// Device selection change
																	// Can be null if nothing special
} SPI_CLASS;
// Create macro for defining an SPI implementation
#define MAKE_SPI_CLASS(init, off, clock, order, mode,xfer,rmany,wmany,sel) { init, off, clock, order, mode, xfer,rmany,wmany,sel}

/*-------------------------------------------------------------
   Things you can do with an abstract SPI bus
-------------------------------------------------------------*/
void spiAbstractBusInit(SPI_ABSTRACT_BUS* bus, boolean master);
void spiAbstractBusOff(SPI_ABSTRACT_BUS* bus);
void spiAbstractBusSetMode(SPI_ABSTRACT_BUS* bus,SPI_MODE mode);
void spiAbstractBusSetDataOrder(SPI_ABSTRACT_BUS* bus,SPI_DATA_ORDER order);
void spiAbstractBusSetClock(SPI_ABSTRACT_BUS* bus,SPI_CLOCK clock);

static __inline__ SPI_MODE spiAbstractBusGetMode(const SPI_ABSTRACT_BUS* bus){
	return bus->mode;
}
static __inline__ SPI_DATA_ORDER spiAbstractBusGetDataOrder(const SPI_ABSTRACT_BUS* bus){
	return bus->order;
}
static __inline__ SPI_CLOCK spiAbstractBusGetClock(const SPI_ABSTRACT_BUS* bus){
	return bus->clock;
}

/*-------------------------------------------------------------
   Things you can do with a concrete SPI bus
-------------------------------------------------------------*/
// Convert a concrete SPI bus into an SPI_ABSTRACT_BUS
#define spiGetAbstractBus(bus) &((bus)->_bus_)

#define spiBusInit(bus,master) 	spiAbstractBusInit(spiGetAbstractBus(bus),master)
#define spiBusOff(bus) 			spiAbstractBusOff(spiGetAbstractBus(bus))

#define spiBusSetMode(bus,mode) spiAbstractBusSetMode(spiGetAbstractBus(bus),mode)
#define spiBusSetDataOrder(bus,order) spiAbstractBusSetDataOrder(spiGetAbstractBus(bus),order)
#define spiBusSetClock(bus,clock) spiAbstractBusSetClock(spiGetAbstractBus(bus),clock)

#define spiBusGetMode(bus) 		spiAbstractBusGetMode(spiGetAbstractBus(bus))
#define spiBusGetDataOrder(bus) spiAbstractBusGetDataOrder(spiGetAbstractBus(bus))
#define spiBusGetClock(bus) 	spiAbstractBusGetClock(spiGetAbstractBus(bus))


#endif /* _SPI_COMMON_H_ */
