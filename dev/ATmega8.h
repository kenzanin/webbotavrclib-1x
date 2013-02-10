// -- This file has been autogenerated - do not modify by hand
// -- This file is automatically included when necessary, don't include it in your own program
#ifndef _DEV_ATmega8_H_
#define _DEV_ATmega8_H_

#include "../libdefs.h"

#define EEPROM_SIZE 512
// -- include EEPROM support --
#include <avr/eeprom.h>
// -------------- Define the I/O ports that haven't been disabled by the system file ------------
#ifdef PORTB
// Define Port B
	IOPort PROGMEM _PORTB={_SFR_MEM_ADDR(PORTB),_SFR_MEM_ADDR(DDRB),_SFR_MEM_ADDR(PINB)};
# ifdef PB0
	IOPin PROGMEM _B0 = { &_PORTB, BV(PB0)};
#	define B0 &_B0
#	undef PB0
# endif
# ifdef PB1
	IOPin PROGMEM _B1 = { &_PORTB, BV(PB1)};
#	define B1 &_B1
#	undef PB1
# endif
# ifdef PB2
	IOPin PROGMEM _B2 = { &_PORTB, BV(PB2)};
#	define B2 &_B2
#	undef PB2
# endif
# ifdef PB3
	IOPin PROGMEM _B3 = { &_PORTB, BV(PB3)};
#	define B3 &_B3
#	undef PB3
# endif
# ifdef PB4
	IOPin PROGMEM _B4 = { &_PORTB, BV(PB4)};
#	define B4 &_B4
#	undef PB4
# endif
# ifdef PB5
	IOPin PROGMEM _B5 = { &_PORTB, BV(PB5)};
#	define B5 &_B5
#	undef PB5
# endif
# ifdef PB6
	IOPin PROGMEM _B6 = { &_PORTB, BV(PB6)};
#	define B6 &_B6
#	undef PB6
# endif
# ifdef PB7
	IOPin PROGMEM _B7 = { &_PORTB, BV(PB7)};
#	define B7 &_B7
#	undef PB7
# endif
//undefine pins to stop them being used directly
# undef PORTB
# undef DDRB
# undef PINB
#endif

#ifdef PORTC
// Define Port C
	IOPort PROGMEM _PORTC={_SFR_MEM_ADDR(PORTC),_SFR_MEM_ADDR(DDRC),_SFR_MEM_ADDR(PINC)};
# ifdef PC0
	IOPin PROGMEM _C0 = { &_PORTC, BV(PC0)};
#	define C0 &_C0
#	undef PC0
# endif
# ifdef PC1
	IOPin PROGMEM _C1 = { &_PORTC, BV(PC1)};
#	define C1 &_C1
#	undef PC1
# endif
# ifdef PC2
	IOPin PROGMEM _C2 = { &_PORTC, BV(PC2)};
#	define C2 &_C2
#	undef PC2
# endif
# ifdef PC3
	IOPin PROGMEM _C3 = { &_PORTC, BV(PC3)};
#	define C3 &_C3
#	undef PC3
# endif
# ifdef PC4
	IOPin PROGMEM _C4 = { &_PORTC, BV(PC4)};
#	define C4 &_C4
#	undef PC4
# endif
# ifdef PC5
	IOPin PROGMEM _C5 = { &_PORTC, BV(PC5)};
#	define C5 &_C5
#	undef PC5
# endif
# ifdef PC6
	IOPin PROGMEM _C6 = { &_PORTC, BV(PC6)};
#	define C6 &_C6
#	undef PC6
# endif
# ifdef PC7
	IOPin PROGMEM _C7 = { &_PORTC, BV(PC7)};
#	define C7 &_C7
#	undef PC7
# endif
//undefine pins to stop them being used directly
# undef PORTC
# undef DDRC
# undef PINC
#endif

#ifdef PORTD
// Define Port D
	IOPort PROGMEM _PORTD={_SFR_MEM_ADDR(PORTD),_SFR_MEM_ADDR(DDRD),_SFR_MEM_ADDR(PIND)};
# ifdef PD0
	IOPin PROGMEM _D0 = { &_PORTD, BV(PD0)};
#	define D0 &_D0
#	undef PD0
# endif
# ifdef PD1
	IOPin PROGMEM _D1 = { &_PORTD, BV(PD1)};
#	define D1 &_D1
#	undef PD1
# endif
# ifdef PD2
	IOPin PROGMEM _D2 = { &_PORTD, BV(PD2)};
#	define D2 &_D2
#	undef PD2
# endif
# ifdef PD3
	IOPin PROGMEM _D3 = { &_PORTD, BV(PD3)};
#	define D3 &_D3
#	undef PD3
# endif
# ifdef PD4
	IOPin PROGMEM _D4 = { &_PORTD, BV(PD4)};
#	define D4 &_D4
#	undef PD4
# endif
# ifdef PD5
	IOPin PROGMEM _D5 = { &_PORTD, BV(PD5)};
#	define D5 &_D5
#	undef PD5
# endif
# ifdef PD6
	IOPin PROGMEM _D6 = { &_PORTD, BV(PD6)};
#	define D6 &_D6
#	undef PD6
# endif
# ifdef PD7
	IOPin PROGMEM _D7 = { &_PORTD, BV(PD7)};
#	define D7 &_D7
#	undef PD7
# endif
//undefine pins to stop them being used directly
# undef PORTD
# undef DDRD
# undef PIND
#endif

// -------------- End of I/O ports --------------------------------------------------------------

// -------------- Define the PWM pins that haven't been disabled by the system file -------------
#ifdef B1
# define PWM1A B1
#else
# define PWM1A null
#endif
#ifdef B2
# define PWM1B B2
#else
# define PWM1B null
#endif
#ifdef B3
# define PWM2 B3
#else
# define PWM2 null
#endif
// -------------- End of PWM pins ---------------------------------------------------------------

// -------------- Define the timers -------------------------------------------------------------

// Create Timer 0
TimerData __timer0_data = MAKE_TIMER_DATA(0);

// Create Timer 1
TimerData __timer1_data = MAKE_TIMER_DATA(0);
TimerDataCompare __timer1CompareA_data = MAKE_TIMER_COMPARE_DATA();
TimerDataCompare __timer1CompareB_data = MAKE_TIMER_COMPARE_DATA();
TimerCompare PROGMEM __timer1Compare[] = {
	MAKE_TIMER_COMPARE(__timer1CompareA_data,1,TIMSK,OCIE1A,OCR1A,TIFR,OCF1A,TCCR1A,COM1A0,PWM1A),
	MAKE_TIMER_COMPARE(__timer1CompareB_data,1,TIMSK,OCIE1B,OCR1B,TIFR,OCF1B,TCCR1A,COM1B0,PWM1B)
};
#define TIMER1_COMPAREA &__timer1Compare[0]
#define TIMER1_COMPAREB &__timer1Compare[1]

// Create Timer 2
TimerData __timer2_data = MAKE_TIMER_DATA(0);
TimerDataCompare __timer2Compare_data = MAKE_TIMER_COMPARE_DATA();
TimerCompare PROGMEM __timer2Compare[] = {
	MAKE_TIMER_COMPARE(__timer2Compare_data,2,TIMSK,OCIE2,OCR2,TIFR,OCF2,TCCR2,COM20,PWM2)
};
#define TIMER2_COMPARE &__timer2Compare[0]

// Input Capture pins for each timer
#define InputCapture0 null
#ifdef B0
#define InputCapture1 B0
#else
#define InputCapture1 null
#endif
#define InputCapture2 null

const PROGMEM Timer PROGMEM pgm_Timers[] = {
	MAKE_TIMER(__timer0_data,TCNT0,TCCR0, FALSE, FALSE, TIMER_NO_MODES,UNUSED_PORT,0,UNUSED_PORT,0,UNUSED_PORT,0,UNUSED_PORT,0,null, TIMSK,TOIE0, TIFR,TOV0, UNUSED_PORT, UNUSED_PORT,0, UNUSED_PORT,0, UNUSED_PORT,0, InputCapture0),
	MAKE_TIMER(__timer1_data,TCNT1,TCCR1B, TRUE, FALSE, TIMER_ALL_MODES,TCCR1A,WGM10,TCCR1A,WGM11,TCCR1B,WGM12,TCCR1B,WGM13,__timer1Compare, TIMSK,TOIE1, TIFR,TOV1, ICR1, TIMSK,TICIE1, TIFR,ICF1, TCCR1B,ICES1, InputCapture1),
	MAKE_TIMER(__timer2_data,TCNT2,TCCR2, FALSE, TRUE, TIMER_2BIT_MODES,TCCR2,WGM20,TCCR2,WGM21,UNUSED_PORT,0,UNUSED_PORT,0,__timer2Compare, TIMSK,TOIE2, TIFR,TOV2, UNUSED_PORT, UNUSED_PORT,0, UNUSED_PORT,0, UNUSED_PORT,0, InputCapture2)
};
const uint8_t NUMBER_OF_TIMERS =  (sizeof(pgm_Timers)/sizeof(Timer));
#define TIMER0 &pgm_Timers[0]
#define TIMER1 &pgm_Timers[1]
#define TIMER2 &pgm_Timers[2]
// -------------- End of timer definitions ------------------------------------------------------

// -------------- Define the UARTs --------------------------------------------------------------
#include "../uart.h"
// -- Create receive buffers
#ifdef UART_RX_BUFFER_SIZE
# ifndef UART_RX_BUFFER_SIZE
   // Use default value
#  define UART_RX_BUFFER_SIZE  UART_RX_BUFFER_SIZE
# endif
#endif


#ifdef UART_RX_BUFFER_SIZE
  unsigned char rxBuf[UART_RX_BUFFER_SIZE];
  cBuffer rxBuffer = MAKE_BUFFER(rxBuf);
# define rxB &rxBuffer
#else
# define rxB null
#endif

// -- Create transmit buffers
#ifdef UART_TX_BUFFER_SIZE
# ifndef UART_TX_BUFFER_SIZE
   // Use default value
#  define UART_TX_BUFFER_SIZE  UART_TX_BUFFER_SIZE
# endif
#endif


#ifdef UART_TX_BUFFER_SIZE
  unsigned char txBuf[UART_TX_BUFFER_SIZE];
  cBuffer txBuffer = MAKE_BUFFER(txBuf);
# define txB &txBuffer
#else
# define txB null
#endif

// -- Define the pin used for the SPI clock, if supported, for each uart
#define UART_SPI_CLOCK null


#define _NUM_UARTS 1
const uint8_t NUM_UARTS = _NUM_UARTS;

HW_UART Uarts[] = {
	MAKE_UART_BUFFERED(rxB,txB,UCSRA,UCSRB,UBRRL,UBRRH,UDR,UART_SPI_CLOCK,2,D0,D1,&uart0GetByte,&uart0SendByte)
};

#define UART0 &Uarts[0]
// -------------- End of UART definitions -------------------------------------------------------

// -------------- Start of ADC channel definitions ----------------------------------------------
#define ADC0 ADC_NUMBER_TO_CHANNEL(0)
#define ADC1 ADC_NUMBER_TO_CHANNEL(1)
#define ADC2 ADC_NUMBER_TO_CHANNEL(2)
#define ADC3 ADC_NUMBER_TO_CHANNEL(3)
#define ADC4 ADC_NUMBER_TO_CHANNEL(4)
#define ADC5 ADC_NUMBER_TO_CHANNEL(5)
const uint8_t NUM_ADC_CHANNELS = 6;
// -------------- End of ADC channel definitions ------------------------------------------------

// -------------- Define which IOPin is used by the pin change interrupts --------------------------------
// -------------- End of pin change definitions - ------------------------------------------------

#endif
