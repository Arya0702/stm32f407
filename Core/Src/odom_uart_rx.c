/**
 * @brief USART2 DMA + IDLE 接收 RosRobot 里程计二进制帧（与 odom_uart_bridge.c 一致）
 */
#include "odom_uart_rx.h"

#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

#include <string.h>

#define ODOM_FRAME_BYTES 29U
#define ODOM_SYNC0       0xA5U
#define ODOM_SYNC1       0x5AU
#define RX_CHUNK         64U

static uint8_t s_dma_rx[RX_CHUNK];

static uint8_t s_sliding[48];
static uint8_t s_sliding_len;

static volatile OdomUartRxSnapshot s_snap;
static volatile uint8_t s_have_odom;

static uint8_t frame_checksum(const uint8_t *f)
{
  uint8_t s = 0;
  for (int i = 2; i < 28; ++i) {
    s = (uint8_t)(s + f[i]);
  }
  return s;
}

static void try_extract_frames(void)
{
  while (s_sliding_len >= ODOM_FRAME_BYTES) {
    int idx = -1;
    for (uint8_t i = 0; i + 1 < s_sliding_len; ++i) {
      if (s_sliding[i] == ODOM_SYNC0 && s_sliding[i + 1] == ODOM_SYNC1) {
        idx = (int)i;
        break;
      }
    }
    if (idx < 0) {
      if (s_sliding_len > 0) {
        s_sliding[0] = s_sliding[s_sliding_len - 1];
        s_sliding_len = 1;
      }
      return;
    }
    if (idx > 0) {
      memmove(s_sliding, s_sliding + (uint8_t)idx, s_sliding_len - (uint8_t)idx);
      s_sliding_len = (uint8_t)(s_sliding_len - (uint8_t)idx);
    }
    if (s_sliding_len < ODOM_FRAME_BYTES) {
      return;
    }
    uint8_t *f = s_sliding;
    if (f[2] != 0x01U || f[28] != frame_checksum(f)) {
      memmove(s_sliding, s_sliding + 1, s_sliding_len - 1);
      s_sliding_len--;
      continue;
    }

    OdomUartRxSnapshot snap;
    memcpy(&snap.x_m, &f[4], sizeof(float));
    memcpy(&snap.y_m, &f[8], sizeof(float));
    memcpy(&snap.theta_rad, &f[12], sizeof(float));
    memcpy(&snap.vx_m_s, &f[16], sizeof(float));
    memcpy(&snap.vy_m_s, &f[20], sizeof(float));
    memcpy(&snap.vtheta_rad_s, &f[24], sizeof(float));
    snap.host_tick_ms = HAL_GetTick();

    __disable_irq();
    s_snap = snap;
    s_have_odom = 1U;
    __enable_irq();

    memmove(s_sliding, s_sliding + ODOM_FRAME_BYTES, s_sliding_len - ODOM_FRAME_BYTES);
    s_sliding_len = (uint8_t)(s_sliding_len - ODOM_FRAME_BYTES);
  }
}

static void feed_bytes(const uint8_t *data, uint16_t len)
{
  for (uint16_t i = 0; i < len; ++i) {
    if (s_sliding_len >= sizeof(s_sliding)) {
      memmove(s_sliding, s_sliding + 1, s_sliding_len - 1);
      s_sliding_len--;
    }
    s_sliding[s_sliding_len++] = data[i];
    try_extract_frames();
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance != USART2) {
    return;
  }
  if (Size > 0U && Size <= RX_CHUNK) {
    feed_bytes(s_dma_rx, Size);
  }
  (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_dma_rx, RX_CHUNK);
}

void OdomUartRx_Init(void)
{
  s_sliding_len = 0;
  memset((void *)&s_snap, 0, sizeof(s_snap));
  s_have_odom = 0U;

  HAL_NVIC_SetPriority(USART2_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_dma_rx, RX_CHUNK);
}

bool OdomUartRx_GetSnapshot(OdomUartRxSnapshot *out)
{
  if (out == NULL || s_have_odom == 0U) {
    return false;
  }
  __disable_irq();
  *out = s_snap;
  __enable_irq();
  return true;
}
