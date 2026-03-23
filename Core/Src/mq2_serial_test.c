#include "mq2_serial_test.h"

#include "adc.h"
#include "mq2_smoke.h"
#include "usart.h"

#include "cmsis_os.h"

#include <stdio.h>
#include <string.h>

#ifndef MQ2_SERIAL_TEST_PERIOD_MS
#define MQ2_SERIAL_TEST_PERIOD_MS 200U
#endif

#ifndef MQ2_STARTUP_CALIB_SAMPLES
#define MQ2_STARTUP_CALIB_SAMPLES 20U
#endif

extern UART_HandleTypeDef huart6;
extern ADC_HandleTypeDef hadc1;

static uint32_t mq2_read_adc_raw(void)
{
  (void)HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 50U) != HAL_OK) {
    (void)HAL_ADC_Stop(&hadc1);
    return 0U;
  }
  const uint32_t v = HAL_ADC_GetValue(&hadc1);
  (void)HAL_ADC_Stop(&hadc1);
  return v;
}

static void mq2_calibrate_on_boot(void)
{
  uint32_t sum = 0U;
  for (uint32_t i = 0U; i < MQ2_STARTUP_CALIB_SAMPLES; i++) {
    sum += mq2_read_adc_raw();
    osDelay(10U);
  }
  const uint32_t avg = sum / MQ2_STARTUP_CALIB_SAMPLES;
  if (avg > 0U) {
    MQ2_CalibrateR0_FromCleanAirAdc(avg);
  }
}

static void uart6_send_line(const char *s)
{
  const size_t n = strlen(s);
  if (n == 0U || n > 512U) {
    return;
  }
  (void)HAL_UART_Transmit(&huart6, (uint8_t *)s, (uint16_t)n, 200U);
}

void mq2_serial_test_run(void *argument)
{
  (void)argument;

  uart6_send_line("MQ2_SERIAL_TEST v1 PA6=ADC1_IN6 USART6=115200 (no micro-ROS)\r\n");

  mq2_calibrate_on_boot();
  uart6_send_line("calib_done\r\n");

  for (;;) {
    const uint32_t raw = mq2_read_adc_raw();
    const float ppm = MQ2_AdcToSmokePpm(raw);
    /* 避免链接 printf 浮点：ppm 用毫 PPM 整数输出 */
    const int32_t ppm_milli = (int32_t)(ppm * 1000.0f);
    const uint32_t v_adc_mv = (raw * 3300U) / 4095U;

    char buf[128];
    const int n = snprintf(
        buf, sizeof(buf), "raw=%lu adc_mV=%lu ppm_milli=%ld\r\n",
        (unsigned long)raw, (unsigned long)v_adc_mv, (long)ppm_milli);
    if (n > 0 && n < (int)sizeof(buf)) {
      (void)HAL_UART_Transmit(&huart6, (uint8_t *)buf, (uint16_t)n, 200U);
    }

    osDelay(MQ2_SERIAL_TEST_PERIOD_MS);
  }
}
