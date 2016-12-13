/*
 * main.c
 *
 *  Created on: 26 lip 2015
 *  Author: Jerzy Maczenski JMS
 */

#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"
#include "string.h"



#include "modbus/mb.h"
#include "modbus/mbIO.h"


//macros for led configuration port B pin 5, and switch ON/OFF
#define LED_1_PIN 		(1<<PB5)
#define LED_1_OFF 		PORTB &= ~LED_1_PIN 	//led ON
#define LED_1_ON 		PORTB |= LED_1_PIN 		//led OFF
#define LED_1_CHANGE 	PORTB ^= LED_1_PIN		//led SWITCH



//callback functions, for better transparency of code you should put this functions in another files (ex. callbackFunctions.c and callbackFunctions.h)




//function for set LED ON/OFF
uint8_t callbackSetCoil(void) {
	uint16_t coilID;
	uint16_t stateCoil;

	coilID = mbUtilSwapUint16(MB_FRAME_BUFFER.RxFn05.coilAddress);		//coil address
	stateCoil = mbUtilSwapUint16(MB_FRAME_BUFFER.RxFn05.coilState);		//coil status ==0 => LED OFF, !=0 => LED ON

	switch (coilID) {
	case 1:
		//set LED 1 ON/OFF
		if( stateCoil != 0 ) LED_1_ON;
		else LED_1_OFF;
		break;
	case 2:
		//to do : set another LED ON/OFF
		break;
	case 3:
		//to do : set another LED ON/OFF
		break;
	default:
		break;
	}
	return MB_ERR_OK;
}







//function for read holding registers

//register must be defined as a global variable
uint16_t	firstRegisterValue;
uint16_t	secondRegisterValue[2];
//


#define FIRST_REGISTER_ADDRESS 	5
#define SECOND_REGISTER_ADDRESS	7


uint8_t callbackReadHoldingRegisters( void ) {

	uint16_t registersStartAddress;														//reads from MODBUS request freame
	uint16_t registersNb;																//reads from MODBUS request freame
	uint8_t  bytesNb;																	//calculates from data sending by master

	registersStartAddress = mbUtilSwapUint16(MB_FRAME_BUFFER.RxFn03.startAddress); 		// requested register start address
	registersNb = mbUtilSwapUint16(MB_FRAME_BUFFER.RxFn03.numberRegs); 					// requested number of registers
	bytesNb = registersNb * 2;															// number of bytes sending back (if you want 2 regs = 4 bytes, 4 regs = 8 bytes so  on...)



	switch (registersStartAddress) {
		case FIRST_REGISTER_ADDRESS:																//send value of first register
			if(registersNb == 1) {																	//check - master want one register pointed by reg sddress?
				MB_FRAME_BUFFER.TxFn03.numberBytes = bytesNb;										//set number of bytes sending back to master
				MB_FRAME_BUFFER.TxFn03.data_uint16[0] =  mbUtilSwapUint16(firstRegisterValue);		//set value of register
				return MB_ERR_OK;																	//everything is OK we can send response to master
			}
			else {
				MB_PROTOCOL_STATE.codeError = MB_ERR_ILLEGAL_DATA_ADDR;								//if number of register+address is outside of the scope send MB_ERR_ILLEGAL_DATA_ADDR
			}
			break;
		case SECOND_REGISTER_ADDRESS:
			if(registersNb == 2) {																	//check - master want two registers pointed by reg sddress?
				MB_FRAME_BUFFER.TxFn03.numberBytes = bytesNb;										//set number of bytes sending back to master
				MB_FRAME_BUFFER.TxFn03.data_uint16[0] =  mbUtilSwapUint16(secondRegisterValue[0]);	//set value of registers
				MB_FRAME_BUFFER.TxFn03.data_uint16[1] =  mbUtilSwapUint16(secondRegisterValue[1]);
				return MB_ERR_OK;																	//everything is OK we can send response to master
			}
			else {
				MB_PROTOCOL_STATE.codeError = MB_ERR_ILLEGAL_DATA_ADDR;					//if number of register+address is outside of the scope send MB_ERR_ILLEGAL_DATA_ADDR
			}
			break;

		default:
				MB_PROTOCOL_STATE.codeError = MB_ERR_ILLEGAL_DATA_ADDR;					//if address is not FIRST_REGISTER_ADDRESS nor SECOND_REGISTER_SDDRES send MB_ERR_ILLEGAL_DATA_ADDR
			break;
		}
	return MB_PROTOCOL_STATE.codeError;
}








int main(void) {
	//pin5 port B as output configuration
	DDRB |= LED_1_PIN;
	PORTB |= LED_1_PIN;
	LED_1_OFF;

	//register callback function for handle MB_FN_WRITE_SINGLE_COIL modbus function
	mbRegisterFunctionHandler(0, MB_FN_WRITE_SINGLE_COIL, callbackSetCoil);

	//register callback function for handle MB_FN_READ_HOLDING_REGISTERS modbus function
	mbRegisterFunctionHandler(1, MB_FN_READ_HOLDING_REGISTERS, callbackReadHoldingRegisters);

	//init protocolo and UART
	mbInit();
	//enable interrupts
	sei();

	//never ending loop
	while (1 == 1) {
																					//here you can set registers
		firstRegisterValue = 10;													//first register for example data from AD converter
		secondRegisterValue[0] = 72;												//second registers  for example data from 18B20 (temperature)
		secondRegisterValue[1] = 34;
	//poll modbus state-machine
		mbPoll();
	//here do something else
	//...
	//---
	}
}



