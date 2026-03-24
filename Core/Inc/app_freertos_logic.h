#ifndef APP_FREERTOS_LOGIC_H
#define APP_FREERTOS_LOGIC_H

#include <stdint.h>

extern volatile uint8_t pump_state;
extern volatile int rx_ready;
extern volatile int alert;

void AppTasks_StartDefault(void *argument);
void AppTasks_SmokeStep(void);
void AppTasks_PwmInit(void);
void AppTasks_PwmStep(void);
void AppTasks_PumpStep(void);
void AppTasks_OnExti(uint16_t GPIO_Pin);

#endif /* APP_FREERTOS_LOGIC_H */
