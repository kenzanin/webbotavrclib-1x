/*
 * $Id: i2c_slave.c,v 1.3 2010/10/01 19:55:03 clivewebster Exp $
 * Revision History
 * ================
 * $Log: i2c_slave.c,v $
 * Revision 1.3  2010/10/01 19:55:03  clivewebster
 * Setting the bit rate only applies to a hardware master
 *
 * Revision 1.2  2010/10/01 13:17:04  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/09/30 16:44:49  clivewebster
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
 *        File: i2c_slave.c
 *  Created on: 26 Aug 2010
 *      Author: Clive Webster
 */

// Include the device definitions
#include <avr/io.h>
#include "i2cBus.h"

static cBuffer* rxBuffer;
static cBuffer* txBuffer;
static void (*i2cSlaveReceive)(cBuffer* data);
static void (*i2cSlaveTransmit)(cBuffer* data);

void i2cSlaveSetReceiveHandler(void (*i2cSlaveRx_func)(cBuffer* data)){
	CRITICAL_SECTION{
		i2cSlaveReceive = i2cSlaveRx_func;
	}
}
void i2cSlaveSetTransmitHandler(void (*i2cSlaveTx_func)(cBuffer* data)){
	CRITICAL_SECTION{
		i2cSlaveTransmit = i2cSlaveTx_func;
	}
}

void i2cSlaveInit(uint8_t deviceAddr, boolean generalCall, cBuffer* rx, cBuffer* tx){
	// Make the 2 wires into inputs
	cbi(SCL_DDR,SCL_PIN);
	cbi(SDA_DDR,SDA_PIN);

	// Save the buffer addresses
	rxBuffer = rx;
	txBuffer = tx;

	// Only the master has pullups
	//sbi(SCL_PORT,SCL_PIN);
	//sbi(SDA_PORT,SDA_PIN);

	// set i2c bit rate to 100KHz
	// not for slaves as their bit rate is set by the master
//	i2cMasterSetBitrate(100);

	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);

	// set state
	i2cState = I2C_IDLE;

	// enable TWI interrupt and slave address ACK
	sbi(TWCR, TWIE);
	sbi(TWCR, TWEA);

	// Set the device address
	outb(TWAR, ((deviceAddr&0xFE) | (generalCall?1:0)) );

	// enable interrupts
	sei();

}

static inline void i2cReceiveByte(boolean ackFlag){
	// begin receive over i2c
	if( ackFlag ){
		// ackFlag = TRUE: ACK the received data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}else{
		// ackFlag = FALSE: NACK the received data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}


// I2C (TWI) interrupt service routine
ISR(SIG_2WIRE_SERIAL)
{
	// read status bits
	uint8_t status = inb(TWSR) & TWSR_STATUS_MASK;
	uint8_t byte;

	switch(status)
	{
	// Slave Receiver status codes
	case 0x60:					// 0x60: own SLA+W has been received, ACK has been returned
	case 0x68:		// 0x68: own SLA+W has been received, ACK has been returned
	case 0x70:				// 0x70:     GCA+W has been received, ACK has been returned
	case 0x78:		// 0x78:     GCA+W has been received, ACK has been returned
		// we are being addressed as slave for writing (data will be received from master)
		// set state
		i2cState = I2C_SLAVE_RX;
		// prepare buffer
		bufferFlush(rxBuffer);
		// receive data byte and return ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		break;

	case 0x80:			// 0x80: data byte has been received, ACK has been returned
	case 0x90:			// 0x90: data byte has been received, ACK has been returned
		byte = inb(TWDR);
		if(rxBuffer){
			boolean ok = bufferPut(rxBuffer, byte);
			i2cReceiveByte(ok);
		}else{
			i2cReceiveByte(FALSE);
		}
		break;

	case 0x88:			// 0x88: data byte has been received, NACK has been returned
	case 0x98:			// 0x98: data byte has been received, NACK has been returned
		// receive data byte and return NACK
		i2cReceiveByte(FALSE);
		break;

	case 0xa0:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		// switch to SR mode with SLA ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// i2c receive is complete, call i2cSlaveReceive
		if(i2cSlaveReceive && rxBuffer){
			i2cSlaveReceive(rxBuffer);
		}
		// set state
		i2cState = I2C_IDLE;
		break;

	// Slave Transmitter
	case 0xa8:					// 0xA8: own SLA+R has been received, ACK has been returned
	case 0xb0:					// 0xB0:     GCA+R has been received, ACK has been returned
		// we are being addressed as slave for reading (data must be transmitted back to master)
		// set state
		i2cState = I2C_SLAVE_TX;

		// request data from application
		if(i2cSlaveTransmit && txBuffer){
			// prepare buffer
			bufferFlush(txBuffer);
			i2cSlaveTransmit(txBuffer);
		}
		// fall-through to transmit first data byte
	case 0xb8:				// 0xB8: data byte has been transmitted, ACK has been received
		// transmit data byte
		bufferGet(txBuffer, &byte);
		outb(TWDR, byte);
		if(bufferBytesUsed(txBuffer)!=0){
			// expect ACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		}else{
			// expect NACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		}
		break;

	case 0xc0:				// 0xC0: data byte has been transmitted, NACK has been received
	case 0xc8:				// 0xC8:
		// all done
		// switch to open slave
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// set state
		i2cState = I2C_IDLE;
		break;

	// Miscellaneous settings
	case 0xf8:					// 0xF8: No relevant state information
		// do nothing
		break;

	case 0:						// 0x00: Bus error due to illegal start or stop condition
		// reset internal hardware and release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
		// set state
		i2cState = I2C_IDLE;
		break;
	}
}

