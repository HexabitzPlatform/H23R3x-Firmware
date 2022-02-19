/*
 BitzOS (BOS) V0.2.5 - Copyright (C) 2017-2021 Hexabitz
 All rights reserved

 File Name     : H23Rx.h
 Description   : Header files for module H23R3.
    			 module (BlueNRG-M2
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H23R3_H
#define H23R3_H

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H23R3_MemoryMap.h"
#include "H23R3_uart.h"
#include "H23R3_gpio.h"
#include "H23R3_dma.h"
#include "H23R3_inputs.h"
#include "H23R3_eeprom.h"
/* Exported definitions -------------------------------------------------------*/

#ifdef H23R3
	#define	modulePN		_H23R3
#endif


/* Port-related definitions */
#define	NumOfPorts			6
#define P_PROG 				P2						/* ST factory bootloader UART */

/* Define available ports */
#define _P1
#define _P2
#define _P3
#define _P4
#define _P5
#define _P6

/* Define available USARTs */
#define _Usart1 1
#define _Usart2 1
#define _Usart3 1
#define _Usart4 1
#define _Usart5 1
#define _Usart6 1

/* Port-UART mapping */
#define P1uart &huart4
#define P2uart &huart2
#define P3uart &huart6
#define P4uart &huart1
#define P5uart &huart5
#define P6uart &huart3

/* Port Definitions */
#define	USART1_TX_PIN		GPIO_PIN_9
#define	USART1_RX_PIN		GPIO_PIN_10
#define	USART1_TX_PORT		GPIOA
#define	USART1_RX_PORT		GPIOA
#define	USART1_AF			GPIO_AF1_USART1

#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2

#define	USART4_TX_PIN		GPIO_PIN_0
#define	USART4_RX_PIN		GPIO_PIN_1
#define	USART4_TX_PORT		GPIOA
#define	USART4_RX_PORT		GPIOA
#define	USART4_AF			GPIO_AF4_USART4

#define	USART5_TX_PIN		GPIO_PIN_3
#define	USART5_RX_PIN		GPIO_PIN_4
#define	USART5_TX_PORT		GPIOB
#define	USART5_RX_PORT		GPIOB
#define	USART5_AF			GPIO_AF4_USART5

#define	USART6_TX_PIN		GPIO_PIN_4
#define	USART6_RX_PIN		GPIO_PIN_5
#define	USART6_TX_PORT		GPIOA
#define	USART6_RX_PORT		GPIOA
#define	USART6_AF			GPIO_AF5_USART6

/* Module-specific Definitions */
#define	USART3_TX_PIN			GPIO_PIN_10
#define	USART3_RX_PIN			GPIO_PIN_11
#define	USART3_RTS_PIN			GPIO_PIN_1
#define	USART3_CTS_PIN			GPIO_PIN_6
#define	USART3_TX_PORT			GPIOB
#define	USART3_RX_PORT			GPIOB
#define	USART3_RTS_PORT			GPIOB
#define	USART3_CTS_PORT			GPIOA
#define	USART3_AF				GPIO_AF4_USART3


#define NUM_MODULE_PARAMS		1

/* Module EEPROM Variables */

// Module Addressing Space 500 - 599
#define _EE_H23xVSP							500		// Temporary - H23Rx Bluetooth module VSP mode

/* H23R0_Status Type Definition */
typedef enum
{
	H23R3_OK = 0,
	H23R3_ERR_UnknownMessage = 1,
	H23R3_ERR_WrongParams,
	H23R3_RUN_VspCommandMode,
	H23R3_RUN_VspBridgeToUartMode,
	H23R3_RUN_DownloadScriptViaOta,
	H23R3_RUN_DownloadScriptViaUart,
	H23R3_ERROR = 255
} Module_Status;


//constants:

#define BT_Max_Number_Of_Devices 10
#define BT_Max_Device_Name_Length 30
#define BT_Device_Address_Length 12

#define BT_Command_Buffer_Length 500


//BLUENRG-2 Private Variables and Data structures:
extern uint8_t BT_User_Buffer_Length;


extern uint8_t BT_Devices_Name[BT_Max_Number_Of_Devices][BT_Max_Device_Name_Length];
extern uint8_t BT_Devices_Address[BT_Max_Number_Of_Devices][BT_Device_Address_Length];
extern uint8_t BT_Devices_Index;
extern uint8_t BT_Rx;
extern uint8_t BT_User_Buffer[192];
extern uint8_t BT_User_Buffer_Index;
extern uint8_t BT_Commands_Buffer[BT_Command_Buffer_Length];
extern uint8_t BT_Commands_Buffer_Index;
extern uint8_t BT_BOS_Index;

extern uint8_t* BT_User_Buffer_ptr;
extern uint8_t* BT_User_Buffer_beginning_ptr;
extern uint8_t* BT_User_Buffer_Index_ptr;


extern uint8_t BT_To_User_Buffer_flag;
//1: Bluetooth To User_Buffer
//0: Bluetooth To BOS Messaging Buffer

extern uint8_t BT_Connection_flag;
//1: Connected
//0: Disconnected


extern uint8_t BT_delete_connecting_char_flag;
extern uint8_t BT_delete_disconnecting_char_flag;

extern uint8_t BT_boot; //flag for sending name to BlueNRG Module on startup


extern uint8_t BT_Mac_Address_Buffer[6];
extern uint8_t BT_Mac_Address_Buffer_Index;
extern uint8_t BT_Receive_Mac_Address_Flag;




/*-------------------------------------------------------------------------------------*/


/* BlueNRG Private Function Prototypes-------------------------------------------*/
extern void BT_Get_Mac_Address();
extern Module_Status BT_Connect(uint8_t * Mac_Address, uint8_t len);



/* Indicator LED */
#define _IND_LED_PORT										GPIOA
#define _IND_LED_PIN										GPIO_PIN_11

#define PORT_BTC_CONN 										P6

/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART4_UART_Init(void);
extern void MX_USART5_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void SystemClock_Config(void);

/* handler for control bluetooth module task */

/* -----------------------------------------------------------------------
	|																APIs	 																 	|
   -----------------------------------------------------------------------
*/
extern void BT_Receive_Data_To_BOS(void);
extern Module_Status BT_Receive_Data(uint8_t* buffer,uint8_t size);
extern Module_Status BT_Send_Message(uint8_t dst,uint16_t code,uint16_t numberOfParams);
extern Module_Status BT_Send_Data(uint8_t* BT_Data, uint8_t length);
extern Module_Status BT_Clear_User_Buffer(void);
extern void BT_RESET_MODULE(void);
extern void BT_Disconnect(void);
extern Module_Status BT_Stream_To_Port(uint8_t port_number);
void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport);






/* -----------------------------------------------------------------------
	|															Commands																 	|
   -----------------------------------------------------------------------
*/
extern const CLI_Command_Definition_t btClearUserBufferCommandDefinition;
extern const CLI_Command_Definition_t btSendDataCommandDefinition;
extern const CLI_Command_Definition_t btDisconnectCommandDefinition;
extern const CLI_Command_Definition_t btStreamToPortCommandDefinition;
extern const CLI_Command_Definition_t btGetMacAddressDefinition;
extern const CLI_Command_Definition_t btConnectDefinition;





#endif /* H23R3_H */

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
