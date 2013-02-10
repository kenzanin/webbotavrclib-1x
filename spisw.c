/*
 * $Id: spisw.c,v 1.10 2011/07/06 00:27:33 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: spisw.c,v $
 * Revision 1.10  2011/07/06 00:27:33  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.9  2010/07/01 23:55:27  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.8  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.7  2010/05/09 22:10:31  clivewebster
 * Add modes 1 and 3 for Sony PS2 controller
 *
 * Revision 1.6  2010/04/12 23:14:04  clivewebster
 * Add pullup on MISO and first attempt to handle modes 1 and 3
 *
 * Revision 1.5  2010/03/07 20:17:11  clivewebster
 * *** empty log message ***
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
 * spisw.c
 *
 *  Created on: 24-Jun-2009
 *      Author: Clive Webster
 */
#include "spisw.h"
//#include "pinChange.h"
#include "timer.h"

// Define the implementations of the virtual classes
static void 	__spiSWInit(SPI_ABSTRACT_BUS* spi, boolean master);
static void 	__spiSWOff(SPI_ABSTRACT_BUS* spi);
static void 	__spiSWSetClock(SPI_ABSTRACT_BUS* spi, SPI_CLOCK clock);
static void 	__spiSWSetDataOrder(SPI_ABSTRACT_BUS* spi,SPI_DATA_ORDER order);
static void 	__spiSWSetMode(SPI_ABSTRACT_BUS* spi,SPI_MODE mode);
static uint8_t 	__spiSWSendByte(SPI_ABSTRACT_BUS* spi, uint8_t data);
static void 	readMany(const SPI_ABSTRACT_DEVICE* device,void* dta, size_t size);
static void 	writeMany(const SPI_ABSTRACT_DEVICE* device,const void* dta, size_t size);
static void	 	select(const SPI_ABSTRACT_DEVICE* device,boolean selected);// Device selection change

SPI_CLASS PROGMEM c_sw_spi = MAKE_SPI_CLASS(&__spiSWInit, &__spiSWOff,&__spiSWSetClock,&__spiSWSetDataOrder, &__spiSWSetMode,&__spiSWSendByte,&readMany,&writeMany,&select);


// Called under interrupts when the chip select pin has changed
//void slaveListener(const IOPin* io,boolean val, volatile void* data){
//	if(val==FALSE){
//		// Chip IS selected - so read in the next byte
//
//	}
//}



//------------- Private methods - dont call directly -----
static void __spiSWInit(SPI_ABSTRACT_BUS* _spi, boolean master){
	SPI_SW* spi = (SPI_SW*)_spi;

    if(master){
    	pin_make_output(spi->MOSI, TRUE); 	// make MOSI an output and set high
    	pin_make_input(spi->MISO,TRUE);  	// make MISO an input with a pullup

    }else{
    	pin_make_input(spi->SCLK,FALSE);  	// make clock an input with no pullup
    	pin_make_input(spi->MOSI,FALSE);  	// make MOSI an input with no pullup
    	pin_make_output(spi->MISO,TRUE); 	// make MISO an output
    }

    __spiSWSetClock(_spi,_spi->clock);
    __spiSWSetDataOrder(_spi,_spi->order);
    __spiSWSetMode(_spi,_spi->mode);

}

static void __spiSWOff(SPI_ABSTRACT_BUS* _spi){
//	SPI_SW* spi = (SPI_SW*)_spi;
//	if(spi->SELECT!=null){
//		pin_change_dettach(spi->SELECT);
//	}
}

static void __spiSWSetClock(SPI_ABSTRACT_BUS* _spi, SPI_CLOCK clock){}
static void __spiSWSetDataOrder(SPI_ABSTRACT_BUS* _spi,SPI_DATA_ORDER order){}

static void __spiSWSetMode(SPI_ABSTRACT_BUS* _spi,SPI_MODE mode){
	SPI_SW* spi = (SPI_SW*)_spi;
	// Set the clock to its initial state
	pin_make_output(spi->SCLK, (mode == SPI_MODE_2 || mode == SPI_MODE_3));
}


static uint8_t __spiReadMISO(const SPI_SW* spi, uint8_t data){
	if(spi->_bus_.order == SPI_DATA_ORDER_MSB){
		// MSB is sent first
		data<<=1;
		if(pin_is_high(spi->MISO)){
			data |= 1;
		}
	}else{
		// LSB is sent first
		data>>=1;
		if(pin_is_high(spi->MISO)){
			data |= 0x80;
		}
	}
	return data;
}

static uint8_t __spiWriteMOSI(const SPI_SW* spi, uint8_t data){
	if(spi->_bus_.order == SPI_DATA_ORDER_MSB){
		// MSB is sent first
		pin_set(spi->MOSI, (data & 0x80));
		data<<=1;
	}else{
		// LSB is sent first
		pin_set(spi->MOSI, (data & 1));
		data>>=1;
	}
	return data;
}


// only tested with modes 0 and 2 (cpha = 0)
static uint8_t __spiSWSendByte(SPI_ABSTRACT_BUS* _spi, uint8_t data){
	SPI_SW* spi = (SPI_SW*)_spi;
	uint8_t rtn=0;
	uint8_t delay = _spi->clock / 3;	// delay loop is approx 3 cycles
	uint8_t cpha = _spi->mode & 1;

	for(int i=0; i<8; i++){						/* for each bit			*/
		if(!cpha){ // Mode 0 or 2
			data = __spiWriteMOSI(spi,data);		/* Set the output pin 	*/
			rtn  = __spiReadMISO(spi,rtn);			/* read input for modes 0 and 2	*/
			pin_toggle(spi->SCLK);					/* toggle clock (high) 	*/
			if(delay) _delay_loop_1(delay);			/* delay			  	*/
			pin_toggle(spi->SCLK);					/* toggle clock (low)	*/
		}else{		// Mode 1 or 3
			pin_toggle(spi->SCLK);					/* toggle clock (high) 	*/
			data = __spiWriteMOSI(spi,data);		/* Set the output pin 	*/
			if(delay) _delay_loop_1(delay);			/* delay			  	*/
			pin_toggle(spi->SCLK);					/* toggle clock (low)	*/
			rtn  = __spiReadMISO(spi,rtn);			/* read input for modes 1 and 3	*/
		}
	}											/* next bit				*/
	pin_high(spi->MOSI);						/* make MOSI high		*/
	return rtn;									/* return read byte		*/
}

static void readMany(const SPI_ABSTRACT_DEVICE* device,void* dta, size_t size){
	SPI_SW* spi = (SPI_SW*)(device->bus);
	uint8_t delay = spi->_bus_.clock / 3;		// delay loop is approx 3 cycles
	uint8_t* dst = (uint8_t*)dta;
	__spiWriteMOSI(spi,device->fillerByte);			/* Set the output pin 	     */
													/* using first bit of filler */

	// Get MISO pin data
	const IOPin*  io = spi->MISO;
	const IOPort* misoPortDef = (const IOPort*)pgm_read_word(&io->port);
	PORT misoPin  = pgm_read_word(&misoPortDef->pin);
	PIN  misoMask = pgm_read_byte(&io->pin);

	// Get SCLK data
	io = spi->SCLK;
	const IOPort* sclkPortDef = (const IOPort*)pgm_read_word(&io->port);
	PORT sclkPort = pgm_read_word(&sclkPortDef->port);
	PIN sclkMask = pgm_read_byte(&io->pin);

	uint8_t cpha = spi->_bus_.mode & 1;

	if(spi->_bus_.order == SPI_DATA_ORDER_MSB){
		// MSB is sent first
		while(size--){
			uint8_t rtn = 0;
			for(int i=0; i<8; i++){						/* for each bit			*/
				rtn<<=1;
				if(!cpha){ // Mode 0 and 2
					pin_high(spi->MOSI);					/* make MOSI high		*/
					if(_SFR_MEM8(misoPin) & misoMask){	/* read input for modes 0 & 2 */
						rtn |= 1;
					}
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (high) 	*/
					if(delay) _delay_loop_1(delay);			/* delay			  	*/
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/

				}else{ // Mode 1 or 3
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (high) 	*/
					pin_high(spi->MOSI);					/* make MOSI high		*/
					if(delay) _delay_loop_1(delay);			/* delay			  	*/
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/

					if(_SFR_MEM8(misoPin) & misoMask){	    /* read input for modes 1 & 3 */
						rtn |= 1;
					}
				}
			}											/* next bit				*/
			*dst++ = rtn;								/* store byte			*/
		}
	}else{
		// LSB is sent first
		while(size--){
			uint8_t rtn = 0;
			for(int i=0; i<8; i++){						/* for each bit			*/
				rtn>>=1;
				if(!cpha){ // Mode 0 or 2
					pin_high(spi->MOSI);					/* make MOSI high		*/
					if(_SFR_MEM8(misoPin) & misoMask){	/* read input for modes 0 & 2 */
						rtn |= 0x80;
					}
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (high) 	*/
					if(delay) _delay_loop_1(delay);			/* delay			  	*/
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/
				}else{		// Mode 1 or 3
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (high) 	*/
					pin_high(spi->MOSI);					/* make MOSI high		*/
					if(delay) _delay_loop_1(delay);			/* delay			  	*/
					_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/
					if(_SFR_MEM8(misoPin) & misoMask){	/* read input for modes 1 & 3 */
						rtn |= 0x80;
					}
				}

			}											/* next bit				*/
			*dst++ = rtn;								/* store byte			*/
		}
	}
}

static void writeMany(const SPI_ABSTRACT_DEVICE* device,const void* dta, size_t size){
	SPI_SW* spi = (SPI_SW*)(device->bus);
	uint8_t delay = spi->_bus_.clock / 3;		// delay loop is approx 3 cycles
	uint8_t* src = (uint8_t*)dta;

	// Get MOSI pin data
	const IOPin*  io = spi->MOSI;
	const IOPort* mosiPortDef = (const IOPort*)pgm_read_word(&io->port);
	PORT mosiPort  = pgm_read_word(&mosiPortDef->port);
	PIN  mosiMask = pgm_read_byte(&io->pin);

	// Get SCLK data
	io = spi->SCLK;
	const IOPort* sclkPortDef = (const IOPort*)pgm_read_word(&io->port);
	PORT sclkPort = pgm_read_word(&sclkPortDef->port);
	PIN sclkMask = pgm_read_byte(&io->pin);

	uint8_t cpha = spi->_bus_.mode & 1;


	if(spi->_bus_.order == SPI_DATA_ORDER_MSB){
		// MSB is sent first
		while(size--){
			uint8_t rtn = *src++;
			for(int i=0; i<8; i++){						/* for each bit			*/
				if(cpha) _SFR_MEM8(sclkPort) ^= sclkMask;/* toggle clock (high) 	*/
				if(rtn & 0x80){
					_SFR_MEM8(mosiPort) |= mosiMask;	/* set high 			*/
				}else{
					_SFR_MEM8(mosiPort) &= ~mosiMask;	/* set low 				*/
				}
				rtn<<=1;
				if(!cpha) _SFR_MEM8(sclkPort) ^= sclkMask;/* toggle clock (high) 	*/
				if(delay) _delay_loop_1(delay);			/* delay			  	*/
				_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/
			}											/* next bit				*/
		}
	}else{
		// LSB is sent first
		while(size--){
			uint8_t rtn = *src++;
			for(int i=0; i<8; i++){						/* for each bit			*/
				if(cpha) _SFR_MEM8(sclkPort) ^= sclkMask;/* toggle clock (high) 	*/
				if(rtn & 1){
					_SFR_MEM8(mosiPort) |= mosiMask;	/* set high 			*/
				}else{
					_SFR_MEM8(mosiPort) &= ~mosiMask;	/* set low 				*/
				}
				rtn>>=1;
				if(!cpha) _SFR_MEM8(sclkPort) ^= sclkMask;/* toggle clock (high) 	*/
				if(delay) _delay_loop_1(delay);			/* delay			  	*/
				_SFR_MEM8(sclkPort) ^= sclkMask;		/* toggle clock (low) 	*/
			}											/* next bit				*/
		}
	}
	__spiWriteMOSI(spi,device->fillerByte);			/* Set the output pin 	     */
													/* using first bit of filler */
}

static void	 select(const SPI_ABSTRACT_DEVICE* device,boolean selected){
	if(selected){
		// Set MOSI to initial value
		SPI_SW* spi = (SPI_SW*)(device->bus);
		__spiWriteMOSI(spi,device->fillerByte);			/* Set the output pin 	     */
	}
}
