/**
 ******************************************************************************
 * @file    microros_uart4.c
 * @brief   UART4 与香橙派 Micro-ROS 通讯：发布/订阅模板实现
 *          帧格式: 0x55 0xAA | topic_id(1) | len(2 LSB) | payload(n) | xor_checksum(1)
 ******************************************************************************
 */
#include "microros_uart4.h"
#include "usart.h"
#include "cmsis_os.h"
#include "main.h"
#include <string.h>

/* UART4 发送互斥量由 freertos.c 创建 */
extern osMutexId_t UART4_ProtectHandle;

#define STX0  0x55
#define STX1  0xAA

#define FRAME_HEADER_LEN  5   /* STX(2) + topic_id(1) + len(2) */
#define FRAME_TAIL_LEN    1   /* checksum */

static uint8_t s_rx_buf[MICROROS_UART4_RX_BUF_SIZE];
static MicroROS_SubCallback_t s_sub_cb = NULL;

static uint8_t calc_checksum(uint8_t topic_id, uint16_t len, const uint8_t *data, uint16_t data_len)
{
    uint8_t xor_val = topic_id ^ (uint8_t)(len & 0xFF) ^ (uint8_t)(len >> 8);
    for (uint16_t i = 0; i < data_len && data != NULL; i++)
        xor_val ^= data[i];
    return xor_val;
}

static void restart_rx(void)
{
    (void)HAL_UARTEx_ReceiveToIdle_IT(&huart4, s_rx_buf, sizeof(s_rx_buf));
}

/* 在 UART 接收完成（到 Idle 或满）时由 HAL 调用 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart != &huart4 || Size < FRAME_HEADER_LEN + FRAME_TAIL_LEN)
    {
        restart_rx();
        return;
    }

    if (s_rx_buf[0] != STX0 || s_rx_buf[1] != STX1)
    {
        restart_rx();
        return;
    }

    uint8_t  topic_id = s_rx_buf[2];
    uint16_t len      = (uint16_t)s_rx_buf[3] | ((uint16_t)s_rx_buf[4] << 8);
    if (len > MICROROS_UART4_MAX_PAYLOAD || (Size != (uint16_t)(FRAME_HEADER_LEN + len + FRAME_TAIL_LEN)))
    {
        restart_rx();
        return;
    }

    uint8_t expect_csum = calc_checksum(topic_id, len, &s_rx_buf[FRAME_HEADER_LEN], len);
    if (s_rx_buf[FRAME_HEADER_LEN + len] != expect_csum)
    {
        restart_rx();
        return;
    }

    if (s_sub_cb != NULL)
        s_sub_cb(topic_id, &s_rx_buf[FRAME_HEADER_LEN], len);

    restart_rx();
}

void MicroROS_UART4_Init(void)
{
    s_sub_cb = NULL;
    restart_rx();
}

int MicroROS_Publish(uint8_t topic_id, const uint8_t *data, uint16_t len)
{
    if (len > MICROROS_UART4_MAX_PAYLOAD)
        return -1;
    if (data == NULL && len != 0)
        return -1;

    if (osMutexAcquire(UART4_ProtectHandle, 100) != osOK)
        return -2;

    uint8_t frame[MICROROS_UART4_RX_BUF_SIZE];
    frame[0] = STX0;
    frame[1] = STX1;
    frame[2] = topic_id;
    frame[3] = (uint8_t)(len & 0xFF);
    frame[4] = (uint8_t)(len >> 8);
    if (len > 0 && data != NULL)
        memcpy(&frame[FRAME_HEADER_LEN], data, len);
    frame[FRAME_HEADER_LEN + len] = calc_checksum(topic_id, len, data, len);

    uint16_t frame_len = FRAME_HEADER_LEN + len + FRAME_TAIL_LEN;
    HAL_StatusTypeDef st = HAL_UART_Transmit(&huart4, frame, frame_len, 50);

    osMutexRelease(UART4_ProtectHandle);

    return (st == HAL_OK) ? 0 : -3;
}

void MicroROS_RegisterSubCallback(MicroROS_SubCallback_t cb)
{
    s_sub_cb = cb;
}
