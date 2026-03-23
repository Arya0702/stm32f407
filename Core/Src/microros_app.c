#include "microros_app.h"

#include "adc.h"
#include "mq2_smoke.h"
#include "usart.h"

#include <rcl/init_options.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int32.h>

#include "cmsis_os.h"
#include "microros_transport_uart4.h"

#ifndef MICROROS_PUBLISH_PERIOD_MS
#define MICROROS_PUBLISH_PERIOD_MS 100U
#endif

#ifndef MQ2_STARTUP_CALIB_SAMPLES
#define MQ2_STARTUP_CALIB_SAMPLES 20U
#endif

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

void microros_app_run(void *argument)
{
  (void)argument;

  rmw_ret_t rmw_rc = rmw_uros_set_custom_transport(
      true, (void *)&huart4, cubemx_transport_open, cubemx_transport_close, cubemx_transport_write,
      cubemx_transport_read);
  if (rmw_rc != RMW_RET_OK) {
    for (;;) {
      osDelay(1000U);
    }
  }

  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t rc = rcl_init_options_init(&init_options, allocator);
  if (rc != RCL_RET_OK) {
    for (;;) {
      osDelay(1000U);
    }
  }
  (void)rcl_init_options_set_domain_id(&init_options, 20U);

  rclc_support_t support;
  rc = rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
  if (rc != RCL_RET_OK) {
    (void)rcl_init_options_fini(&init_options);
    for (;;) {
      osDelay(1000U);
    }
  }
  (void)rcl_init_options_fini(&init_options);

  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, "stm32f407_node", "", &support);
  if (rc != RCL_RET_OK) {
    (void)rclc_support_fini(&support);
    for (;;) {
      osDelay(1000U);
    }
  }

  rcl_publisher_t pub_sensor = rcl_get_zero_initialized_publisher();
  rc = rclc_publisher_init_default(&pub_sensor, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
                                   "/stm32/sensor");
  if (rc != RCL_RET_OK) {
    (void)rcl_node_fini(&node);
    (void)rclc_support_fini(&support);
    for (;;) {
      osDelay(1000U);
    }
  }

  rcl_publisher_t pub_smoke = rcl_get_zero_initialized_publisher();
  rc = rclc_publisher_init_default(&pub_smoke, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
                                   "/stm32/mq2/smoke_ppm");
  if (rc != RCL_RET_OK) {
    (void)rcl_publisher_fini(&pub_sensor, &node);
    (void)rcl_node_fini(&node);
    (void)rclc_support_fini(&support);
    for (;;) {
      osDelay(1000U);
    }
  }

  std_msgs__msg__Int32 msg_adc = {0};
  std_msgs__msg__Float32 msg_ppm = {0};

  mq2_calibrate_on_boot();

  for (;;) {
    const uint32_t raw = mq2_read_adc_raw();
    msg_adc.data = (int32_t)raw;
    msg_ppm.data = MQ2_AdcToSmokePpm(raw);

    (void)rcl_publish(&pub_sensor, &msg_adc, NULL);
    (void)rcl_publish(&pub_smoke, &msg_ppm, NULL);

    osDelay(MICROROS_PUBLISH_PERIOD_MS);
  }
}
