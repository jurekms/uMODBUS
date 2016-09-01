/*
 * port.c
 *
 *  Created on: 26 lip 2015
 *  Author: Jerzy Maczenski JMS
 */

#include "avr/io.h"
#include "avr/interrupt.h"

#include "config.h"
#include "mbIO.h"
#include "mb.h"


/***************************************************************************************************************************************************************************************
 *
 *Init UART RX/TX parameters
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/
void initUART(void) {
	UBRRH = (uint8_t) ( UART_UBRR >> 8);
	UBRRL = (uint8_t) ( UART_UBRR);
#ifdef URSEL
	UCSRC = 1 << URSEL | UCSRC_DATA_BITS | UCSRC_PARITY | UCSRC_STOP_BITS;
#else
	UCSRC = UCSRC_DATA_BITS | UCSRC_PARITY | UCSRC_STOP_BITS;
#endif
#ifdef USE_RS485
	RS485_PIN_DIRECTION_SET_OUT;
	RS485_SET_RX;
#endif
	UART_TX_ENABLE;
	UART_RX_ENABLE;
	UART_ENABLE_RX_INT;
}



/***************************************************************************************************************************************************************************************
 *
 *Init timer for count 3 and 1/2 time for modbus communiaction (recive frame)
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/
void initTimer(void) {
	T35_SETUP;
}



/***************************************************************************************************************************************************************************************
 *
 *Send prepared modbus frame
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/
void mbSendFrame(void) {
	uint16_t crc;

	MB_PROTOCOL_STATE.state = MB_ST_IS_TX;
	MB_PROTOCOL_STATE.codeError = MB_ERR_OK;

	if (MB_ACTUAL_FRAME_SIZE + 2 > MB_MAX_FRAME_SIZE) {
		MB_PROTOCOL_STATE.state = MB_ST_ERR_TX;
		MB_PROTOCOL_STATE.codeError = MB_ERR_BUFFER_OVERLOAD;
	} else {

		crc = mbUtilCalculateCRC16();												//calculate CRC16
		crc = mbUtilSwapUint16(crc);												//swap CRC16
		MB_ACTUAL_FRAME_SIZE = MB_ACTUAL_FRAME_SIZE + 2;
		MB_FRAME_BUFFER.mbFrame[MB_ACTUAL_FRAME_SIZE - 1] = (uint8_t) (crc);		//put CRC16 into modbus frame (last two bytes)
		MB_FRAME_BUFFER.mbFrame[MB_ACTUAL_FRAME_SIZE - 2] =	(uint8_t) (crc >> 8);

		while (!(UCSRA & (1 << UDRE)));												//wait for UART RX buffer empty
		MB_TX_BUFFER_INDEX = 0;														//reset MB_TX_BUFFER_INDEX to first byte of buffer
#ifdef USE_RS485
		RS485_SET_TX;																//if RS485 is use then set it to TX
#endif
		UART_DISABLE_RX_INT;														//disable UART RX interrupt
		UART_ENABLE_TX_INT;															//enable UART TX interrupt and send all buffer
	}

}



/***************************************************************************************************************************************************************************************
 *
 *Interrupt TIMER0 rutine
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/
ISR(TIMER0_OVF_vect) {

	UART_DISABLE_RX_INT;
	T35_TOV0_RESET;
	T35_I_TIMER_DISABLE;

	if (MB_PROTOCOL_STATE.state == MB_ST_IS_RX) {
		MB_PROTOCOL_STATE.state = MB_ST_DONE_RX;
		MB_PROTOCOL_STATE.codeError = MB_ERR_OK;
		MB_ACTUAL_FRAME_SIZE = MB_RX_BUFFER_INDEX;

	}

}



/***************************************************************************************************************************************************************************************
 *
 *Interrupt USART RX rutine
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/
ISR(USART_RX_vect) {

	if (MB_PROTOCOL_STATE.state == MB_ST_IS_IDDLE) {
		MB_PROTOCOL_STATE.state = MB_ST_IS_RX;
		MB_RX_BUFFER_INDEX = 0;
		T35_TIMER_RESET;
		T35_TOV0_RESET;
		T35_I_TIMER_ENABLE;
	}

	if (MB_PROTOCOL_STATE.state == MB_ST_IS_RX) {
		if (MB_RX_BUFFER_INDEX < MB_MAX_FRAME_SIZE) {
			MB_FRAME_BUFFER.mbFrame[MB_RX_BUFFER_INDEX++] = UDR;
			T35_TIMER_RESET;
		} else {
			MB_PROTOCOL_STATE.state = MB_ST_ERR_RX;
			MB_PROTOCOL_STATE.codeError = MB_ERR_BUFFER_OVERLOAD;
		}

	}

}



/***************************************************************************************************************************************************************************************
 *
 *Interrupt USART TX rutine
 *
 *@param	void
 *
 *@return	void
 *
 **************************************************************************************************************************************************************************************/

ISR(USART_UDRE_vect) {

	if (MB_TX_BUFFER_INDEX < MB_ACTUAL_FRAME_SIZE) {
		UCSRA = 1 << TXC;
		UDR = MB_FRAME_BUFFER.mbFrame[MB_TX_BUFFER_INDEX++];							//next byte from buffer to UART send
	} else {
		while (!(UCSRA & (1 << TXC)));													//send byte ?
#ifdef USE_RS485
		RS485_SET_RX;																	//if RS485 is use then set it to RX
#endif
		UART_DISABLE_TX_INT;															//disable UART TX interrupt
		UART_ENABLE_RX_INT;																//enable UART RX interrupt
		MB_PROTOCOL_STATE.state = MB_ST_DONE_TX;
		MB_PROTOCOL_STATE.codeError = MB_ERR_OK;
	}

}
