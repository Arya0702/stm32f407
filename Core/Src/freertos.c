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
#include "usart.h"
#include "adc.h"
#include "usbh_def.h"
#include "usb_host.h"
#include "usbh_cdc.h"
#include "tim.h"
#include "MI1640.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define buf_size 19360 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint16_t rx_Buf[buf_size];

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
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
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
/* Definitions for Pwm */
osThreadId_t PwmHandle;
const osThreadAttr_t Pwm_attributes = {
  .name = "Pwm",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Water_Pump */
osThreadId_t Water_PumpHandle;
const osThreadAttr_t Water_Pump_attributes = {
  .name = "Water_Pump",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UART4_Protect */
osMutexId_t UART4_ProtectHandle;
const osMutexAttr_t UART4_Protect_attributes = {
  .name = "UART4_Protect"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern USBH_HandleTypeDef hUsbHostFS;
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void MI1602(void *argument);
void Smoke_detect(void *argument);
void Slave_communicate(void *argument);
void pwm_task(void *argument);
void Pump_task(void *argument);

extern void MX_USB_HOST_Init(void);
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

  /* creation of Smoke */
  SmokeHandle = osThreadNew(Smoke_detect, NULL, &Smoke_attributes);

  /* creation of Slave */
  SlaveHandle = osThreadNew(Slave_communicate, NULL, &Slave_attributes);

  /* creation of Pwm */
  PwmHandle = osThreadNew(pwm_task, NULL, &Pwm_attributes);

  /* creation of Water_Pump */
  Water_PumpHandle = osThreadNew(Pump_task, NULL, &Water_Pump_attributes);

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
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
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
		USBH_Process(&hUsbHostFS);
    USBH_StatusTypeDef status = USBH_CDC_Receive(&hUsbHostFS, (uint8_t *)rx_Buf, buf_size);
    
    if (status == USBH_OK) {
        //---------------------------------------transmit
        HAL_UART_Transmit(&huart6, (uint8_t *)"receive MI160\r\n", strlen("receive MI1602\r\n"), HAL_MAX_DELAY);//--------------------for debug
    }
    osDelay(1);
  }
  /* USER CODE END MI1602 */
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
    HAL_UART_Transmit(&huart6, (uint8_t *)&adc_val, 13, HAL_MAX_DELAY);//--------------------for debug
    osMutexAcquire(UART4_ProtectHandle, osWaitForever);
    //HAL_UART_Transmit(&huart4, (uint8_t *)&adc_val, 13, HAL_MAX_DELAY);--------------------transmit to orange pi
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
    //send x,y,z to slave stm32f407 via UART1-------------------transmit to stm32f407
    osDelay(1);
  }
  /* USER CODE END Slave_communicate */
}

/* USER CODE BEGIN Header_pwm_task */
/**
* @brief Function implementing the Pwm thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm_task */
void pwm_task(void *argument)
{
  /* USER CODE BEGIN pwm_task */
  /* Infinite loop */
  for(;;)
  {
    if(0)//-------------------------------------------------------get angle
    {
      int angle = 90;
      int pwm = 250+(angle*1000)/270;
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);
    }
    osDelay(1);
  }
  /* USER CODE END pwm_task */
}

/* USER CODE BEGIN Header_Pump_task */
/**
* @brief Function implementing the Water_Pump thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Pump_task */
void Pump_task(void *argument)
{
  /* USER CODE BEGIN Pump_task */
  /* Infinite loop */
  for(;;)
  {
    int pump_state = 0;
    if(pump_state)//------------------------------------------get imformation
    {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
    }
    else
    {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    osDelay(1);
  }
  /* USER CODE END Pump_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == alert_smoke_Pin) 
  {
    //send smoke alert message via UART4
    osMutexAcquire(UART4_ProtectHandle, osWaitForever);
    char alert_msg[] = "Smoke detected!\r\n";
    HAL_UART_Transmit(&huart6, (uint8_t *)alert_msg, sizeof(alert_msg) - 1, HAL_MAX_DELAY);//---------------------for debug
    //HAL_UART_Transmit_DMA(&huart4, (uint8_t *)alert_msg, sizeof(alert_msg) - 1);-----------------------transmit to orange pi
    osMutexRelease(UART4_ProtectHandle);
  }
}

void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost)
{
    if (phost == &hUsbHostFS)
    {
      
        // 1. 获取实际收到的字节数
        //int len = USBH_CDC_GetLastRxSize(phost);

        
        // 2. 在这里处理你的数据 (比如存入环形缓冲区或打印)
        // Process_My_Data(rx_buffer, len);
        
        // 3. 【最重要】重新开启下一次接收
        // 如果不调这一句，STM32 就只会接收一次，然后永远沉默
        USBH_CDC_Receive(phost, (uint8_t *)rx_Buf, 64);
    }
}
/* USER CODE END Application */

