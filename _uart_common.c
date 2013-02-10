/*
 * $Id: _uart_common.c,v 1.12 2011/04/20 14:34:55 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: _uart_common.c,v $
 * Revision 1.12  2011/04/20 14:34:55  clivewebster
 * Add void* parameter to uartAttach
 *
 * Revision 1.11  2010/06/14 19:21:25  clivewebster
 * Add copyright license info
 *
 * Revision 1.10  2010/05/09 22:07:06  clivewebster
 * *** empty log message ***
 *
 * Revision 1.9  2010/04/25 03:33:56  clivewebster
 * Handle receive buffer calls better when there is no buffer
 *
 * Revision 1.8  2010/03/24 19:48:54  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2010/03/07 19:31:50  clivewebster
 * *** empty log message ***
 *
 * Revision 1.6  2009/11/02 18:50:27  clivewebster
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
 * _uart_common.c
 *
 *  Created on: 04-May-2009
 *      Author: Clive Webster
 */

#include "_uart_common.h"
#include "core.h"


// Virutal function - start sending the given byte
void _uartStartXmit(UART* uart,uint8_t txData){
	const UART_CLASS* class = uart->class;
	void (*fn)(UART*,uint8_t) = (void (*)(UART*,uint8_t))pgm_read_word(&class->startXmit);
	fn(uart,txData);
}

// We are starting to transmit a block
static void _uartTransmitBlockStart(UART* uart){
	uart->readyTx = FALSE;				// indicate we are sending
	if(uart->xmitCallback){
		uart->xmitCallback(uart->callbackParam,TRUE);
	}
	const UART_CLASS* class = uart->class;
	void (*fn)(UART*) = (void (*)(UART*))pgm_read_word(&class->startXmitMode);
	if(fn){
		fn(uart);
	}
}

// We have finished transmitting a block
static void _uartTransmitBlockEnd(UART* uart){
	uart->readyTx = TRUE;				// indicate we are no longer sending
	const UART_CLASS* class = uart->class;
	void (*fn)(UART*) = (void (*)(UART*))pgm_read_word(&class->endXmitMode);
	if(fn){
		fn(uart);
	}
	if(uart->xmitCallback){
		uart->xmitCallback(uart->callbackParam,FALSE);
	}

}



// A byte has been received - interrupts are off
void __uartReceiveService(UART* uart, uint8_t c){
	// if there's a user function to handle this receive event
	if(uart->rxFunc){
		// call it and pass the received data
		uart->rxFunc(c,uart->rxFuncData);
	}else if( uart->rxBuffer ){
		// there is a receive buffer so try to append it
		if(bufferPut(__uartGetRxBuffer(uart),c)==FALSE){
			// track overflows
			uart->rxOverflow=TRUE;
			setError(UART_RECEIVE_OVERFLOW);
		}
	}else{
		if(uart->rxChar == -1){
			uart->rxChar = c;
		}else{
			uart->rxOverflow=TRUE;
			setError(UART_RECEIVE_OVERFLOW);
		}
	}
}

// We have finished transmitting the current byte
void __uartTransmitService(UART* uart){
	if(uart->sendingBuffer){
		// there is a transmit buffer
		uint8_t c;

		CRITICAL_SECTION_START;
		if(bufferGet(__uartGetTxBuffer(uart),&c)){
			// send the next byte
			_uartStartXmit(uart,c);
		}else{
			// all done
			_uartTransmitBlockEnd(uart);		// we have finished sending
			uart->sendingBuffer = FALSE;		// we are not sending a buffer
		}
		CRITICAL_SECTION_END;
	}else{
		CRITICAL_SECTION_START;
		// indicate transmit complete, back to ready
		_uartTransmitBlockEnd(uart);				// we have finished sending
		CRITICAL_SECTION_END;
	}
}


// Gets a single byte from the uart receive buffer.
// Returns the byte, or -1 if no byte is available (getchar-style).
int __uartGetByte(UART* uart){
	// get single byte from receive buffer (if available)
	uint8_t c;
	int rtn;

	cBuffer* buffer = __uartGetRxBuffer(uart);
	if(buffer!=null){
		// There is a receive buffer
		if(bufferGet(buffer,&c))
			rtn = c;
		else
			rtn = -1;
	}else{
		CRITICAL_SECTION_START;
		rtn = uart->rxChar;		// get the last received char
		uart->rxChar = -1;		// and reset it
		CRITICAL_SECTION_END;
	}
	return rtn;
}

// Send a block of data
void _uartSendBuffer(UART* uart, const uint8_t* data, size_t count){
	while(count--){
		_uartSendByte(uart,*data++);
	}
}


// Sends a single byte over the uart.
uint8_t _uartSendByte(UART* uart, uint8_t txData){
	if(uart->txBuffer){
		// Wait if the buffer is full
		while(bufferIsFull(uart->txBuffer)){
			breathe();
		}

		CRITICAL_SECTION_START;
		if(!uart->sendingBuffer){
			// Not already transmitting so start sending the buffer
			uart->sendingBuffer = TRUE;			// we are sending a buffer
			_uartTransmitBlockStart(uart);			// indicate we are sending
			_uartStartXmit(uart,txData);		// put the byte
		}else{
			// add char to the output queue
			bufferPut(uart->txBuffer,txData);
		}
		CRITICAL_SECTION_END;

	}else{
		// There is no xmit buffer - just send now

		// Wait till sending has stopped
		while(!uart->readyTx){
			breathe();
		}
		CRITICAL_SECTION_START;
		_uartTransmitBlockStart(uart);			// indicate we are sending
		_uartStartXmit(uart,txData);		// put the byte
		CRITICAL_SECTION_END;
	}
	return txData;
}

void _uartSetBaudRate(UART* uart, BAUD_RATE baud){
	const UART_CLASS* class = uart->class;
	void (*fn)(UART*,BAUD_RATE) = (void (*)(UART*,BAUD_RATE))pgm_read_word(&class->setBaudRate);
	fn(uart,baud);
}

void _uartOff(UART* uart){
	const UART_CLASS* class = uart->class;
	void (*fn)(UART*) = (void (*)(UART*))pgm_read_word(&class->off);

	// Wait till everything has been sent
	INTERRUPTABLE_SECTION_START;
	while(__uartIsBusy(uart));
	INTERRUPTABLE_SECTION_END;

	fn(uart);
}


void _uartInit(UART* uart, BAUD_RATE baud){
	cli();

	// For one wire Make the pins into inputs with pullups enabled
	if(_uartIsOneWire(uart)){
		if(uart->rx_pin != uart->tx_pin){
			pin_make_input(uart->rx_pin,FALSE);
		}
		pin_make_input(uart->tx_pin,TRUE);
	}

	// Initialise local variable
	uart->readyTx = TRUE;
	uart->rxOverflow = FALSE;
	uart->sendingBuffer = FALSE;
//	uart->rxFunc = null;

	const UART_CLASS* class = uart->class;
	void (*fn)(UART*,BAUD_RATE) = (void (*)(UART*,BAUD_RATE))pgm_read_word(&class->init);

	fn(uart,baud);
	sei();
}

void __uartFlushReceiveBuffer(UART* uart){
	if(__uartGetRxBuffer(uart)){
		// flush all data from receive buffer
		bufferFlush(__uartGetRxBuffer(uart));
	}else{
		CRITICAL_SECTION_START;
		uart->rxChar = -1;
		CRITICAL_SECTION_END;
	}
}

boolean __uartReceiveBufferIsEmpty(const UART* uart){
	boolean rtn;
	if(__uartGetRxBuffer(uart)){
		CRITICAL_SECTION_START;
		rtn = (__uartGetRxBuffer(uart)->datalength == 0) ? TRUE : FALSE;
		CRITICAL_SECTION_END;
	}else{
		CRITICAL_SECTION_START;
		rtn = (uart->rxChar == -1) ? TRUE : FALSE;
		CRITICAL_SECTION_END;
	}
	return rtn;
}
