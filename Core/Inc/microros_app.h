#ifndef MICROROS_APP_H
#define MICROROS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * micro-ROS 主循环：初始化 UART4 传输、节点与发布者，周期性发布
 * /stm32/sensor（Int32 原始 ADC）与 /stm32/mq2/smoke_ppm（Float32）。
 * 在 FreeRTOS 任务中调用，不返回。
 */
void microros_app_run(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* MICROROS_APP_H */
