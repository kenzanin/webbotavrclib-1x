/*
 * $Id: i2cHwBus.c,v 1.4 2011/07/06 00:25:37 clivewebster Exp $
 * Revision History
 * ================
 * $Log: i2cHwBus.c,v $
 * Revision 1.4  2011/07/06 00:25:37  clivewebster
 * Allow slower baud rates
 *
 * Revision 1.3  2010/10/01 19:55:03  clivewebster
 * Setting the bit rate only applies to a hardware master
 *
 * Revision 1.2  2010/10/01 15:28:11  clivewebster
 * Refactored - merge master and slave into i2cBus.h
 *
 * Revision 1.1  2010/09/30 16:44:01  clivewebster
 * Refactored to replace i2c_master.h
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
 *        File: i2cHwBus.c
 *  Created on: 24 Sep 2010
 *      Author: Clive Webster
 *
 *  Place a thin wrapper around the i2c_master calls so that they can be used
 *  from an abstract i2c bus
 */
#include "i2cBus.h"
#include "_i2c_common.h"

// TWS4:3 set - ACK has been received back
// Acknowledgement in Write mode
#define TW_MT_SLA_ACK				0x18

// TWS6 set
// Acknowledgement in Read mode
#define TW_MR_SLA_ACK				0x40

// The sent data has been acknowledged
#define TW_MT_DATA_ACK				0x28

// Include the device definitions
#include <avr/io.h>

static void speed(const I2C_ABSTRACT_BUS* bus,uint16_t bitrateKHz){
	// TWBR x 4pow(TWPS) = ((cpudiv1000/ bitrateKHz) - 16) /2
	uint16_t bitrate_div;

	// calculate bitrate division
	bitrate_div = cpu_speed_div_1000 / bitrateKHz;

	if(bitrate_div >= 16){
		bitrate_div = (bitrate_div-16)/2;
	}else{
		bitrate_div=0;
	}

	// Turn prescaler off by default
	#ifdef TWPS0
		cbi(TWSR, TWPS0);
	#endif
	#ifdef TWPS1
		cbi(TWSR, TWPS1);
	#endif

	// Check if need to use the prescaler
	#ifdef TWPS0
	if(bitrate_div > 255){
		sbi(TWSR, TWPS0);
		bitrate_div /= 4;
	}
	#endif
	#ifdef TWPS1
	if(bitrate_div > 255){
		sbi(TWSR, TWPS1);
		bitrate_div /= 16;
	}
	#endif

	// Cannot prescale it on some devices - so just go as slow as possible
	if(bitrate_div > 255){
		bitrate_div = 255;
	}

	outb(TWBR, (uint8_t)(bitrate_div) );
}


// Initialiase I2C hardware
void __attribute__ ((constructor)) _private_i2cMasterInit(void){

	// Make the 2 wires into inputs
	cbi(SCL_DDR,SCL_PIN);
	cbi(SDA_DDR,SDA_PIN);
	// and turn on the pullups
	sbi(SCL_PORT,SCL_PIN);
	sbi(SDA_PORT,SDA_PIN);


	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);

	// set state
	i2cState = I2C_IDLE;

	// enable TWI interrupt and slave address ACK
//	sbi(TWCR, TWIE);
//	sbi(TWCR, TWEA);

	// enable interrupts
//	sei();
}

static void init(I2C_ABSTRACT_BUS* bus){
	// set i2c bit rate to 100KHz
	speed(bus,100);
}

static inline void i2cWaitForComplete(void){
	// wait for i2c interface to complete operation
	while( !(inb(TWCR) & BV(TWINT)) );
}

static uint8_t getByte(const I2C_ABSTRACT_BUS* bus, boolean isLastByte){
	// begin receive over i2c
	if( isLastByte ){
		// ackFlag = FALSE: NACK the received data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}else{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	i2cWaitForComplete();
	return inb(TWDR);
}


static boolean putByte(const I2C_ABSTRACT_BUS* bus, uint8_t data){
	// save data to the TWDR
	outb(TWDR, data);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));

	i2cWaitForComplete();

	uint8_t got = inb(TWSR) & TWSR_STATUS_MASK;	// get the status

    return( (got == TW_MT_DATA_ACK) ? TRUE : FALSE );

}

static boolean start(const I2C_DEVICE* device, boolean writeMode){
	uint8_t expect;
	if(writeMode){
		i2cState = I2C_MASTER_TX;
		expect = TW_MT_SLA_ACK;
	}else{
		i2cState = I2C_MASTER_RX;
		expect = TW_MR_SLA_ACK;
	}

	// send start condition
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
	i2cWaitForComplete();

	// Send the device addr and direction
	uint8_t addr = device->addr;
	if(writeMode==FALSE){
		addr |= 1;
	}else{
		addr &= 0xfe;
	}
	outb(TWDR, addr);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	i2cWaitForComplete();

	uint8_t got = inb(TWSR) & TWSR_STATUS_MASK;	// get the status
    return( (got == expect) ? TRUE : FALSE );

}

static void stop(const I2C_ABSTRACT_BUS* bus){
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
	i2cState = I2C_IDLE;
}


// Expose this implementation to the linker
I2C_CLASS c_hw_i2c = MAKE_I2C_CLASS(&init, &start,&stop, &getByte, &putByte, &speed);
