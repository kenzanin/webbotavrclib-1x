/*
 * $Id: device.h,v 1.16 2011/02/21 20:28:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: device.h,v $
 * Revision 1.16  2011/02/21 20:28:57  clivewebster
 * Added ATMega1284P
 *
 * Revision 1.15  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.14  2010/05/09 22:08:36  clivewebster
 * Add ATMega128
 *
 * Revision 1.13  2010/04/12 23:17:43  clivewebster
 * Add support for ATmega128RFA1
 *
 * Revision 1.12  2010/02/18 00:03:40  clivewebster
 * Add support for ATMega1280
 *
 * Revision 1.11  2010/02/09 16:45:11  clivewebster
 * Added ATMega2561
 *
 * Revision 1.10  2010/02/09 15:03:19  clivewebster
 * Use MAKE_READER and MAKE_WRITER
 *
 * Revision 1.9  2010/01/30 21:23:18  clivewebster
 * Fixed ATmega32 problem with Timer0 CompareA
 *
 * Revision 1.8  2010/01/24 17:49:48  clivewebster
 * *** empty log message ***
 *
 * Revision 1.7  2009/11/02 18:55:49  clivewebster
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
 */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "libdefs.h"

#ifdef BUILDING_LIBRARY
#   error "device.h must not be included in the compiled library"
#endif

#ifndef THE_SYSTEM
#   error "You must include a physical board first such as sys/axon.h before including device.h"
#endif



/**
   Define common data structures used by the device
*/
#if defined (__AVR_ATmega640__)
# include "dev/ATmega640.h"
#elif defined (__AVR_ATmega168__)
#  include "dev/ATmega168.h"
#  define USART0_RX_vect USART_RX_vect
#  define USART0_TX_vect USART_TX_vect
#elif defined (__AVR_ATmega8__)
#  include "dev/ATmega8.h"
#  define USART0_RX_vect USART_RXC_vect
#  define USART0_TX_vect USART_TXC_vect
#elif defined (__AVR_ATmega32__)
#  include "dev/ATmega32.h"
#  define USART0_TX_vect USART_TXC_vect
#  define USART0_RX_vect USART_RXC_vect
#elif defined (__AVR_ATmega1280__)
#  include "dev/ATmega1280.h"
#elif defined (__AVR_ATmega128__)
#  include "dev/ATmega128.h"
#elif defined (__AVR_ATmega2560__)
#  include "dev/ATmega2560.h"
#elif defined (__AVR_ATmega2561__)
#  include "dev/ATmega2561.h"
#elif defined (__AVR_ATmega128RFA1__)
#  include "dev/ATmega128RFA1.h"
#elif defined (__AVR_ATmega1284P__)
// They went and changed the names of the ports!
#ifdef PORTA0
#  define PA0 PORTA0
#endif
#ifdef PORTA1
#  define PA1 PORTA1
#endif
#ifdef PORTA2
#  define PA2 PORTA2
#endif
#ifdef PORTA3
#  define PA3 PORTA3
#endif
#ifdef PORTA4
#  define PA4 PORTA4
#endif
#ifdef PORTA5
#  define PA5 PORTA5
#endif
#ifdef PORTA6
#  define PA6 PORTA6
#endif
#ifdef PORTA7
#  define PA7 PORTA7
#endif
#ifdef PORTB0
#  define PB0 PORTB0
#endif
#ifdef PORTB1
#  define PB1 PORTB1
#endif
#ifdef PORTB2
#  define PB2 PORTB2
#endif
#ifdef PORTB3
#  define PB3 PORTB3
#endif
#ifdef PORTB4
#  define PB4 PORTB4
#endif
#ifdef PORTB5
#  define PB5 PORTB5
#endif
#ifdef PORTB6
#  define PB6 PORTB6
#endif
#ifdef PORTB7
#  define PB7 PORTB7
#endif
#ifdef PORTC0
#  define PC0 PORTC0
#endif
#ifdef PORTC1
#  define PC1 PORTC1
#endif
#ifdef PORTC2
#  define PC2 PORTC2
#endif
#ifdef PORTC3
#  define PC3 PORTC3
#endif
#ifdef PORTC4
#  define PC4 PORTC4
#endif
#ifdef PORTC5
#  define PC5 PORTC5
#endif
#ifdef PORTC6
#  define PC6 PORTC6
#endif
#ifdef PORTC7
#  define PC7 PORTC7
#endif
#ifdef PORTD0
#  define PD0 PORTD0
#endif
#ifdef PORTD1
#  define PD1 PORTD1
#endif
#ifdef PORTD2
#  define PD2 PORTD2
#endif
#ifdef PORTD3
#  define PD3 PORTD3
#endif
#ifdef PORTD4
#  define PD4 PORTD4
#endif
#ifdef PORTD5
#  define PD5 PORTD5
#endif
#ifdef PORTD6
#  define PD6 PORTD6
#endif
#ifdef PORTD7
#  define PD7 PORTD7
#endif
#  include "dev/ATmega1284P.h"
#elif defined (__AVR_ATmega328P__)
// They went and changed the names of the ports!
#ifdef PORTA0
#  define PA0 PORTA0
#endif
#ifdef PORTA1
#  define PA1 PORTA1
#endif
#ifdef PORTA2
#  define PA2 PORTA2
#endif
#ifdef PORTA3
#  define PA3 PORTA3
#endif
#ifdef PORTA4
#  define PA4 PORTA4
#endif
#ifdef PORTA5
#  define PA5 PORTA5
#endif
#ifdef PORTA6
#  define PA6 PORTA6
#endif
#ifdef PORTA7
#  define PA7 PORTA7
#endif
#ifdef PORTB0
#  define PB0 PORTB0
#endif
#ifdef PORTB1
#  define PB1 PORTB1
#endif
#ifdef PORTB2
#  define PB2 PORTB2
#endif
#ifdef PORTB3
#  define PB3 PORTB3
#endif
#ifdef PORTB4
#  define PB4 PORTB4
#endif
#ifdef PORTB5
#  define PB5 PORTB5
#endif
#ifdef PORTB6
#  define PB6 PORTB6
#endif
#ifdef PORTB7
#  define PB7 PORTB7
#endif
#ifdef PORTC0
#  define PC0 PORTC0
#endif
#ifdef PORTC1
#  define PC1 PORTC1
#endif
#ifdef PORTC2
#  define PC2 PORTC2
#endif
#ifdef PORTC3
#  define PC3 PORTC3
#endif
#ifdef PORTC4
#  define PC4 PORTC4
#endif
#ifdef PORTC5
#  define PC5 PORTC5
#endif
#ifdef PORTC6
#  define PC6 PORTC6
#endif
#ifdef PORTC7
#  define PC7 PORTC7
#endif
#ifdef PORTD0
#  define PD0 PORTD0
#endif
#ifdef PORTD1
#  define PD1 PORTD1
#endif
#ifdef PORTD2
#  define PD2 PORTD2
#endif
#ifdef PORTD3
#  define PD3 PORTD3
#endif
#ifdef PORTD4
#  define PD4 PORTD4
#endif
#ifdef PORTD5
#  define PD5 PORTD5
#endif
#ifdef PORTD6
#  define PD6 PORTD6
#endif
#ifdef PORTD7
#  define PD7 PORTD7
#endif
#  include "dev/ATmega328P.h"
#  define USART0_RX_vect USART_RX_vect
#  define USART0_TX_vect USART_TX_vect
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    error "device type not defined in your makefile"
#  else
#    error "This device is not currently supported"
#  endif
#endif




/* -------------------------------------------------
 *
 * Define timer interrupt vectors
 *
 --------------------------------------------------*/
#include "timer.h"

	// Interrupt vectors for the various timers
	// default type of interrupt handler to use for timers
	// *do not change unless you know what you're doing
	#ifndef TIMER_INTERRUPT_HANDLER
	#define TIMER_INTERRUPT_HANDLER		ISR
	#endif
	// -------------- Timer0 vectors --------------------------------------------
	#ifdef TIMER0_OVF_vect
	// Interrupt handler for tcnt0 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_OVF_vect){
		__timer_overflowService(TIMER0);
	}
	#endif

	#ifdef TIMER0_COMPA_vect
	// Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_COMPA_vect){
		__timer_compareService(TIMER0_COMPAREA);
	}
	#endif

	#ifdef TIMER0_COMPB_vect
	// Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_COMPB_vect){
		__timer_compareService(TIMER0_COMPAREB);
	}
	#endif

	#ifdef TIMER0_COMPC_vect
	// Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_COMPB_vect){
		__timer_compareService(TIMER0_COMPAREC);
	}
	#endif

	#ifdef TIMER0_COMP_vect
	#ifdef TIMER0_COMPAREA
	// Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_COMP_vect){
		__timer_compareService(TIMER0_COMPAREA);
	}
	#else
	// Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_COMP_vect){
		__timer_compareService(TIMER0_COMPARE);
	}
	#endif
	#endif

	#ifdef TIMER0_CAPT_vect
	// Interrupt handler for tcnt0 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER0_CAPT_vect){
		__timer_captureService(TIMER0);
	}
	#endif

	// -------------- Timer1 vectors --------------------------------------------

	#ifdef TIMER1_OVF_vect
	//! Interrupt handler for tcnt1 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_OVF_vect){
		__timer_overflowService(TIMER1);
	}
	#endif

	#ifdef TIMER1_COMPA_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_COMPA_vect){
		__timer_compareService(TIMER1_COMPAREA);
	}
	#endif // TIMER1_COMPA_vect

	#ifdef TIMER1_COMPB_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_COMPB_vect){
		__timer_compareService(TIMER1_COMPAREB);
	}
	#endif // TIMER1_COMPB_vect

	#ifdef TIMER1_COMPC_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_COMPC_vect){
		__timer_compareService(TIMER1_COMPAREC);
	}
	#endif //TIMER1_COMPC_vect

	#ifdef TIMER1_COMP_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_COMP_vect){
		__timer_compareService(TIMER1_COMPAREA);
	}
	#endif // TIMER1_COMP_vect

	#ifdef TIMER1_CAPT_vect
	// Interrupt handler for tcnt1 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER1_CAPT_vect){
		__timer_captureService(TIMER1);
	}
	#endif

	// -------------- Timer2 vectors --------------------------------------------

	#ifdef TIMER2_OVF_vect
	//! Interrupt handler for tcnt2 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_OVF_vect){
		__timer_overflowService(TIMER2);
	}
	#endif

	#ifdef TIMER2_COMPA_vect
	//! Interrupt handler for tcnt2 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_COMPA_vect){
		__timer_compareService(TIMER2_COMPAREA);
	}
	#endif

	#ifdef TIMER2_COMPB_vect
	//! Interrupt handler for tcnt2 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_COMPB_vect){
		__timer_compareService(TIMER2_COMPAREB);
	}
	#endif

	#ifdef TIMER2_COMPC_vect
	//! Interrupt handler for tcnt2 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_COMPC_vect){
		__timer_compareService(TIMER2_COMPAREC);
	}
	#endif

	#ifdef TIMER2_COMP_vect
	#ifdef TIMER2_COMPAREA
	//! Interrupt handler for tcnt2 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_COMP_vect){
		__timer_compareService(TIMER2_COMPAREA);
	}
	#else
	//! Interrupt handler for tcnt2 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_COMP_vect){
		__timer_compareService(TIMER2_COMPARE);
	}
	#endif
	#endif

	#ifdef TIMER2_CAPT_vect
	// Interrupt handler for tcnt2 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER2_CAPT_vect){
		__timer_captureService(TIMER2);
	}
	#endif
	// -------------- Timer3 vectors --------------------------------------------

	#ifdef TIMER3_OVF_vect
	//! Interrupt handler for tcnt3 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_OVF_vect){
		__timer_overflowService(TIMER3);
	}
	#endif

	#ifdef TIMER3_COMPA_vect
	//! Interrupt handler for tcnt3 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_COMPA_vect){
		__timer_compareService(TIMER3_COMPAREA);
	}
	#endif

	#ifdef TIMER3_COMPB_vect
	//! Interrupt handler for tcnt3 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_COMPB_vect){
		__timer_compareService(TIMER3_COMPAREB);
	}
	#endif

	#ifdef TIMER3_COMPC_vect
	//! Interrupt handler for tcnt3 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_COMPC_vect){
		__timer_compareService(TIMER3_COMPAREC);
	}
	#endif

	#ifdef TIMER3_COMP_vect
	//! Interrupt handler for tcnt3 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_COMP_vect){
		__timer_compareService(TIMER3_COMPAREA);
	}
	#endif

	#ifdef TIMER3_CAPT_vect
	// Interrupt handler for tcnt3 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER3_CAPT_vect){
		__timer_captureService(TIMER3);
	}
	#endif

	// -------------- Timer4 vectors --------------------------------------------

	#ifdef TIMER4_OVF_vect
	//! Interrupt handler for tcnt4 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_OVF_vect){
		__timer_overflowService(TIMER4);
	}
	#endif

	#ifdef TIMER4_COMPA_vect
	//! Interrupt handler for tcnt4 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_COMPA_vect){
		__timer_compareService(TIMER4_COMPAREA);
	}
	#endif

	#ifdef TIMER4_COMPB_vect
	//! Interrupt handler for tcnt4 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_COMPB_vect){
		__timer_compareService(TIMER4_COMPAREB);
	}
	#endif

	#ifdef TIMER4_COMPC_vect
	//! Interrupt handler for tcnt4 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_COMPC_vect){
		__timer_compareService(TIMER4_COMPAREC);
	}
	#endif

	#ifdef TIMER4_COMP_vect
	//! Interrupt handler for tcnt4 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_COMP_vect){
		__timer_compareService(TIMER4_COMPAREA);
	}
	#endif

	#ifdef TIMER4_CAPT_vect
	// Interrupt handler for tcnt4 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER4_CAPT_vect){
		__timer_captureService(TIMER4);
	}
	#endif

	// -------------- Timer5 vectors --------------------------------------------

	#ifdef TIMER5_OVF_vect
	//! Interrupt handler for tcnt5 overflow interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_OVF_vect){
		__timer_overflowService(TIMER5);
	}
	#endif

	#ifdef TIMER5_COMPA_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_COMPA_vect){
		__timer_compareService(TIMER5_COMPAREA);
	}
	#endif

	#ifdef TIMER5_COMPB_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_COMPB_vect){
		__timer_compareService(TIMER5_COMPAREB);
	}
	#endif

	#ifdef TIMER5_COMPC_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_COMPC_vect){
		__timer_compareService(TIMER5_COMPAREC);
	}
	#endif

	#ifdef TIMER5_COMP_vect
	//! Interrupt handler for tcnt0 compare interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_COMP_vect){
		__timer_compareService(TIMER5_COMPAREA);
	}
	#endif

	#ifdef TIMER5_CAPT_vect
	// Interrupt handler for tcnt5 capture interrupt
	TIMER_INTERRUPT_HANDLER(TIMER5_CAPT_vect){
		__timer_captureService(TIMER5);
	}
	#endif

#ifdef TCNT0
#undef TIMSK
#undef TCNT0
#undef TCCR0
#undef TCCR0A
#undef TCCR0B
#undef TIMSK0
#undef OCR0
#undef OCR0A
#undef OCR0B
#undef OCR0C
#endif // TCNT0

#ifdef TCNT1
#undef TIMSK
#undef TCNT1
#undef TCCR1
#undef TCCR1A
#undef TCCR1B
#undef TIMSK1
#undef OCR1
#undef OCR1A
#undef OCR1B
#undef OCR1C
#endif // TCNT1

#ifdef TCNT2
#undef TIMSK
#undef TCNT2
#undef TCCR2
#undef TCCR2A
#undef TCCR2B
#undef TIMSK2
#undef OCR2
#undef OCR2A
#undef OCR2B
#undef OCR2C
#endif // TCNT2

#ifdef TCNT3
#undef TIMSK
#undef TCNT3
#undef TCCR3
#undef TCCR3A
#undef TCCR3B
#undef TIMSK3
#undef OCR3
#undef OCR3A
#undef OCR3B
#undef OCR3C
#endif // TCNT3

#ifdef TCNT4
#undef TIMSK
#undef TCNT4
#undef TCCR4
#undef TCCR4A
#undef TCCR4B
#undef TIMSK4
#undef OCR4
#undef OCR4A
#undef OCR4B
#undef OCR4C
#endif // TCNT4

#ifdef TCNT5
#undef TIMSK
#undef TCNT5
#undef TCCR5
#undef TCCR5A
#undef TCCR5B
#undef TIMSK5
#undef OCR5
#undef OCR5A
#undef OCR5B
#undef OCR5C
#endif // TCNT5

// --------------- Define UART vectors -------------------
#include "uart.h"

// ----------------- Interrupt routines and helpers for each UART --------------
// Type of interrupt handler to use for uart interrupts.
// Do not change unless you know what you're doing.
#ifndef UART_INTERRUPT_HANDLER
#define UART_INTERRUPT_HANDLER	ISR
#endif


#if defined(UBRR0) || defined(UBRRL)|| defined(UBRR0L)
// ------------------ UART 0 --------------------------------
MAKE_WRITER(uart0SendByte){
	return _uartSendByte(&Uarts[0]._uart_,byte);
}
MAKE_READER(uart0GetByte){
	return __uartGetByte(&Uarts[0]._uart_);
}

#ifndef USART0_TX_vect
# error Uart0 Tx complete vector undefined
#endif
#ifndef USART0_RX_vect
# error Uart0 Rx complete vector undefined
#endif
UART_INTERRUPT_HANDLER(USART0_TX_vect)
{
	uartTransmitService(&Uarts[0]);
}

UART_INTERRUPT_HANDLER(USART0_RX_vect)
{
	uartReceiveService(&Uarts[0]);
}
#endif


#if defined(UBRR1) || defined(UBRR1L)
// ------------------ UART 1 --------------------------------
MAKE_WRITER(uart1SendByte){
	return _uartSendByte(&Uarts[1]._uart_,byte);
}
MAKE_READER(uart1GetByte){
	return __uartGetByte(&Uarts[1]._uart_);
}

#ifndef USART1_TX_vect
# error Uart1 Tx complete vector undefined
#endif
#ifndef USART1_RX_vect
# error Uart1 Rx complete vector undefined
#endif

UART_INTERRUPT_HANDLER(USART1_TX_vect)
{
	uartTransmitService(&Uarts[1]);
}

UART_INTERRUPT_HANDLER(USART1_RX_vect)
{
	uartReceiveService(&Uarts[1]);
}
#endif


#ifdef UBRR2
// ------------------ UART 2 --------------------------------
MAKE_WRITER(uart2SendByte){
	return _uartSendByte(&Uarts[2]._uart_,byte);
}
MAKE_READER(uart2GetByte){
	return __uartGetByte(&Uarts[2]._uart_);
}

#ifndef USART2_TX_vect
# error Uart2 Tx complete vector undefined
#endif
#ifndef USART2_RX_vect
# error Uart2 Rx complete vector undefined
#endif

UART_INTERRUPT_HANDLER(USART2_TX_vect)
{
	uartTransmitService(&Uarts[2]);
}

UART_INTERRUPT_HANDLER(USART2_RX_vect)
{
	uartReceiveService(&Uarts[2]);
}
#endif

#ifdef UBRR3
// ------------------ UART 3 --------------------------------
MAKE_WRITER(uart3SendByte){
	return _uartSendByte(&Uarts[3]._uart_,byte);
}
MAKE_READER(uart3GetByte){
	return __uartGetByte(&Uarts[3]._uart_);
}
#ifndef USART3_TX_vect
# error Uart3 Tx complete vector undefined
#endif
#ifndef USART3_RX_vect
# error Uart3 Rx complete vector undefined
#endif

UART_INTERRUPT_HANDLER(USART3_TX_vect)
{
	uartTransmitService(&Uarts[3]);
}
UART_INTERRUPT_HANDLER(USART3_RX_vect)
{
	uartReceiveService(&Uarts[3]);
}
#endif


#endif

