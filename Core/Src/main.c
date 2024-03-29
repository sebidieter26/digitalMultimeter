/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "liquidcrystal_i2c.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define modprint 0
#define modvolt 1
#define modamper 2
#define modohm 3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* Definitions for voltMeter */
osThreadId_t voltMeterHandle;
const osThreadAttr_t voltMeter_attributes = {
  .name = "voltMeter",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for amperMeter */
osThreadId_t amperMeterHandle;
const osThreadAttr_t amperMeter_attributes = {
  .name = "amperMeter",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ohmmeter */
osThreadId_t ohmmeterHandle;
const osThreadAttr_t ohmmeter_attributes = {
  .name = "ohmmeter",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for defaultPrint */
osThreadId_t defaultPrintHandle;
const osThreadAttr_t defaultPrint_attributes = {
  .name = "defaultPrint",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for adcAcquire */
osThreadId_t adcAcquireHandle;
const osThreadAttr_t adcAcquire_attributes = {
  .name = "adcAcquire",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for voltQueue */
osMessageQueueId_t voltQueueHandle;
const osMessageQueueAttr_t voltQueue_attributes = {
  .name = "voltQueue"
};
/* Definitions for queueAmper */
osMessageQueueId_t queueAmperHandle;
const osMessageQueueAttr_t queueAmper_attributes = {
  .name = "queueAmper"
};
/* Definitions for queueOhm */
osMessageQueueId_t queueOhmHandle;
const osMessageQueueAttr_t queueOhm_attributes = {
  .name = "queueOhm"
};
/* USER CODE BEGIN PV */
uint16_t raw = 0;
char voltageStr[20];
char amperStr[20];
char ohmStr[20];
uint8_t mod = modprint;
uint16_t Button_Pressed = RESET;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
void StartVoltMeter(void *argument);
void StartAmperMeter(void *argument);
void StartOhmMeter(void *argument);
void StartPrint(void *argument);
void StartADC(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of voltQueue */
  voltQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &voltQueue_attributes);

  /* creation of queueAmper */
  queueAmperHandle = osMessageQueueNew (16, sizeof(uint16_t), &queueAmper_attributes);

  /* creation of queueOhm */
  queueOhmHandle = osMessageQueueNew (16, sizeof(uint16_t), &queueOhm_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of voltMeter */
  voltMeterHandle = osThreadNew(StartVoltMeter, NULL, &voltMeter_attributes);

  /* creation of amperMeter */
  amperMeterHandle = osThreadNew(StartAmperMeter, NULL, &amperMeter_attributes);

  /* creation of ohmmeter */
  ohmmeterHandle = osThreadNew(StartOhmMeter, NULL, &ohmmeter_attributes);

  /* creation of defaultPrint */
  defaultPrintHandle = osThreadNew(StartPrint, NULL, &defaultPrint_attributes);

  /* creation of adcAcquire */
  adcAcquireHandle = osThreadNew(StartADC, NULL, &adcAcquire_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Amper_Button_Pin */
  GPIO_InitStruct.Pin = Amper_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Amper_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Volt_Button_Pin */
  GPIO_InitStruct.Pin = Volt_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Volt_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Ohm_Button_Pin */
  GPIO_InitStruct.Pin = Ohm_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Ohm_Button_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	enum Mode {SELECT_MODE, VOLTMETER_MODE, AMPERMETER_MODE, OHMMETER_MODE };
		enum Mode currentState = SELECT_MODE;
		bool checkButton(uint16_t GPIO_Pin) {
		  if (currentState == SELECT_MODE && GPIO_Pin == B1_Pin) {
		    currentState = SELECT_MODE;
		    return true;
		  } else if (currentState == SELECT_MODE) {
		    if (GPIO_Pin == Volt_Button_Pin) {
		      currentState = VOLTMETER_MODE;
		      return true;
		    } else if (GPIO_Pin == Amper_Button_Pin) {
		      currentState = AMPERMETER_MODE;
		      return true;
		    } else if (GPIO_Pin == Ohm_Button_Pin) {
		      currentState = OHMMETER_MODE;
		      return true;
		    } else {
		      // Handle unexpected button press in SELECT_MODE (optional)
		      return false;
		    }
		  } else if (currentState == VOLTMETER_MODE && GPIO_Pin == Volt_Button_Pin) {
		    // Perform voltmeter operations here
		    return true;
		  } else if (currentState == AMPERMETER_MODE && GPIO_Pin == Amper_Button_Pin) {
		    // Perform ammeter operations here
		    return true;
		  } else if (currentState == OHMMETER_MODE && GPIO_Pin == Ohm_Button_Pin) {
		    // Perform ohmmeter operations here
		    return true;
		  }
		  return false;
		}

		void selectNextMode() {
		  switch (currentState) {
		    case SELECT_MODE:
		      currentState = VOLTMETER_MODE;
		      break;
		    case VOLTMETER_MODE:
		      currentState = AMPERMETER_MODE;
		      break;
		    case AMPERMETER_MODE:
		      currentState = OHMMETER_MODE;
		      break;
		    case OHMMETER_MODE:
		      // Optional: Can reset to DEFAULT_MODE or cycle back to VOLTMETER_MODE
		      break;
		  }
		}


}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartVoltMeter */
/**
  * @brief  Function implementing the voltMeter thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartVoltMeter */
void StartVoltMeter(void *argument)
{
  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  for(;;)
  {
		osThreadSuspend(defaultPrintHandle);
		osThreadSuspend(amperMeterHandle);
		osThreadSuspend(ohmmeterHandle);

	switch(mod){
	case modprint:
		osThreadResume(defaultPrintHandle);
		break;
	case modamper:
		osThreadResume(amperMeterHandle);
		break;
	case modohm:
		osThreadResume(ohmmeterHandle);
		break;
	case modvolt:
	HD44780_Init(2);
	HD44780_Clear();
	HD44780_SetCursor(0,0);
	HD44780_PrintStr("VOLTMETER");
    float voltage = 0;
    snprintf(voltageStr, sizeof(voltageStr), "%.2f V", voltage);
    HD44780_SetCursor(0,1);
    HD44780_PrintStr(voltageStr);
    osDelay(100);
    break;
	}

  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartAmperMeter */
/**
* @brief Function implementing the amperMeter thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAmperMeter */
void StartAmperMeter(void *argument)
{
  /* USER CODE BEGIN StartAmperMeter */

  /* Infinite loop */
  for(;;)
  {
		osThreadSuspend(defaultPrintHandle);
		osThreadSuspend(voltMeterHandle);
		osThreadSuspend(ohmmeterHandle);

	  switch(mod){
	case modprint:
		osThreadResume(defaultPrintHandle);
		break;
	case modvolt:
		osThreadResume(voltMeterHandle);
		break;
	case modohm:
		osThreadResume(ohmmeterHandle);
		break;
	case modamper:
	HD44780_Init(2);
	HD44780_Clear();
	HD44780_SetCursor(0,0);
	HD44780_PrintStr("AMPERMETER");

	float ampers = 0;
	snprintf(amperStr, sizeof(amperStr), "%.2f A", ampers);
	HD44780_SetCursor(0,1);
	HD44780_PrintStr(amperStr);
    osDelay(100);
    break;
  }
  }
  /* USER CODE END StartAmperMeter */
}

/* USER CODE BEGIN Header_StartOhmMeter */
/**
* @brief Function implementing the ohmmeter thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOhmMeter */
void StartOhmMeter(void *argument)
{
  /* USER CODE BEGIN StartOhmMeter */

  /* Infinite loop */
  for(;;)
  {
	  osThreadSuspend(defaultPrintHandle);
	  osThreadSuspend(voltMeterHandle);
	  osThreadSuspend(amperMeterHandle);

	  switch(mod){
	case modprint:
		osThreadResume(defaultPrintHandle);
		break;
	case modvolt:
		osThreadResume(voltMeterHandle);
		break;
	case modamper:
		osThreadResume(amperMeterHandle);
		break;
	case modohm:
	  	HD44780_Init(2);
	  	HD44780_Clear();
	  	HD44780_SetCursor(0,0);
	  	HD44780_PrintStr("OHMMETER");
	  	float ohms = 0;
	  	snprintf(ohmStr, sizeof(ohmStr), "%.2f ohm", ohms);
	  	HD44780_SetCursor(0,1);
	  	HD44780_PrintStr(ohmStr);
	    osDelay(100);
	    break;
	  }

  }
  /* USER CODE END StartOhmMeter */
}

/* USER CODE BEGIN Header_StartPrint */
/**
* @brief Function implementing the defaultPrint thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPrint */
void StartPrint(void *argument)
{
  /* USER CODE BEGIN StartPrint */

  /* Infinite loop */
  for(;;)
  {
	  osThreadSuspend(voltMeterHandle);
	  osThreadSuspend(amperMeterHandle);
	  osThreadSuspend(ohmmeterHandle);

	  switch(mod){
	case modohm:
		osThreadResume(ohmmeterHandle);
		break;
	case modvolt:
		osThreadResume(voltMeterHandle);
		break;
	case modamper:
		osThreadResume(amperMeterHandle);
		break;
	case modprint:
	  HD44780_Init(2);
	  HD44780_Clear();
	  HD44780_SetCursor(0,0);
	  HD44780_PrintStr("/////DIGITAL////");
	  HD44780_SetCursor(0,1);
	  HD44780_PrintStr("///MULTIMETER///");
	  osDelay(100);
	  break;
	  }

  }
  /* USER CODE END StartPrint */
}

/* USER CODE BEGIN Header_StartADC */
/**
* @brief Function implementing the adcAcquire thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartADC */
void StartADC(void *argument)
{
  /* USER CODE BEGIN StartADC */
  /* Infinite loop */
  for(;;)
  {
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,100);
	raw = HAL_ADC_GetValue(&hadc1);
	osMessageQueuePut(voltQueueHandle, &raw, 1, 10);
	osMessageQueuePut(queueAmperHandle, &raw, 1, 10);
	osMessageQueuePut(queueOhmHandle, &raw, 1, 10);
    osDelay(100);
  }
  /* USER CODE END StartADC */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
