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






int main(void) {
	//pin5 port B as output configuration
	DDRB |= LED_1_PIN;
	PORTB |= LED_1_PIN;
	LED_1_OFF;

	//register callback function for handle MB_FN_WRITE_SINGLE_COIL modbus function
	mbRegisterFunctionHandler(0, MB_FN_WRITE_SINGLE_COIL, callbackSetCoil);

	//init protocolo and UART
	mbInit();
	//enable interrupts
	sei();

	//never ending loop
	while (1 == 1) {
	//poll modbus state-machine
		mbPoll();
	//here do something else
	//...
	//---
	}
}



