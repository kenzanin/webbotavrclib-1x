/*
 * $Id: _spi_common.c,v 1.8 2011/07/06 00:20:25 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _spi_common.c,v $
 * Revision 1.8  2011/07/06 00:20:25  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.7  2010/07/01 23:50:27  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.6  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/05/09 22:06:00  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2010/03/07 20:18:32  clivewebster
 * *** empty log message ***
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
 * _spi_common.c
 *
 *  Created on: 23-Jun-2009
 *      Author: Clive Webster
 */
#include "_spi_common.h"


//--------------- Implement the virtual functions for an SPI_ABSTRACT_DEVICE -----

void spiAbstractDeviceSelect(const SPI_ABSTRACT_DEVICE* device, boolean active){
	SPI_ABSTRACT_BUS* bus = spiAbstractDeviceGetBus(device);

	if(bus){
		const SPI_CLASS* class = bus->sclass;
		void (*fn)(const SPI_ABSTRACT_DEVICE*,boolean) = (void (*)(const SPI_ABSTRACT_DEVICE*, boolean))pgm_read_word(&class->sel);


		if(active){
			// Deselect the current device
			if(bus->selectedDevice!=null){
				spiAbstractDeviceSelect(bus->selectedDevice,FALSE);
			}

			// Now set the mode and order for the bus for this device
			if(bus->mode!=device->mode){
				spiAbstractBusSetMode(bus,device->mode);
			}
			if(bus->order!=device->order){
				spiAbstractBusSetDataOrder(bus,device->order);
			}

			// Tell the bus the selected device is changing
			if(fn){
				fn(device,TRUE);
			}

			// Select the chip
			bus->selectedDevice=(SPI_ABSTRACT_DEVICE*)device;
			if(device->select!=null){
				pin_low(device->select);
			}
		}else{
			// Deselect the chip
			if(device->select!=null){
				pin_high(device->select);
			}

			// If its the currently selected device then notify it
			if(bus->selectedDevice==device){
				bus->selectedDevice = NULL;
				// Tell the bus the device is no longer selected
				if(fn){
					fn(device,FALSE);
				}
			}
		}
	}else{
		setError(SPI_BUS_NOT_INITIALIZED);
	}

}

// Send a byte with no response
void spiAbstractDeviceSendByte(const SPI_ABSTRACT_DEVICE* device, uint8_t data){
	spiAbstractDeviceXferByte(device, data);
}

// Send a byte and receive a response
uint8_t spiAbstractDeviceXferByte(const SPI_ABSTRACT_DEVICE* device, uint8_t data){
	SPI_ABSTRACT_BUS* bus = spiAbstractDeviceGetBus(device);
	uint8_t rtn;
	if(bus){
		// Select the device if required
		if(bus->selectedDevice != device){
			spiAbstractDeviceSelect(device,TRUE);
		}

		const SPI_CLASS* class = bus->sclass;
		uint8_t (*fn)(SPI_ABSTRACT_BUS*,uint8_t) = (uint8_t (*)(SPI_ABSTRACT_BUS*, uint8_t))pgm_read_word(&class->xfer);
		rtn = fn(bus,data);
	}else{
		setError(SPI_BUS_NOT_INITIALIZED);
		rtn = 0;
	}
	return rtn;
}

// Get a byte from the device
uint8_t spiAbstractDeviceReceiveByte(const SPI_ABSTRACT_DEVICE* device){
	return spiAbstractDeviceXferByte(device, device->fillerByte);
}

// Get a word from the device
uint16_t spiAbstractDeviceReceiveWord(const SPI_ABSTRACT_DEVICE* device){
	uint16_t xmit = device->fillerByte;
	xmit <<= 8;
	xmit |= device->fillerByte;

	return spiAbstractDeviceXferWord(device, xmit);
}

void spiAbstractDeviceReceiveBytes(const SPI_ABSTRACT_DEVICE* device, void* buff, size_t size){
	if(size){
		// Select the device if required
		SPI_ABSTRACT_BUS* bus = spiAbstractDeviceGetBus(device);

		if(bus){
			if(bus->selectedDevice != device){
				spiAbstractDeviceSelect(device,TRUE);
			}
			const SPI_CLASS* class = bus->sclass;
			void (*fn)(const SPI_ABSTRACT_DEVICE*,void*,size_t) = (void (*)(const SPI_ABSTRACT_DEVICE*, void*,size_t))pgm_read_word(&class->readMany);

			if(fn){
				// It has a special method for receiving many bytes in a row
				fn(device,buff,size);
			}else{
				// No special routine so just receive each in turn

				// Get the routine for xfering one byte
				const SPI_CLASS* class = bus->sclass;
				uint8_t (*fn)(SPI_ABSTRACT_BUS*,uint8_t) = (uint8_t (*)(SPI_ABSTRACT_BUS*, uint8_t))pgm_read_word(&class->xfer);
				uint8_t* dta = (uint8_t*)buff;
				while(size--){
					*dta++ = fn(bus,device->fillerByte);
				}
			}
		}else{
			setError(SPI_BUS_NOT_INITIALIZED);
		}

	}
}
void spiAbstractDeviceSendBytes(const SPI_ABSTRACT_DEVICE* device, const void* buff, size_t size){
	if(size){
		// Select the device if required
		SPI_ABSTRACT_BUS* bus = spiAbstractDeviceGetBus(device);
		if(bus){
			if(bus->selectedDevice != device){
				spiAbstractDeviceSelect(device,TRUE);
			}
			const SPI_CLASS* class = bus->sclass;
			void (*fn)(const SPI_ABSTRACT_DEVICE*,const void*,size_t) = (void (*)(const SPI_ABSTRACT_DEVICE*, const void*,size_t))pgm_read_word(&class->writeMany);

			if(fn){
				// It has a special method for writing many bytes in a row
				fn(device,buff,size);
			}else{
				// No special routine so just write each in turn

				// Get the routine for xfering one byte
				const SPI_CLASS* class = bus->sclass;
				uint8_t (*fn)(SPI_ABSTRACT_BUS*,uint8_t) = (uint8_t (*)(SPI_ABSTRACT_BUS*, uint8_t))pgm_read_word(&class->xfer);
				const uint8_t* dta = (const uint8_t*)buff;
				while(size--){
					fn(bus,*dta++);
				}
			}
		}else{
			setError(SPI_BUS_NOT_INITIALIZED);
		}
	}
}


//-------------- Implement common methonds
// Send a 16 bit word with no response
void spiAbstractDeviceSendWord(const SPI_ABSTRACT_DEVICE* device, uint16_t data){
	spiAbstractDeviceSendByte(device, ((data>>8) & 0xFF) );
	spiAbstractDeviceSendByte(device, (data & 0xFF) );
}

// Send a 16 bit word and receive a response
uint16_t spiAbstractDeviceXferWord(const SPI_ABSTRACT_DEVICE* device, uint16_t data){
	// Send MSB
	uint16_t rxData = spiAbstractDeviceXferByte( device, ((data>>8) & 0x00FF) )<<8;
	// send LSB
	rxData |= spiAbstractDeviceXferByte( device, data & 0x00FF) ;
    return rxData;
}

//--------------- Implement the virtual functions for an SPI_ABSTRACT_BUS -----
void spiAbstractBusInit(SPI_ABSTRACT_BUS* spi, boolean master){
	if(spi && !spi->initialised){
		const SPI_CLASS* class = spi->sclass;
		void (*fn)(SPI_ABSTRACT_BUS*,boolean) = (void (*)(SPI_ABSTRACT_BUS*, boolean))pgm_read_word(&class->init);
		for(int deviceNo=0;deviceNo < spi->numDevices; deviceNo++){
			SPI_ABSTRACT_DEVICE* device = (SPI_ABSTRACT_DEVICE *)pgm_read_word(&spi->devices[deviceNo]);
			device->bus = (struct s_spi_abstract_bus*)spi;
			if(device->select!=null){
				// Make into output pin and deselect
				pin_make_output(device->select, TRUE);
			}
		}
		spi->selectedDevice=null;
		fn(spi,master);				// initialise the bus
		spi->initialised = TRUE;
	}
}
void spiAbstractBusOff(SPI_ABSTRACT_BUS* spi){
	if(spi && spi->initialised){
		const SPI_CLASS* class = spi->sclass;
		void (*fn)(SPI_ABSTRACT_BUS*) = (void (*)(SPI_ABSTRACT_BUS*))pgm_read_word(&class->off);
		fn(spi);
		spi->initialised = FALSE;
	}
}

void spiAbstractBusSetMode(SPI_ABSTRACT_BUS* bus,SPI_MODE mode){
	if(bus){
		bus->mode = mode;
		const SPI_CLASS* class = bus->sclass;
		void (*fn)(SPI_ABSTRACT_BUS*,SPI_MODE mode) = (void (*)(SPI_ABSTRACT_BUS*,SPI_MODE mode))pgm_read_word(&class->mode);
		fn(bus,mode);
	}
}
void spiAbstractBusSetDataOrder(SPI_ABSTRACT_BUS* bus,SPI_DATA_ORDER order){
	if(bus){
		bus->order = order;
		const SPI_CLASS* class = bus->sclass;
		void (*fn)(SPI_ABSTRACT_BUS*,SPI_DATA_ORDER order) = (void (*)(SPI_ABSTRACT_BUS*,SPI_DATA_ORDER order))pgm_read_word(&class->order);
		fn(bus,order);
	}
}
void spiAbstractBusSetClock(SPI_ABSTRACT_BUS* bus,SPI_CLOCK clock){
	if(bus){
		bus->clock = clock;
		const SPI_CLASS* class = bus->sclass;
		void (*fn)(SPI_ABSTRACT_BUS*,SPI_CLOCK clock) = (void (*)(SPI_ABSTRACT_BUS*,SPI_CLOCK clock))pgm_read_word(&class->clock);
		fn(bus,clock);
	}
}


