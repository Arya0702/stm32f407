#ifndef MQ2_SERIAL_TEST_H
#define MQ2_SERIAL_TEST_H

/**
 * 置 1：默认任务只跑 ADC + MQ2 解算，经 USART6 输出文本（不启动 micro-ROS / UART4）。
 * 验证完解算与硬件后改回 0 再编译烧录。
 */
#ifndef USE_MQ2_SERIAL_TEST_ONLY
/* 调试 MQ2 时改为 1：不启动 micro-ROS，仅 USART6 输出；用完务必改回 0 */
#define USE_MQ2_SERIAL_TEST_ONLY 0
#endif

void mq2_serial_test_run(void *argument);

#endif
