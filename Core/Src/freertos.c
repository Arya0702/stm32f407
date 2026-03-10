/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "spi.h"
#include "usart.h"
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t spiBuf[19360];

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MI1602_task */
osThreadId_t MI1602_taskHandle;
const osThreadAttr_t MI1602_task_attributes = {
  .name = "MI1602_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for MI1602_Send */
osThreadId_t MI1602_SendHandle;
const osThreadAttr_t MI1602_Send_attributes = {
  .name = "MI1602_Send",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Smoke */
osThreadId_t SmokeHandle;
const osThreadAttr_t Smoke_attributes = {
  .name = "Smoke",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Slave */
osThreadId_t SlaveHandle;
const osThreadAttr_t Slave_attributes = {
  .name = "Slave",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for UART4_Protect */
osMutexId_t UART4_ProtectHandle;
const osMutexAttr_t UART4_Protect_attributes = {
  .name = "UART4_Protect"
};
/* Definitions for MI1602Data_readytosend */
osSemaphoreId_t MI1602Data_readytosendHandle;
const osSemaphoreAttr_t MI1602Data_readytosend_attributes = {
  .name = "MI1602Data_readytosend"
};
/* Definitions for MI1602Data_readytoreceive */
osSemaphoreId_t MI1602Data_readytoreceiveHandle;
const osSemaphoreAttr_t MI1602Data_readytoreceive_attributes = {
  .name = "MI1602Data_readytoreceive"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void MI1602(void *argument);
void MI1602_SendTask(void *argument);
void Smoke_detect(void *argument);
void Slave_communicate(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of UART4_Protect */
  UART4_ProtectHandle = osMutexNew(&UART4_Protect_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of MI1602Data_readytosend */
  MI1602Data_readytosendHandle = osSemaphoreNew(1, 0, &MI1602Data_readytosend_attributes);

  /* creation of MI1602Data_readytoreceive */
  MI1602Data_readytoreceiveHandle = osSemaphoreNew(1, 1, &MI1602Data_readytoreceive_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of MI1602_task */
  MI1602_taskHandle = osThreadNew(MI1602, NULL, &MI1602_task_attributes);

  /* creation of MI1602_Send */
  MI1602_SendHandle = osThreadNew(MI1602_SendTask, NULL, &MI1602_Send_attributes);

  /* creation of Smoke */
  SmokeHandle = osThreadNew(Smoke_detect, NULL, &Smoke_attributes);

  /* creation of Slave */
  SlaveHandle = osThreadNew(Slave_communicate, NULL, &Slave_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_MI1602 */
/**
* @brief Function implementing the MI1602_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MI1602 */
void MI1602(void *argument)
{
  /* USER CODE BEGIN MI1602 */
  /* Infinite loop */
  for(;;)
  {
		if(osSemaphoreAcquire(MI1602Data_readytoreceiveHandle,osWaitForever))
	  {
		  HAL_GPIO_WritePin(GPIOA, MI48_SS_Pin, GPIO_PIN_RESET);

		  if(HAL_SPI_Receive(&hspi3, (uint8_t *)spiBuf, 19360, HAL_MAX_DELAY) != HAL_OK)
		  {
			  HAL_GPIO_WritePin(GPIOA, MI48_SS_Pin, GPIO_PIN_SET);

		    Error_Handler();
		  }
		  else
		  {
			  HAL_GPIO_WritePin(GPIOA, MI48_SS_Pin, GPIO_PIN_SET);
				osSemaphoreRelease(MI1602Data_readytosendHandle);
		  }
	  }
    osDelay(1);
  }
  /* USER CODE END MI1602 */
}

/* USER CODE BEGIN Header_MI1602_SendTask */
/**
* @brief Function implementing the MI1602_Send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MI1602_SendTask */
void MI1602_SendTask(void *argument)
{
  /* USER CODE BEGIN MI1602_SendTask */
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreAcquire(MI1602Data_readytosendHandle,osWaitForever);
    osMutexAcquire(UART4_ProtectHandle, osWaitForever);
		HAL_UART_Transmit_DMA(&huart4,(uint8_t *)spiBuf,19360*2);
    osMutexRelease(UART4_ProtectHandle);
    osDelay(1);
  }
  /* USER CODE END MI1602_SendTask */
}

/* USER CODE BEGIN Header_Smoke_detect */
/**
* @brief Function implementing the Smoke thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Smoke_detect */
void Smoke_detect(void *argument)
{
  /* USER CODE BEGIN Smoke_detect */
  /* Infinite loop */
  for(;;)
  {
    HAL_ADC_Start(&hadc1);
    int adc_val = -1;
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        adc_val = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    osMutexAcquire(UART4_ProtectHandle, osWaitForever);
    //HAL_UART_Transmit(&huart4, (uint8_t *)&adc_val, 13, HAL_MAX_DELAY);
    osMutexRelease(UART4_ProtectHandle);
    osDelay(200);
  }
  /* USER CODE END Smoke_detect */
}

/* USER CODE BEGIN Header_Slave_communicate */
/**
* @brief Function implementing the Slave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Slave_communicate */
void Slave_communicate(void *argument)
{
  /* USER CODE BEGIN Slave_communicate */
  /* Infinite loop */
  for(;;)
  {
    //send x,y,z to slave stm32f407 via UART1
    osDelay(1);
  }
  /* USER CODE END Slave_communicate */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == MI48_DATA_READY_Pin) 
  {
     osSemaphoreRelease(MI1602Data_readytoreceiveHandle);
  }
  if(GPIO_Pin == alert_smoke_Pin) 
  {
    //send smoke alert message via UART4
    osMutexAcquire(UART4_ProtectHandle, osWaitForever);
    char alert_msg[] = "Smoke detected!\r\n";
    //HAL_UART_Transmit_DMA(&huart4, (uint8_t *)alert_msg, sizeof(alert_msg) - 1);
    osMutexRelease(UART4_ProtectHandle);
  }
}
/* USER CODE END Application */

