/*
 * mb.h
 *
 *  Created on: 26 lip 2015
 *  Author: Jerzy Maczenski JMS
 */
#ifndef MB_H_
#define MB_H_
#include "avr/io.h"
#include "config.h"

// 	definition of modbus frame
//	[ MB_ADDR_SIZE - 2 bytes ][ MB_FN_CODE_SIZE - 1 byte ][ MB_DATA_SIZE - n byte ][ MB_CRC_SIZE - 4 bytes ]
//	[                                MB_FRAME_SIZE - 7 + n bytes                                           ]

#define	MB_MAX_FRAME_SIZE				128
#define MB_MAX_BUFFER_SIZE 				MB_MAX_FRAME_SIZE - 1

#define MB_SIZE_ADDR					1
#define MB_SIZE_FN_TX					1
#define MB_SIZE_NUMBER_BYTES_TX			1
#define MB_SIZE_CRC						2
#define MB_SIZE_DUMMY					1


#define MB_SIZE_DATA_UINT8_TX			MB_MAX_FRAME_SIZE - MB_SIZE_ADDR - MB_SIZE_FN_TX - MB_SIZE_NUMBER_BYTES_TX- MB_SIZE_CRC
#define MB_SIZE_DATA_UINT16_TX			( MB_MAX_FRAME_SIZE - MB_SIZE_ADDR - MB_SIZE_FN_TX - MB_SIZE_NUMBER_BYTES_TX- MB_SIZE_CRC - MB_SIZE_DUMMY) / (2)



//states of state-machine modbus protocol
typedef enum {
	MB_ST_IS_IDDLE,										// system idle (raedy for RX or TX modbus frame)
	MB_ST_IS_RX,										// system is receving the frame
	MB_ST_DONE_RX,										// system recived the modbus frame, CRC - OK
	MB_ST_ERR_RX,										// error during receving the modbus frame
	MB_ST_IS_PROCESSING_RX_FRAME,						// system is processing recived frame
	MB_ST_DONE_PROCESSING_RX_FRAME,						// system has processed the received frame
	MB_ST_ERR_PROCESSING_RX_FRAME,						// error during prcessing recived frame
	MB_ST_IS_HANDLING_FUNCTIONS,						// is handling modbus function
	MB_ST_DONE_HANDLING_FUNCTIONS,						// handling modbus function is done with success
	MB_ST_ERR_HANDLING_FUNCTIONS,						// error during handle modbus function
	MB_ST_IS_TX,										// system is sending modbus frame
	MB_ST_DONE_TX,										// sending modbus frame is done with success
	MB_ST_ERR_TX,										// error during sending modbus frame
	MB_ST_THROW_EXCEPTION,								// throw exepction
	MB_ST_DONE_EXCEPTION								// exception frame is ready for sending
} TmbState;



//codes of error and feedback during modbus frame processing
typedef enum {
	MB_ERR_ILLEGAL_FUNCTION				=  0x51,		// unknow modbus function
	MB_PE_ILLEGAL_FUNCTION				=  0x01,		// feedback for MASTER: unknow modbus function
	MB_ERR_LEGAL_DATA_ADDR				=  0x72,		// correct data address
	MB_ERR_ILLEGAL_DATA_ADDR			=  0x52,		// incorrect data address
	MB_PE_ILLEGAL_DATA_ADDR				=  0x02,		// feedback for MASTER: incorrect data address
	MB_ERR_LEEGAL_DATA_VALUE			=  0x73,		// correct data
	MB_ERR_ILLEEGAL_DATA_VALUE			=  0x53,		// correct data
	MB_PE_ILLEEGAL_DATA_VALUE			=  0x03,		// feedback for MASTER: incorrect data
	MB_ERR_SLAVE_DEVICE_FAILURE			=  0x54,		// damage to the recipient SLAVE
	MB_PE_SLAVE_DEVICE_FAILURE			=  0x04,		// feedback for MASTER: damage to the recipient SLAVE
	MB_ERR_ACKNOWLEDGE 					=  0x55,		// the expected long response time from SLAVE
	MB_PE_ACKNOWLEDGE 					=  0x05,		// feedback for MASTER: the expected long response time from SLAVE
	MB_ERR_SLAVE_DEVICE_BUSY			=  0x56,		// SLAVE is busy
	MB_PE_SLAVE_DEVICE_BUSY				=  0x06,		// feedback for MASTER: SLAVE is busy
	MB_ERR_NEGATIVE_ACKNOWLEDGE			=  0x57,		// SLAVE requires diagnostic
	MB_PE_NEGATIVE_ACKNOWLEDGE			=  0x07,		// feedback for MASTER: SLAVE requires diagnostic
	MB_ERR_MEMORY_PARITY_ERR			=  0x58,		// memory parity error
	MB_PE_MEMORY_PARITY_ERR				=  0x08,		// feedback for MASTER: memory parity error
	MB_ERR_OK 							=  0x00,		// no errors
	MB_ERR_VALID_CRC					=  0x90,		// CRC OK
	MB_ERR_INVALID_CRC					=  0x91,		// CRC error
	MB_ERR_VALID_SIZE_FRAME 			=  0x92,		// corect lenght of modbus frame
	MB_ERR_INVALID_SIZE_FRAME 			=  0x93,		// incorect lenght of modbus frame
	MB_ERR_BUFFER_OVERLOAD 				=  0x94,		// buffer overload
	MB_ERR_VALID_SLAVE_ADDR				=  0x95,		// SLAVE address sent by the MASTER matches the device address
	MB_ERR_INVALID_SLAVE_ADDR			=  0x96,		// SLAVE address sent by the MASTER doesn't matche the device address
	MB_ERR_INVALID_CALLBACK_FUNCTION	=  0x97			// callback function didn't call
} TmbError;



//codes of modbus function
typedef enum {
	MB_FN_READ_COILS 						= 0x01,			// requesting the ON/OFF status of discrete coil
	MB_FN_WRITE_SINGLE_COIL					= 0x05,			// writing the contents of discrete coil (one coil)
	MN_FN_WRITE_MULTIPLE_COILS				= 0x0F,			// writing the contents of a series of n x 8 discrete coils (multiple coils)
	MB_FN_READ_INPUT_REGISTERS				= 0x04,			// requesting the content of analog input register
	MB_FN_WRITE_HOLDING_REGISTER			= 0x06,			// writing the contents of analog output holding register (one register or one cell memory)
	MB_FN_WRITE_HOLDING_REGISTERS			= 0x10,			// writing the contents of multiple holding registers
	MB_FN_READ_DISCRET_INPUTS 				= 0x02,			// requesting the ON/OFF status of discrete inputs (multiple binary inputs)
	MB_FN_READ_HOLDING_REGISTERS			= 0x03			// requesting the content of analog output holding registers (multiple cells memory)
} TmbFunction;



//structures for modbus frame

//for 0x0f function (writing the contents of multiple holding registers)
typedef struct {
	uint16_t startAddress;
	uint16_t numberRegs;
} TMBTxFn16;

typedef struct {
	uint16_t startAddress;
	uint16_t numberRegs;
	uint8_t  numberBytes;
	uint16_t dataUint16[ MB_SIZE_DATA_UINT16_TX - 1  ];
} TMBRxFn16;

//for 0x05 function (writing the contents of discrete coil)
typedef struct {
	uint16_t coilAddress;
	uint16_t coilState;
} TMBTxFn05;

typedef struct {
	uint16_t coilAddress;
	uint16_t coilState;
} TMBRxFn05;

//for 0x04 function (requesting the content of analog input register)
typedef struct {
	uint8_t  numberBytes;
	uint16_t data_uint16[ MB_SIZE_DATA_UINT16_TX  ];
} TMBTxFn04;

typedef struct {
	uint16_t startAddress;
	uint16_t numberRegs;
} TMBRxFn04;

//for 0x01 function (requesting the ON/OFF status of discrete coils)
typedef struct {
	uint16_t startAddress;
	uint16_t numberCoils;
} TMBRxFn01;

typedef struct {
	uint8_t numberBytes;
	uint8_t data_uint8[ MB_SIZE_DATA_UINT8_TX  ];
} TMBTxFn01;

//for 0x03 function (requesting the content of analog output holding registers)
typedef struct {
	uint16_t startAddress;
	uint16_t numberRegs;
} TMBRxFn03;

typedef struct {
	uint8_t  numberBytes;
	uint16_t data_uint16[ MB_SIZE_DATA_UINT16_TX  ];
} TMBTxFn03;

//for exception
typedef struct {
	uint8_t exceptionCode;
} TMBTxEception;

// complete structure of modbus frame
typedef union {
	uint8_t mbFrame[ MB_MAX_FRAME_SIZE ];
	struct {
		uint8_t		slaveAddr;
		uint8_t		codeFunction;
		union {
			uint8_t		dataUint8[ MB_SIZE_DATA_UINT8_TX ];
			uint16_t	dataUint16[ MB_SIZE_DATA_UINT16_TX ];
			TMBRxFn16	RxFn16;
			TMBTxFn16	TxFn16;
			TMBRxFn05	RxFn05;
			TMBTxFn05	TxFn05;
			TMBRxFn04	RxFn04;
			TMBTxFn04	TxFn04;
			TMBRxFn01	RxFn01;
			TMBTxFn01	TxFn01;
			TMBRxFn03	RxFn03;
			TMBTxFn03	TxFn03;
			TMBTxEception exception;
		};
		uint16_t	crc;
	};
} TMBFrame ;



//structure for description states of state-machine, and errors
typedef struct {
	TmbState	state;
	TmbError	codeError;
} TMBState;


//definition of callback function
typedef uint8_t ( *TMBPtrCallbackFunction )( void );

//structure for description of callback function in global table MB_CALLBACK_FUNCTIONS
typedef struct {
	uint8_t					codeFunction;
	TMBPtrCallbackFunction	ptrFunction;
} TMBCallbackFunction;


//global variables

volatile TMBCallbackFunction	MB_CALLBACK_FUNCTIONS[ MB_MAX_NUMER_CALLBACK_FUNCTIONS ]; 	// callback functions table
volatile TMBFrame				MB_FRAME_BUFFER ;											// RX TX buffer

volatile uint8_t				MB_RX_BUFFER_INDEX;											// RX buffer pointer (during reciving data)
volatile uint8_t				MB_TX_BUFFER_INDEX;											// TX buffer pointer (during reciving data)
volatile uint8_t				MB_ACTUAL_FRAME_SIZE;										// size of RX/TX frame

volatile TMBState				MB_PROTOCOL_STATE;											// actual state of protocol (state machine), and error
//volatile TmbError				MB_ERR;														// errors during procesing frame



//function for modbus
uint16_t mbUtilSwapUint16( uint16_t _uint16 );
uint8_t  mbRegisterFunctionHandler(uint8_t functionPosition ,TmbFunction functionCode  ,TMBPtrCallbackFunction ptrFunction );
void mbAnalizeRecivedPacket( void );
void mbHandleExceptionRespons( void );
void mbPoll( void );
void mbInit(void);
uint16_t mbUtilCalculateCRC16( void );




#endif /* MB_H_ */
