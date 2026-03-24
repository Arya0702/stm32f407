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
#include "app_freertos_logic.h"
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
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 3000U * 4U,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Smoke */
osThreadId_t SmokeHandle;
const osThreadAttr_t Smoke_attributes = {
  .name = "Smoke",
  .stack_size = 256U * 4U,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Slave */
osThreadId_t SlaveHandle;
const osThreadAttr_t Slave_attributes = {
  .name = "Slave",
  .stack_size = 256U * 4U,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Pwm */
osThreadId_t PwmHandle;
const osThreadAttr_t Pwm_attributes = {
  .name = "Pwm",
  .stack_size = 256U * 4U,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Water_Pump */
osThreadId_t Water_PumpHandle;
const osThreadAttr_t Water_Pump_attributes = {
  .name = "Water_Pump",
  .stack_size = 256U * 4U,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UART4_Protect */
osMutexId_t UART4_ProtectHandle;
const osMutexAttr_t UART4_Protect_attributes = {
  .name = "UART4_Protect"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Smoke_detect(void *argument);
void Slave_communicate(void *argument);
void pwm_task(void *argument);
void Pump_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
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
  /* USER CODE BEGIN StartDefaultTask */
  AppTasks_StartDefault(argument);
  /* USER CODE END StartDefaultTask */
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
  (void)argument;
  for(;;)
  {
    AppTasks_SmokeStep();
    osDelay(200U);
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
  (void)argument;
  for(;;)
  {
    osDelay(1U);
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
  (void)argument;
  AppTasks_PwmInit();
  for(;;)
  {
    AppTasks_PwmStep();
    osDelay(1U);
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
  (void)argument;
  for(;;)
  {
    AppTasks_PumpStep();
    osDelay(1U);
  }
  /* USER CODE END Pump_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  AppTasks_OnExti(GPIO_Pin);
}
/* USER CODE END Application */
