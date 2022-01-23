/*
    BitzOS (BOS) V0.2.5 - Copyright (C) 2017-2021 Hexabitz
    All rights reserved

    File Name     : H23R3.c
    Description   : Source code for module H23R3.
    				module (BlueNRG-M2)
*/

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"


/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] = {{.paramPtr=NULL, .paramFormat=FMT_FLOAT, .paramName=""}};

/* Private variables ---------------------------------------------------------*/

/* Exported variables */
extern FLASH_ProcessTypeDef pFlash;
extern uint8_t numOfRecordedSnippets;

//BLUENRG-2 Private Variables and Data structures:
uint8_t BT_User_Buffer_Length = 192;


uint8_t BT_Devices_Name[BT_Max_Number_Of_Devices][BT_Max_Device_Name_Length] = {0};
uint8_t BT_Devices_Address[BT_Max_Number_Of_Devices][BT_Device_Address_Length]= {0};
uint8_t BT_Devices_Index=0;
uint8_t BT_Rx=0;
uint8_t BT_User_Buffer[192] = {0};
uint8_t BT_User_Buffer_Index = 0;
uint8_t BT_Commands_Buffer[BT_Command_Buffer_Length] = {0};
uint8_t BT_Commands_Buffer_Index = 0;
uint8_t BT_BOS_Index = 0;


uint8_t* BT_User_Buffer_ptr = BT_User_Buffer;
uint8_t* BT_User_Buffer_beginning_ptr = BT_User_Buffer;
uint8_t* BT_User_Buffer_Index_ptr = &BT_User_Buffer_Index;

uint8_t BT_To_User_Buffer_flag = 0;
//1: Bluetooth To User_Buffer
//0: Bluetooth To BOS Messaging Buffer

uint8_t BT_Connection_flag = 0;
//1: Connected
//0: Disconnected


uint8_t BT_delete_connecting_char_flag = 0;
uint8_t BT_delete_disconnecting_char_flag = 0;

uint8_t BT_boot = 1; //flag for sending name to BlueNRG Module on startup

uint8_t BT_Mac_Address_Buffer[6] = {0};
uint8_t BT_Mac_Address_Buffer_Index = 0;
uint8_t BT_Receive_Mac_Address_Flag = 0;

/*-------------------------------------------------------------------------------------*/
void bytes2hex (unsigned char *src, char *out)
{
	char HexLookUp[] = "0123456789abcdef";


        *out++ = HexLookUp[*src >> 4];
        *out++ = HexLookUp[*src & 0x0F];

}

/* BlueNRG Private Function Prototypes-------------------------------------------*/
void BT_Get_Mac_Address()
{

	char hex_display [2];
	writePxMutex(PORT_BTC_CONN,(char*)"\t\t\tm",4,cmd50ms, HAL_MAX_DELAY);
	Delay_ms(1000);
	for(int i=0;i<6;i++)
	{
		bytes2hex( (unsigned char*)&BT_Mac_Address_Buffer[i],hex_display);
		writePxMutex(PcPort,hex_display,2,cmd50ms, HAL_MAX_DELAY);
		if(i != 5) writePxMutex(PcPort,(char*)":",1,cmd50ms, HAL_MAX_DELAY);
		else writePxMutex(PcPort,(char*)"\n\r",2,cmd50ms, HAL_MAX_DELAY);

	}

}

/*-------------------------------------------------------------------------------------*/
Module_Status BT_Connect(uint8_t * Mac_Address,uint8_t len)
{
	Module_Status result = H23R3_OK;
	uint8_t mac_address_arr[6];
	uint8_t temp;

	if(len != 17)
	{
		result = H23R3_ERR_WrongParams;
		return result;
	}


		for(int i=0;i<6;i++)
		{
			switch(Mac_Address[i*3])
			{
			case '0': case '1': case '2': case '3': case '4':
			case '5':case '6': case '7': case '8': case '9':

				temp = ( (Mac_Address[i*3] - '0')*(16) );
				break;

			case 'a': case 'b': case 'c':
			case 'd': case 'e': case 'f':
				temp = ( (Mac_Address[i*3] - 'a')*(16) );
				break;

			case 'A': case 'B': case 'C':
			case 'D': case 'E': case 'F':
				temp = ( (Mac_Address[i*3] - 'A')*(16) );
				break;

			default:
				result = H23R3_ERR_WrongParams;
				return result;

			}

			switch(Mac_Address[i*3 + 1])
			{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				temp += ( (Mac_Address[i*3 + 1] - '0'));
				break;

			case 'a': case 'b': case 'c':
			case 'd': case 'e': case 'f':
				temp += ( (Mac_Address[i*3 + 1] - 'a'));
				break;

			case 'A': case 'B': case 'C':
			case 'D': case 'E': case 'F':
				temp += ( (Mac_Address[i*3 + 1] - 'A'));
				break;

			default:
				result = H23R3_ERR_WrongParams;
				return result;

			}

			if(i != 5)
			{
				if(Mac_Address[i*3 + 1] != ':')
				{
					result = H23R3_ERR_WrongParams;
					return result;
				}
			}
			mac_address_arr[5-i] = temp;

		}



		writePxMutex(PcPort,(char*)"\t\t\tc",4,cmd50ms, HAL_MAX_DELAY);
		writePxMutex(PcPort,(char*)mac_address_arr,6,cmd50ms, HAL_MAX_DELAY);

		return result;



}
/* Create CLI commands --------------------------------------------------------*/
static portBASE_TYPE btClearUserBuffer( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE btSendData( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE btDisconnect( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE btStreamToPort( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE btGetMacAddress( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE btConnect( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );


/* CLI command structure : bt-clear-user-buffer */
const CLI_Command_Definition_t btClearUserBufferCommandDefinition =
{
	( const int8_t * ) "bt-clear-user-buffer", /* The command string to type. */
	( const int8_t * ) "bt-clear-user-buffer:\r\n Clear Bluetooth User Data Buffer\r\n\r\n",
	btClearUserBuffer, /* The function to run. */
	0/* No Parameters are expected. */
};
//--------------------------------------------------------------------------
/* CLI command structure : bt-send-data */
const CLI_Command_Definition_t btSendDataCommandDefinition =
{
	( const int8_t * ) "bt-send-data", /* The command string to type. */
	( const int8_t * ) "bt-send-data:\r\n Send Data over BlueNRG Bluetooth Module \r\n\r\n",
	btSendData, /* The function to run. */
	1/* One Parameter is expected. */
};
//--------------------------------------------------------------------------
/* CLI command structure : bt-disconnect */
const CLI_Command_Definition_t btDisconnectCommandDefinition =
{
	( const int8_t * ) "bt-disconnect", /* The command string to type. */
	( const int8_t * ) "bt-disconnect:\r\n Disconnect BlueNRG module\r\n\r\n",
	btDisconnect, /* The function to run. */
	0/* No Parameters are expected. */
};
//--------------------------------------------------------------------------
/* CLI command structure : bt-stream-to-port */
const CLI_Command_Definition_t btStreamToPortDefinition =
{
	( const int8_t * ) "bt-stream-to-port", /* The command string to type. */
	( const int8_t * ) "bt-stream-to-port:\r\n Streaming BlueNRG data to another port\r\n\r\n",
	btStreamToPort, /* The function to run. */
	1/* One Parameter is expected. */
};
//--------------------------------------------------------------------------
/* CLI command structure : bt-get-mac-address */
const CLI_Command_Definition_t btGetMacAddressDefinition =
{
	( const int8_t * ) "bt-get-mac-address", /* The command string to type. */
	( const int8_t * ) "bt-get-mac-address:\r\n Getting BlueNRG MAC Address\r\n\r\n",
	btGetMacAddress, /* The function to run. */
	0/* No Parameters are expected. */
};
//--------------------------------------------------------------------------
/* CLI command structure : bt-connect */
const CLI_Command_Definition_t btConnectDefinition =
{
	( const int8_t * ) "bt-connect", /* The command string to type. */
	( const int8_t * ) "bt-connect:\r\n Connect to a  BlueNRG Device using its MAC Address\r\n\r\n",
	btConnect, /* The function to run. */
	1/* One Parameter is expected. */
};
//--------------------------------------------------------------------------

/* -----------------------------------------------------------------------
	|												 Private Functions	 														|
   -----------------------------------------------------------------------
*/

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PREDIV                         = 1
  *            PLLMUL                         = 6
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	
	__HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV32;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	

	__SYSCFG_CLK_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
}
/* --- Save array topology and Command Snippets in Flash RO --- 
*/
uint8_t SaveToRO(void)
{
	BOS_Status result = BOS_OK; 
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint16_t add = 2, temp = 0;
	uint8_t snipBuffer[sizeof(snippet_t)+1] = {0};
	
	HAL_FLASH_Unlock();
	
	/* Erase RO area */
	FLASH_PageErase(RO_START_ADDRESS);
	FlashStatus = FLASH_WaitForLastOperation((uint32_t)HAL_FLASH_TIMEOUT_VALUE); 
	if(FlashStatus != HAL_OK) {
		return pFlash.ErrorCode;
	} else {			
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}	
	
	/* Save number of modules and myID */
	if (myID)
	{
		temp = (uint16_t) (N<<8) + myID;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, RO_START_ADDRESS, temp);
		FlashStatus = FLASH_WaitForLastOperation((uint32_t)HAL_FLASH_TIMEOUT_VALUE); 
		if (FlashStatus != HAL_OK) {
			return pFlash.ErrorCode;
		} else {
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
		}			
	
	/* Save topology */
		for(uint8_t i=1 ; i<=N ; i++)
		{
			for(uint8_t j=0 ; j<=MaxNumOfPorts ; j++)
			{
				if (array[i-1][0]) {
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, RO_START_ADDRESS+add, array[i-1][j]);
					add += 2;
					FlashStatus = FLASH_WaitForLastOperation((uint32_t)HAL_FLASH_TIMEOUT_VALUE); 
					if (FlashStatus != HAL_OK) {
						return pFlash.ErrorCode;
					} else {
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					}		
				}				
			}
		}
	}
	
	// Save Command Snippets
	int currentAdd = RO_MID_ADDRESS;
	for(uint8_t s=0 ; s<numOfRecordedSnippets ; s++) 
	{
		if (snippets[s].cond.conditionType) 
		{
			snipBuffer[0] = 0xFE;		// A marker to separate Snippets
			memcpy( (uint8_t *)&snipBuffer[1], (uint8_t *)&snippets[s], sizeof(snippet_t));
			// Copy the snippet struct buffer (20 x numOfRecordedSnippets). Note this is assuming sizeof(snippet_t) is even.
			for(uint8_t j=0 ; j<(sizeof(snippet_t)/2) ; j++)
			{		
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, currentAdd, *(uint16_t *)&snipBuffer[j*2]);
				FlashStatus = FLASH_WaitForLastOperation((uint32_t)HAL_FLASH_TIMEOUT_VALUE); 
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 2;
				}				
			}			
			// Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped
			for(uint8_t j=0 ; j<((strlen(snippets[s].cmd)+1)/2) ; j++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, currentAdd, *(uint16_t *)(snippets[s].cmd+j*2));
				FlashStatus = FLASH_WaitForLastOperation((uint32_t)HAL_FLASH_TIMEOUT_VALUE); 
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 2;
				}				
			}				
		}	
	}
	
	HAL_FLASH_Lock();
	
	return result;
}

/* --- Clear array topology in SRAM and Flash RO --- 
*/
uint8_t ClearROtopology(void)
{
	// Clear the array 
	memset(array, 0, sizeof(array));
	N = 1; myID = 0;
	
	return SaveToRO();
}
/* --- clear "listBtcDevices" buffer
*/


/* --- H23R0 module initialization.
*/
void Module_Peripheral_Init(void)
{


	/* Array ports */
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  MX_USART6_UART_Init();

	/* BlueNRG-M2 UART */
  MX_USART3_UART_Init();
  UpdateBaudrate(6, 115200);

}

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

/* --- H23R0 message processing task
*/
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift)
{
	Module_Status result = H23R3_OK;

	switch (code)
	{

	case CODE_H23Rx_SEND_DATA:
		//The first parameter is data length ( cMessage[port-1][4] )
		result = BT_Send_Data(&cMessage[port-1][5], cMessage[port-1][4]);
		break;

	case CODE_H23Rx_CLEAR_USER_BUFFER:
		result = BT_Clear_User_Buffer();
		break;

	case CODE_H23Rx_DISCONNECT_INQUIRE:
		BT_Disconnect();
		break;

	case CODE_H23Rx_STREAM_TO_PORT:
		result = BT_Stream_To_Port(cMessage[port-1][4]);
		break;

	default:
		result = H23R3_ERR_UnknownMessage;
		break;
	}



	return result;
}


/* --- Register this module CLI Commands
*/
void RegisterModuleCLICommands(void)
{
	FreeRTOS_CLIRegisterCommand (&btClearUserBufferCommandDefinition);
	FreeRTOS_CLIRegisterCommand (&btSendDataCommandDefinition);
	FreeRTOS_CLIRegisterCommand (&btDisconnectCommandDefinition);
	FreeRTOS_CLIRegisterCommand (&btStreamToPortDefinition);
	FreeRTOS_CLIRegisterCommand (&btGetMacAddressDefinition);
	FreeRTOS_CLIRegisterCommand (&btConnectDefinition);

}



/* --- Get the port for a given UART.
*/
uint8_t GetPort(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART4)
			return P1;
	else if (huart->Instance == USART2)
			return P2;
	else if (huart->Instance == USART6)
			return P3;
	else if (huart->Instance == USART1)
			return P4;
	else if (huart->Instance == USART5)
			return P5;
	else if (huart->Instance == USART3)
			return P6;

	return 0;
}

/* -----------------------------------------------------------------------
	|																APIs	 																 	|
   -----------------------------------------------------------------------
*/
void BT_Receive_Data_To_BOS(void){

	BT_To_User_Buffer_flag = 0;

}
//-----------------------------------------------------------------------
Module_Status BT_Receive_Data(uint8_t* buffer,uint8_t size){
	Module_Status result = H23R3_OK;
	if(buffer!= NULL && size!=0)
	{
		BT_To_User_Buffer_flag = 1;
		BT_User_Buffer_beginning_ptr = buffer;
		BT_User_Buffer_ptr = buffer;
		BT_User_Buffer_Length = size;
		*BT_User_Buffer_Index_ptr = 0;

	}
	else
	{
		result = H23R3_ERROR;
	}
	return result;

}
//-----------------------------------------------------------------------
Module_Status BT_Send_Message(uint8_t dst,uint16_t code,uint16_t numberOfParams){

	Module_Status result = H23R3_OK;
	if(BT_Connection_flag == 1)
	{
		SendMessageToModule(dst,code,numberOfParams);
	}
	else
	{
	    result = H23R3_ERROR;
	}

	return result;



}
//-----------------------------------------------------------------------
Module_Status BT_Send_Data(uint8_t* BT_Data,uint8_t length){

	Module_Status result = H23R3_OK;
	if(BT_Connection_flag == 1 && length!=0 && BT_Data!=NULL)
	{
		for(int i=0;i<length;i++)
		{
			writePxMutex(PORT_BTC_CONN, (char *)&BT_Data[0+i], 1, cmd50ms, HAL_MAX_DELAY);
		}

	}
	else
	{
		if(BT_Connection_flag == 1) result = H23R3_ERR_WrongParams;
		else result = H23R3_ERROR;
	}

	return result;

}
//-----------------------------------------------------------------------
Module_Status BT_Clear_User_Buffer(void){

	 Module_Status result = H23R3_OK;


	 if(BT_To_User_Buffer_flag == 1)
	 {
		memset(BT_User_Buffer_beginning_ptr,0,BT_User_Buffer_Length);
		BT_User_Buffer_ptr = BT_User_Buffer_beginning_ptr;
		*BT_User_Buffer_Index_ptr = 0;
	 }
	 else
	 {
		 result = H23R3_ERROR;
	 }

	 return result;


}
//-----------------------------------------------------------------------
void BT_RESET_MODULE(void){

	writePxMutex(PORT_BTC_CONN,(char*)"\t\t\tr",4,cmd50ms, HAL_MAX_DELAY);
	BT_Connection_flag = 0;


}
//-----------------------------------------------------------------------
void BT_Disconnect(void){
	BT_Connection_flag = 0;
	BT_RESET_MODULE();
}
//-----------------------------------------------------------------------
Module_Status BT_Stream_To_Port(uint8_t port_number){
	Module_Status result = H23R3_OK;
	if(port_number >= 1 && port_number<6)
	{
		if(BT_Connection_flag == 1)
		{
			//UpdateBaudrate(port_number, 115200);
			StartScastDMAStream(PORT_BTC_CONN, myID, port_number, myID, BIDIRECTIONAL, 0xFFFFFFFF, 0xFFFFFFFF, false);
		}
		else
		{
			result = H23R3_ERROR;
		}
	}
	else
	{
		result = H23R3_ERR_WrongParams;
	}
	return result;
}


/* -----------------------------------------------------------------------
	|					Commands										  |
   -----------------------------------------------------------------------
*/
static portBASE_TYPE btClearUserBuffer( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	BT_Clear_User_Buffer();

	writePxMutex(PcPort, "Clearing Bluetooth User Data Buffer\r\n", 37, cmd50ms, HAL_MAX_DELAY);

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
//--------------------------------------------------------------------------------------------------------
static portBASE_TYPE btSendData( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

	Module_Status result = H23R3_OK;
	int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the 1st parameter string: Data to be sent*/
	pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);

	result = BT_Send_Data(pcParameterString1, xParameterStringLength1);

	if(result == H23R3_ERR_WrongParams)
	{
		writePxMutex(PcPort, "Wrong input parameter (No data to be sent)\r\n", 44, cmd50ms, HAL_MAX_DELAY);
	}
	else if(result == H23R3_ERROR)
	{
		writePxMutex(PcPort, "BlueNRG module is Not connected to any device\r\n", 47, cmd50ms, HAL_MAX_DELAY);
	}
	else
	{
		writePxMutex(PcPort,"Done! (Data has been sent)\r\n", 28, cmd50ms, HAL_MAX_DELAY);
	}
	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
//--------------------------------------------------------------------------------------------------------
static portBASE_TYPE btDisconnect( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if(BT_Connection_flag == 1)
	{
		BT_Disconnect();
		sprintf( ( char * ) pcWriteBuffer, "BlueNRG Disconnected\r\n");
	}
	else
	{
		sprintf( ( char * ) pcWriteBuffer, "BlueNRG is Not Connected\r\n");

	}


	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
//--------------------------------------------------------------------------------------------------------
static portBASE_TYPE btStreamToPort( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

	Module_Status result = H23R3_OK;
	int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;
	uint8_t lenPar = 0;
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the 1st parameter string: Data to be sent*/
	pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);

	lenPar = strlen((char *)pcParameterString1);
	if(lenPar < 2)
	{
		result = H23R3_ERR_WrongParams;
	}
	else if ( ('p' == pcParameterString1[0]) || ('P' == pcParameterString1[0]) )
	{
		result = BT_Stream_To_Port(pcParameterString1[1] - '0');
	}


	if(result == H23R3_ERR_WrongParams)
	{
		writePxMutex(PcPort, "Wrong input parameter (Wrong Port)\r\n", 36, cmd50ms, HAL_MAX_DELAY);
	}
	else if(result == H23R3_ERROR)
	{
		writePxMutex(PcPort, "Can't Start Stream, BlueNRG module is Not connected to any device\r\n", 67, cmd50ms, HAL_MAX_DELAY);
	}
	else
	{
		writePxMutex(PcPort,"Streaming BlueNRG Data to ", 26, cmd50ms, HAL_MAX_DELAY);
		writePxMutex(PcPort,pcParameterString1, lenPar, cmd50ms, HAL_MAX_DELAY);
		writePxMutex(PcPort,", baudrate: 115200\r\n", 20, cmd50ms, HAL_MAX_DELAY);


	}
	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
//--------------------------------------------------------------------------------------------------------
static portBASE_TYPE btGetMacAddress( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	BT_Get_Mac_Address();


	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
//--------------------------------------------------------------------------------------------------------
static portBASE_TYPE btConnect( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

	Module_Status result = H23R3_OK;
	int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the 1st parameter string: Data to be sent*/
	pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);

	result = BT_Connect(pcParameterString1, xParameterStringLength1);

	if(result == H23R3_ERR_WrongParams)
	{
		writePxMutex(PcPort, "Wrong input parameter (Wrong Port)\r\n", 36, cmd50ms, HAL_MAX_DELAY);
	}

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
