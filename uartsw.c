/*
 * $Id: uartsw.c,v 1.12 2010/12/02 19:11:04 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: uartsw.c,v $
 * Revision 1.12  2010/12/02 19:11:04  clivewebster
 * Try to achieve higher and more reliable baud rates
 *
 * Revision 1.11  2010/07/01 23:56:37  clivewebster
 * pin_make_output now specifies the initial output value
 *
 * Revision 1.10  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.9  2010/02/21 19:54:21  clivewebster
 * Allow software UART to use an 8 bit timer
 *
 * Revision 1.8  2010/02/17 23:57:58  clivewebster
 * Specify timer number in MAKE command rather than a point to the TIMER
 *
 * Revision 1.7  2009/11/02 19:07:54  clivewebster
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
 * uartsw.c
 *
 *  Created on: 30-Apr-2009
 *      Author: Clive Webster
 */
#include "timer.h"
#include "uartsw.h"
#include "core.h"
#include "errors.h"
// Program ROM constants

// Global variables

// functions

static void	startXmitMode(UART* _uart);
static void	endXmitMode(UART* _uart);
static void oneWireReceiveMode(const UART* _uart);

static void uartswTxBitService(const TimerCompare *channel, void* _uart)
{
	SW_UART* uart = (SW_UART*) _uart;
	if(uart->txBitNum)
	{
		// there are bits still waiting to be transmitted
		if(uart->txBitNum > 1)
		{
			// transmit data bits (inverted, LSB first)
			if( (uart->txData & 0x01) )
				pin_high(uart->_uart_.tx_pin);
			else
				pin_low(uart->_uart_.tx_pin);
			// shift bits down
			uart->txData >>= 1;
		}
		else
		{
			// transmit stop bit
			if(uart->inverted){
				pin_low(uart->_uart_.tx_pin);
			}else{
				pin_high(uart->_uart_.tx_pin);
			}
		}
		// schedule the next bit
		uint16_t top = 1 + timerGetTOP(compareGetTimer(channel));
		uint16_t next = (compareGetThreshold(channel) + uart->dataBitLength);
		if(next >= top){
			next -= top;
		}
		compareSetThreshold(channel, next );

		// count down
		uart->txBitNum--;
	}
	else
	{
		// We have finished sending the current byte - look for the next byte to send
		__uartTransmitService(&uart->_uart_);
	}
}


// Start receiving a new byte. Caused by a change on the input capture
static void uartswRxStartService(const Timer *timer, void* _uart){
	SW_UART* uart = (SW_UART*)_uart;
	// disable ICP interrupt - as remainder are done by OC1B
	__portMaskClear(&timer->pgm_captureint);

	// Get the timer value of when the interrupt happened
	PORT icr = pgm_read_word(&timer->pgm_icr);
	uint16_t capture = _SFR_MEM16(icr);

	// attach RxBit service routine to OC1B for 1.5 bit from now
	const TimerCompare* rx = timerGetCompare(timer,1);
	compareSetThreshold(rx, (capture + uart->dataBitLength + uart->dataBitLength/2) );
	compareClearInterruptPending(rx);		// clear any interrupt pending
	__portMaskSet(&rx->pgm_intenable);		// turn interrupt on

	// reset bit counter and data
	uart->rxBitNum = uart->rxData = 0;

}

static void uartswRxBitService(const TimerCompare *channel, void* _uart){
	SW_UART* uart = (SW_UART*)_uart;
	// start bit has already been received
	// we're in the data bits

	// shift data byte to make room for new bit
	uart->rxData >>= 1;

	// sample the data line
	if(uart->inverted){
		if( pin_is_low(uart->_uart_.rx_pin) ){
			uart->rxData |= 0x80;
		}
	}else{
		if( pin_is_high(uart->_uart_.rx_pin) ){
			uart->rxData |= 0x80;
		}
	}

	// schedule next bit sample
	compareSetThreshold(channel, (compareGetThreshold(channel) + uart->dataBitLength));

	// check if we have a full byte
	if(++uart->rxBitNum >= 8)
	{
		uint8_t c = uart->rxData;

		// Detach the OC1B interrupt
//		compareDetach(channel);
		__portMaskClear(&channel->pgm_intenable);		// turn interrupt off

		// attach RxBit service routine to ICP
		// trigger on rising edge if inverted, or falling if not
//		timerCaptureAttach(compareGetTimer(channel), uartswRxStartService, uart, uart->inverted );
		const Timer* timer = compareGetTimer(channel);
		timerCaptureClearInterruptPending(timer);	// Clear any pending capture interrupt flag
		__portMaskSet(&timer->pgm_captureint);		// Re-enable capture interrupt

		// save data in receive buffer
		__uartReceiveService(&uart->_uart_, c);
	}
}

// Disable the uart
static void __uartswOff(UART * _uart){
	SW_UART* uart = (SW_UART*) _uart;

	const Timer* timer = &pgm_Timers[uart->timer];

	// detach the service routines
	compareDetach(timerGetCompare(timer,0));
	compareDetach(timerGetCompare(timer,1));
	timerCaptureDetach(timer);
}


static void setTxIdle(SW_UART* uart){
	// set the output idle state of the transmitter
	pin_make_output(uart->_uart_.tx_pin, (uart->inverted) ? FALSE : TRUE);
}


// enable and initialize the software uart - called with interrupts turned off
void __uartswInit(UART* _uart, BAUD_RATE baud){
	SW_UART* uart = (SW_UART*) _uart;
	if(uart->_uart_.rx_pin!=null || uart->_uart_.tx_pin!=null){

		const Timer* timer = &pgm_Timers[uart->timer];

		// initialize baud rate
		uartSetBaudRate(uart, baud);

		// setup the transmitter
		if(uart->_uart_.tx_pin!=null){
			uart->txBitNum=0;

			// set the output idle state of the transmitter
			if(!uartIsOneWire(uart)){
				setTxIdle(uart);
			}

			CRITICAL_SECTION{
				const TimerCompare* channel = timerGetCompare(timer,0);
				// Mark the transmit channel as in use
				compareAttach(channel,&uartswTxBitService,0,uart);

				// But turn off interrupt for now
				__portMaskClear(&channel->pgm_intenable);
			}

		}

		// setup the receiver
		if(uart->_uart_.rx_pin!=null){

			// Locate the timer whose capture input is this pin
			const IOPin* tCapture = timerGetCapturePin(timer);
			if(tCapture!=uart->_uart_.rx_pin){
				setError(UARTSW_NOT_CAPTURE);
			}else{
				// Make the Rx pin an input with no pullup
				pin_make_input(uart->_uart_.rx_pin,FALSE);

				// attach RxBit service routine to ICP
				// trigger on rising edge if inverted, falling if not
				timerCaptureAttach(timer, uartswRxStartService, uart, uart->inverted );

				// Set up receive interrupt for bit handling - then immediately disable it
				const TimerCompare* rx = timerGetCompare(timer,1);
				compareAttach(rx, uartswRxBitService, compareGetThreshold(rx)-1, uart);
				__portMaskClear(&rx->pgm_intenable);	// turn interrupt off
				compareClearInterruptPending(rx);		// clear any interrupt pending
			}
		}
	}
	oneWireReceiveMode(_uart);
}


void __uartswSetBaudRate(UART* _uart, BAUD_RATE baudrate){
	SW_UART* uart = (SW_UART*)_uart;

	const Timer* timer = &pgm_Timers[uart->timer];
	boolean	isMyTimer = FALSE;

	// Make sure the timer is in normal mode
	if(!timerIsInUse(timer)){
		timerSetMode(timer,TIMER_MODE_NORMAL);
		// set timer prescaler
		timerSetPrescaler(timer,1);

		isMyTimer = TRUE;
	}

	uint16_t top = timerGetTOP(timer);

	// Get the current prescaler
	uint16_t prescale = timerGetPrescaler(timer);

	// calculate division factor for requested baud rate, and set it
calcBaud:
	uart->dataBitLength = (uint16_t)(  (cpu_speed+(baudrate/2L)) / (prescale * baudrate * 1L)  );

	// Fudge factor compensates for interrupt processing overhead
	// The larger this number then the shorter the start bit
	//
	uint16_t pre = prescale;
	uint16_t fudge = 104U;
	while(pre > 1){
		fudge>>=1;
		pre>>=1;
	}
	uart->startBitLength = uart->dataBitLength - fudge;

	if(uart->dataBitLength > top){
		uart->dataBitLength = top;
		if(isMyTimer){
			// Try increasing the prescaler
			uint16_t nextpre = timerGetClosestPrescale(timer, prescale+1);
			if(nextpre != prescale){
				prescale = nextpre;
				timerSetPrescaler(timer,prescale);
				goto calcBaud;
			}
		}
	}



}

// Start transmitting the given byte
void __uartswStartXmit(UART* _uart, uint8_t data){

	SW_UART* uart = (SW_UART*)_uart;

	// save data
	uart->txData = (uart->inverted) ? ~data : data;

	// set number of bits (+1 for stop bit)
	uart->txBitNum = 9;

	const Timer* timer = &pgm_Timers[uart->timer];

	// Get the time when the start bit is set
	uint16_t start;
	start = timerGetCounter(timer);
	// set the start bit
	if(uart->inverted){
		pin_high(uart->_uart_.tx_pin);
	}else{
		pin_low(uart->_uart_.tx_pin);
	}

	if(uart->_uart_.tx_pin){
		const TimerCompare* channel = timerGetCompare(timer,0);

		// schedule the next bit
		uint16_t top = 1 + timerGetTOP(timer);
		uint16_t next = (start + uart->startBitLength);
		if(next >= top){
			next -= top;
		}
		compareSetThreshold(channel, next);

		// No interrupt pending
		compareClearInterruptPending(channel);

		// Allow compare interrupts
		__portMaskSet(&channel->pgm_intenable);

	}else{
		// There is no iopin - so we have finished sending the current byte
		__uartTransmitService(&uart->_uart_);
	}

}

// We are about to start sending stuff
static void	startXmitMode(UART* _uart){
	if(_uartIsOneWire(_uart)){
		SW_UART* uart = (SW_UART*)_uart;
		// Place into transmit mode and turn receiver off
		const Timer* timer = &pgm_Timers[uart->timer];
		const TimerCompare* rx = timerGetCompare(timer,1);

		CRITICAL_SECTION_START;

		// Disable the input capture
		__portMaskClear(&timer->pgm_captureint);	// disable capture interrupt on timer
		__portMaskClear(&rx->pgm_intenable);		// disable timer compare interrupt for recv bits

		// Turn the transmitter pin back into an output
		setTxIdle(uart);

		CRITICAL_SECTION_END;

	}
}

// If one wire - then go into receive mode
static void oneWireReceiveMode(const UART* _uart){
	if(_uartIsOneWire(_uart)){
		const SW_UART* uart = (const SW_UART*)_uart;
		// Place into receive mode and turn transmitter off

		const Timer* timer = &pgm_Timers[uart->timer];

		CRITICAL_SECTION_START;

		// Turn the transmitter/receiver pin back into an output
//		setTxIdle(uart);

		// Turn the xmit into an input pin with pullup
		pin_make_input(_uart->tx_pin,TRUE);

		// Clear any pending capture interrupt flag
		timerCaptureClearInterruptPending(timer);

		// Re-enable the input capture to detect the start of a byte
		__portMaskSet(&timer->pgm_captureint);

		CRITICAL_SECTION_END;
	}
}

// We have finished sending stuff
static void	endXmitMode(UART* _uart){
	const SW_UART* uart = (const SW_UART*)_uart;
	// Turn timer interrupt off
	const Timer* timer = &pgm_Timers[uart->timer];
	const TimerCompare* channel = timerGetCompare(timer,0);
	__portMaskClear(&channel->pgm_intenable);

	oneWireReceiveMode(_uart);
}




// Create a class with the method overrides for this type of UART
UART_CLASS c_sw_uart = MAKE_UART_CLASS( \
		&__uartswSetBaudRate, \
		&__uartswStartXmit, \
		&__uartswInit, \
		&__uartswOff, \
		&startXmitMode, \
		&endXmitMode );
