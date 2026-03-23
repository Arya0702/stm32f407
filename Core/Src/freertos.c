#include "FreeRTOS.h"
#include "cmsis_os.h"

#include "microros_app.h"
#include "mq2_serial_test.h"

/* USER CODE BEGIN Variables */
osMutexId_t UART4_ProtectHandle;
const osMutexAttr_t UART4_Protect_attributes = {
    .name = "UART4_Protect",
};

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 3000U * 4U,
    .priority = (osPriority_t)osPriorityNormal,
};

osThreadId_t smokeTaskHandle;
const osThreadAttr_t smokeTask_attributes = {
    .name = "Smoke",
    .stack_size = 256U * 4U,
    .priority = (osPriority_t)osPriorityBelowNormal,
};

osThreadId_t slaveTaskHandle;
const osThreadAttr_t slaveTask_attributes = {
    .name = "Slave",
    .stack_size = 256U * 4U,
    .priority = (osPriority_t)osPriorityBelowNormal,
};

osThreadId_t pwmTaskHandle;
const osThreadAttr_t pwmTask_attributes = {
    .name = "Pwm",
    .stack_size = 256U * 4U,
    .priority = (osPriority_t)osPriorityLow,
};

osThreadId_t pumpTaskHandle;
const osThreadAttr_t pumpTask_attributes = {
    .name = "Water_Pump",
    .stack_size = 256U * 4U,
    .priority = (osPriority_t)osPriorityLow,
};
/* USER CODE END Variables */

void StartDefaultTask(void *argument);
void Smoke_detect(void *argument);
void Slave_communicate(void *argument);
void pwm_task(void *argument);
void Pump_task(void *argument);

void MX_FREERTOS_Init(void)
{
  UART4_ProtectHandle = osMutexNew(&UART4_Protect_attributes);

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  smokeTaskHandle = osThreadNew(Smoke_detect, NULL, &smokeTask_attributes);
  slaveTaskHandle = osThreadNew(Slave_communicate, NULL, &slaveTask_attributes);
  pwmTaskHandle = osThreadNew(pwm_task, NULL, &pwmTask_attributes);
  pumpTaskHandle = osThreadNew(Pump_task, NULL, &pumpTask_attributes);
}

/* USER CODE BEGIN Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
#if USE_MQ2_SERIAL_TEST_ONLY
  mq2_serial_test_run(argument);
#else
  microros_app_run(argument);
#endif
}
/* USER CODE END Header_StartDefaultTask */

void Smoke_detect(void *argument)
{
  (void)argument;
  for (;;) {
    osDelay(1000U);
  }
}

void Slave_communicate(void *argument)
{
  (void)argument;
  for (;;) {
    osDelay(1000U);
  }
}

void pwm_task(void *argument)
{
  (void)argument;
  for (;;) {
    osDelay(1000U);
  }
}

void Pump_task(void *argument)
{
  (void)argument;
  for (;;) {
    osDelay(1000U);
  }
}
