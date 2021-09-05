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





/* Private function prototypes -----------------------------------------------*/



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
	static const int8_t *pcMessageWrongParam = ( int8_t * ) "Wrong parameter!\r\n";
	static const int8_t *pcMessageMustScan = ( int8_t * ) "Must call \"scan\" command first\r\n";
	uint8_t lenPar = 0;



	return result;
}

/* --- Register this module CLI Commands
*/
void RegisterModuleCLICommands(void)
{

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

/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
	|															Commands																 	|
   -----------------------------------------------------------------------
*/


/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
