#include "microros_transport_uart4.h"

#include "main.h"
#include "usart.h"

#include <rmw_microxrcedds_c/config.h>

#include "cmsis_os.h"

#include <string.h>

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM

#define UART_IT_BUFFER_SIZE 2048U

static uint8_t s_it_buffer[UART_IT_BUFFER_SIZE];
static uint8_t s_it_data;
static size_t s_it_head = 0;
static size_t s_it_tail = 0;

bool cubemx_transport_open(struct uxrCustomTransport *transport)
{
  UART_HandleTypeDef *uart = (UART_HandleTypeDef *)transport->args;
  (void)HAL_UART_Receive_IT(uart, &s_it_data, 1U);
  return true;
}

bool cubemx_transport_close(struct uxrCustomTransport *transport)
{
  UART_HandleTypeDef *uart = (UART_HandleTypeDef *)transport->args;
  (void)HAL_UART_Abort_IT(uart);
  return true;
}

size_t cubemx_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len,
                                uint8_t *err)
{
  (void)err;
  UART_HandleTypeDef *uart = (UART_HandleTypeDef *)transport->args;

  HAL_StatusTypeDef ret;
  if (uart->gState == HAL_UART_STATE_READY) {
    ret = HAL_UART_Transmit_IT(uart, (uint8_t *)(uintptr_t)buf, (uint16_t)len);
    while (ret == HAL_OK && uart->gState != HAL_UART_STATE_READY) {
      osDelay(1);
    }
    return (ret == HAL_OK) ? len : 0U;
  }
  return 0U;
}

size_t cubemx_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len,
                               int timeout, uint8_t *err)
{
  (void)transport;
  (void)timeout;
  (void)err;

  size_t wrote = 0;
  while ((s_it_head != s_it_tail) && (wrote < len)) {
    buf[wrote] = s_it_buffer[s_it_head];
    s_it_head = (s_it_head + 1U) % UART_IT_BUFFER_SIZE;
    wrote++;
  }
  return wrote;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance != UART4) {
    return;
  }

  s_it_buffer[s_it_tail] = s_it_data;
  s_it_tail = (s_it_tail + 1U) % UART_IT_BUFFER_SIZE;
  if (s_it_tail == s_it_head) {
    s_it_head = (s_it_head + 1U) % UART_IT_BUFFER_SIZE;
  }

  (void)HAL_UART_Receive_IT(huart, &s_it_data, 1U);
}

#endif /* RMW_UXRCE_TRANSPORT_CUSTOM */
