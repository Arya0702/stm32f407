#include "app_freertos_logic.h"

#include "adc.h"
#include "main.h"
#include "mq2_serial_test.h"
#include "microros_app.h"
#include "tim.h"

volatile uint8_t pump_state = 0U;
volatile int alert = 0;

void AppTasks_StartDefault(void *argument)
{
#if USE_MQ2_SERIAL_TEST_ONLY
  mq2_serial_test_run(argument);
#else
  microros_app_run(argument);
#endif
}

void AppTasks_SmokeStep(void)
{
  volatile uint32_t adc_val = 0U;

  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
    adc_val = HAL_ADC_GetValue(&hadc1);
  }
  HAL_ADC_Stop(&hadc1);

  if (alert == 1) {
    alert = 0;
  }

  (void)adc_val;
}

void AppTasks_PwmInit(void)
{
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void AppTasks_PwmStep(void)
{
  int angle_x = 90;
  int angle_z = 90;
  int pwm_x = 50 + (angle_x * 200) / 270;
  int pwm_z = 50 + (angle_z * 200) / 180;

  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_x);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm_z);
}

void AppTasks_PumpStep(void)
{
  if (pump_state) {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
  }
}

void AppTasks_OnExti(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == alert_smoke_Pin) {
    if (HAL_GPIO_ReadPin(GPIOA, alert_smoke_Pin) == GPIO_PIN_SET) {
      alert = 0;
    } else {
      alert = 1;
    }
  }
}
