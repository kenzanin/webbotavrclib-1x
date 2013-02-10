/*
 * $Id: i2cSwBus.c,v 1.3 2010/10/01 19:55:03 clivewebster Exp $
 * Revision History
 * ================
 * $Log: i2cSwBus.c,v $
 * Revision 1.3  2010/10/01 19:55:03  clivewebster
 * Setting the bit rate only applies to a hardware master
 *
 * Revision 1.2  2010/09/30 16:43:33  clivewebster
 * refactored
 *
 * Revision 1.1  2010/09/30 12:09:06  clivewebster
 * Added
 *
 * ================
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
 *        File: i2cSwBus.c
 *  Created on: 28 Sep 2010
 *      Author: Clive Webster
 *
 *  Simulate an I2C bus in software. Only 'single master' is supported
 *
 *  You MUST add external 4k7 resistors. I cannot use the internal pullups.
 *  Equally when setting either pin high then I can't use pin_high as if the
 *  slave extends the clock then something may blow up
 *
 */
#include "i2cBus.h"
#include "iopin.h"
#include "timer.h"

// Make the clock high
static void scl_high(const I2C_SOFTWARE_BUS* i2c){
	do{
		pin_make_input(i2c->scl,FALSE);		// Assumes external resistor (works)
	}while(pin_is_low(i2c->scl));
}

// Make the data line high
static void sda_high(const I2C_SOFTWARE_BUS* i2c){
	pin_make_input(i2c->sda,FALSE);			// Assumes external resistor (works)
}


// Make the clock low
static void scl_low(const I2C_SOFTWARE_BUS* i2c){
	pin_make_output(i2c->scl,FALSE);
}

// Make the data line low
static void sda_low(const I2C_SOFTWARE_BUS* i2c){
	pin_make_output(i2c->sda,FALSE);
}

// Wait for at least 5uS
static void halfDelay(){
	delay_us(5);
}


// Return true if the slave acknowledges
static boolean getAck(const I2C_SOFTWARE_BUS* i2c){
	sda_high(i2c);	// allow slave to drive sda
	scl_high(i2c);
	halfDelay();
	boolean rtn = pin_is_low(i2c->sda);
	scl_low(i2c);

	return rtn;
}

// Put a byte across the wire
// Return true if the slave acknowledges
static boolean putByte(const I2C_ABSTRACT_BUS* bus, uint8_t b){
	const I2C_SOFTWARE_BUS* i2c = (const I2C_SOFTWARE_BUS*)bus;
	int8_t i;

	for(i=7; i>=0; i--){
		if( b & 0x80 ){
			sda_high(i2c);
		}else{
			sda_low(i2c);
		}
		scl_high(i2c);
		b <<= 1;
		scl_low(i2c);
	}
	return getAck(i2c);						// return ACK value
}

// Send the start and address
// return TRUE if acknowledged
static boolean start(const I2C_DEVICE* device, boolean writeMode){
	boolean rtn = FALSE;

	const I2C_SOFTWARE_BUS* i2c = (const I2C_SOFTWARE_BUS*)(device->bus);
	if(i2c){
		sda_high(i2c);
		halfDelay();
		scl_high(i2c);
		halfDelay();

		sda_low(i2c);
		halfDelay();
		scl_low(i2c);
		halfDelay();

		// Send the device addr and direction
		uint8_t addr = device->addr & 0xfe;
		if(writeMode==FALSE){
			addr |= 1;
		}
		rtn = putByte(device->bus, addr);
	}
	return rtn;
}


static uint8_t getByte(const I2C_ABSTRACT_BUS* bus, boolean isLastByte){
	const I2C_SOFTWARE_BUS* i2c = (const I2C_SOFTWARE_BUS*)bus;
	int8_t i;
	uint8_t b = 0;

	sda_high(i2c);
	for(i=7; i>=0; i--){
		b <<= 1;
		scl_high(i2c);
		halfDelay();

		if(pin_is_high(i2c->sda)){
			b |= 1;
		}
		scl_low(i2c);
		halfDelay();
	}

	// Put the ACK
	if(isLastByte){
		sda_high(i2c);
	}else{
		sda_low(i2c);
	}
	scl_high(i2c);
	halfDelay(i2c);
	scl_low(i2c);
	sda_high(i2c);

	return b;						// return received byte
}

// Send the stop
static void stop(const I2C_ABSTRACT_BUS* bus){
	const I2C_SOFTWARE_BUS* i2c = (const I2C_SOFTWARE_BUS*)bus;
	sda_low(i2c);
	halfDelay();
	scl_high(i2c);
	halfDelay();
	sda_high(i2c);
	halfDelay();
}

static void init(I2C_ABSTRACT_BUS* bus){
	const I2C_SOFTWARE_BUS* i2c = (I2C_SOFTWARE_BUS*)bus;

	// Make both pins high
	sda_high(i2c);
	scl_high(i2c);
}



// Expose this implementation to the linker
I2C_CLASS c_sw_i2c = MAKE_I2C_CLASS(&init, &start,&stop, &getByte, &putByte, null);

