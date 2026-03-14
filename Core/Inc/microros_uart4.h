/**
 ******************************************************************************
 * @file    microros_uart4.h
 * @brief   UART4 与香橙派 Micro-ROS 通讯模板：订阅节点 + 发布节点
 *          帧格式: STX(2) + topic_id(1) + len(2) + payload(n) + checksum(1)
 ******************************************************************************
 */
#ifndef MICROROS_UART4_H
#define MICROROS_UART4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 单帧最大 payload 长度，总帧长 <= MICROROS_UART4_RX_BUF_SIZE */
#define MICROROS_UART4_MAX_PAYLOAD  256
#define MICROROS_UART4_RX_BUF_SIZE  (2 + 1 + 2 + MICROROS_UART4_MAX_PAYLOAD + 1)

/* 与香橙派约定的 Topic ID（可按实际 ROS2 话题扩展） */
typedef enum {
    MICROROS_TOPIC_CMD_FROM_PI   = 0x00,  /* 香橙派下发的命令 → STM32 订阅 */
    MICROROS_TOPIC_SENSOR_TO_PI  = 0x01,  /* STM32 传感器数据 → 发布到香橙派 */
    MICROROS_TOPIC_ALERT_TO_PI   = 0x02,  /* STM32 告警信息 → 发布到香橙派 */
    MICROROS_TOPIC_CUSTOM        = 0x80,  /* 0x80~0xFF 自定义 */
} MicroROS_TopicId_t;

/* 订阅回调：从香橙派收到一帧时调用（在中断上下文，请尽快返回或投递到队列） */
typedef void (*MicroROS_SubCallback_t)(uint8_t topic_id, const uint8_t *data, uint16_t len);

/**
 * @brief 初始化 UART4 Micro-ROS 通讯（注册接收回调并启动接收）
 * @note  需在 MX_UART4_Init() 和 FreeRTOS 启动之后调用（若用互斥量则需在创建 mutex 后调用）
 */
void MicroROS_UART4_Init(void);

/**
 * @brief 发布一帧到香橙派（发送节点模板）
 * @param topic_id 话题 ID，见 MicroROS_TopicId_t
 * @param data      payload 指针，可为 NULL（当 len==0）
 * @param len       payload 长度，0 ~ MICROROS_UART4_MAX_PAYLOAD
 * @return 0 成功，非 0 失败（如互斥量超时、发送错误）
 */
int MicroROS_Publish(uint8_t topic_id, const uint8_t *data, uint16_t len);

/**
 * @brief 注册订阅回调（收到香橙派数据时调用）
 * @param cb 回调函数，NULL 表示不处理
 */
void MicroROS_RegisterSubCallback(MicroROS_SubCallback_t cb);

#ifdef __cplusplus
}
#endif

#endif /* MICROROS_UART4_H */
