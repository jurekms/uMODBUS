/*
 * port.h
 *
 *  Created on: 26 lip 2015
 *  Author: Jerzy Maczenski JMS
 */

#ifndef MBIO_H_
#define MBIO_H_

#include "config.h"

void initTimer( void );
void initUART( void );
void mbSendFrame( void );



// Timer configuration - this timer is use in order to check time 3 and 1/2 (3,5) of .....
#if UART_SPEED >= 19200
	#define	T35_TIMER			220
#else
	#define T35_TIMER			200
#endif

#ifdef TCCR0B
	#define TCCR0 TCCR0B
#endif

#ifdef TIMSK0
	#define TIMSK TIMSK0
#endif

#ifdef TIFR0
	#define TIFR TIFR0
#endif

#define T35_SETUP 			TCCR0 |= (1<<CS00) | (1<<CS02) 		//source : CLK, prescaler: 1024
#define	T35_I_TIMER_ENABLE	TIMSK |= (1<< TOIE0)            	//turn on interrupt from overflow timer
#define	T35_I_TIMER_DISABLE	TIMSK &= ~(1<< TOIE0)           	//turn off interrupt from overflow timer
#define T35_TOV0_RESET		TIFR = (1<<TOV0)					//reset timer
#define T35_TIMER_RESET 	TCNT0 = T35_TIMER



// change name of UART registers
#ifndef UCSRA
    #define UCSRA UCSR0A
#endif
#ifndef UCSRB
    #define UCSRB UCSR0B
#endif
#ifndef UCSRC
    #define UCSRC UCSR0C
#endif
#ifndef UBRRL
    #define UBRRL UBRR0L
#endif
#ifndef UBRRH
    #define UBRRH UBRR0H
#endif
#ifndef UDR
    #define UDR UDR0
#endif
#ifdef  USART_RX0_vect
	#define USART_RX_vect USART_RX0_vect
#endif

// change bits name in UCSRA register
#ifndef RXC
    #define RXC RXC0
#endif
#ifndef TXC
    #define TXC TXC0
#endif
#ifndef UDRE
    #define UDRE UDRE0
#endif
#ifndef FE
    #define FE FE0
#endif
#ifndef DOR
    #define DOR DOR0
#endif
#ifndef UPE
    #define UPE UPE0
#endif
#ifndef U2X
    #define U2X U2X0
#endif
#ifndef MPCM
    #define MPCM MPCM0
#endif

// change bits name in UCSRB register
#ifndef RXCIE
    #define RXCIE RXCIE0
#endif
#ifndef TXCIE
    #define TXCIE TXCIE0
#endif
#ifndef UDRIE
    #define UDRIE UDRIE0
#endif
#ifndef RXEN
    #define RXEN RXEN0
#endif
#ifndef TXEN
    #define TXEN TXEN0
#endif
#ifndef UCSZ2
    #define UCSZ2 UCSZ02
#endif
#ifndef RXB8
    #define RXB8 RXB80
#endif
#ifndef TXB8
    #define TXB8 TXB80
#endif

// change bits name in UCSRC register
#ifndef UMSEL
    #ifdef UMSEL0
        #define UMSEL UMSEL0
    #else
        #define UMSEL UMSEL00
        #define UMSEL0 UMSEL00
        #define UMSEL1 UMSEL01
    #endif
#endif
#ifndef UPM1
    #define UPM1 UPM01
#endif
#ifndef UPM0
    #define UPM0 UPM00
#endif
#ifndef USBS
    #define USBS USBS0
#endif
#ifndef UCSZ1
    #define UCSZ1 UCSZ01
#endif
#ifndef UCSZ0
    #define UCSZ0 UCSZ00
#endif
#ifndef UCPOL
    #define UCPOL UCPOL0
#endif

// set configurations bits for UART
#if UART_DATA_BITS == 5
    #define UCSRB_DATA_BITS ( 0 << UCSZ2 )
    #define UCSRC_DATA_BITS ( 0 << UCSZ1 | 0 << UCSZ0 )
#elif UART_DATA_BITS == 6
    #define UCSRB_DATA_BITS ( 0 << UCSZ2 )
    #define UCSRC_DATA_BITS ( 0 << UCSZ1 | 1 << UCSZ0 )
#elif UART_DATA_BITS == 7
    #define UCSRB_DATA_BITS ( 0 << UCSZ2 )
    #define UCSRC_DATA_BITS ( 1 << UCSZ1 | 0 << UCSZ0 )
#elif UART_DATA_BITS == 8
    #define UCSRB_DATA_BITS ( 0 << UCSZ2 )
    #define UCSRC_DATA_BITS ( 1 << UCSZ1 | 1 << UCSZ0 )
#elif UART_DATA_BITS == 9
    #define UCSRB_DATA_BITS ( 1 << UCSZ2 )
    #define UCSRC_DATA_BITS ( 1 << UCSZ1 | 1 << UCSZ0 )
#endif

#if UART_PARITY == 'n'
    #define UCSRC_PARITY ( 0 << UPM1 | 0 << UPM0 )
#elif UART_PARITY == 'e'
    #define UCSRC_PARITY ( 1 << UPM1 | 0 << UPM0 )
#elif UART_PARITY == 'o'
    #define UCSRC_PARITY ( 1 << UPM1 | 1 << UPM0 )
#endif

#if UART_STOP_BITS == 1
    #define UCSRC_STOP_BITS ( 0 << USBS )
#elif UART_STOP_BITS == 2
    #define UCSRC_STOP_BITS ( 1 << USBS )
#endif

#define UART_UBRR	( ( F_CPU ) / ( ( UART_SPEED ) * 16UL ) - 1 )

#define UART_ENABLE_RX_INT			UCSRB |= (1 << RXCIE)
#define UART_DISABLE_RX_INT			UCSRB &= ~(1 << RXCIE)

#define UART_ENABLE_TX_INT			UCSRB |= (1 << UDRIE)
#define UART_DISABLE_TX_INT			UCSRB &= ~(1 << UDRIE)

#define UART_RX_ENABLE				UCSRB |= (1 << RXEN)
#define UART_RX_DISABLE				UCSRB &= ~(1 << RXEN)

#define UART_TX_ENABLE				UCSRB |= (1 << TXEN)
#define UART_TX_DISABLE				UCSRB &= ~(1 << RXEN)


#endif /* MBIO_H_ */
