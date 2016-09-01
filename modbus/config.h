/*
 * config.h
 *
 *  Created on: 26 lip 2015
 *  Author: Jerzy Maczenski JMS
 */

#ifndef CONFIG_H_
#define CONFIG_H_


// UART parameters
#define UART_SPEED						19200
#define UART_DATA_BITS 					8
#define UART_PARITY  					'n'
#define UART_STOP_BITS 					1

//use RS485?
#define USE_RS485

// define port and pin to change TX/RX of RS485
#define RS485_PORT_DIRECTION			PORTD
#define RS485_DDR_PORT_DIRECTION		DDRD
#define RS485_PIN_DIRECTION 			(1<<PD3)
#define	RS485_PIN_DIRECTION_SET_OUT 	RS485_DDR_PORT_DIRECTION |= RS485_PIN_DIRECTION

// TX/RX of RS485 macros
#define RS485_SET_TX					RS485_PORT_DIRECTION |= RS485_PIN_DIRECTION
#define RS485_SET_RX					RS485_PORT_DIRECTION &= ~RS485_PIN_DIRECTION



// Modbus SLAVE address
#define	MB_MY_SLAVE_ADDR				10



// Max numer of callback functions
#define MB_MAX_NUMER_CALLBACK_FUNCTIONS	4

#endif /* CONFIG_H_ */
