#ifndef ODOM_UART_RX_H
#define ODOM_UART_RX_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float x_m;
  float y_m;
  float theta_rad;
  float vx_m_s;
  float vy_m_s;
  float vtheta_rad_s;
  uint32_t host_tick_ms;
} OdomUartRxSnapshot;

void OdomUartRx_Init(void);
bool OdomUartRx_GetSnapshot(OdomUartRxSnapshot *out);

#endif
