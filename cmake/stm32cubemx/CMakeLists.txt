# STM32CubeMX: HAL + FreeRTOS (static library). Regenerate/adjust when CubeMX changes drivers.
# CMAKE_SOURCE_DIR = project root

set(DRV ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src)
set(RT ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source)

add_library(stm32cubemx STATIC
  ${DRV}/stm32f4xx_hal.c
  ${DRV}/stm32f4xx_hal_adc.c
  ${DRV}/stm32f4xx_hal_adc_ex.c
  ${DRV}/stm32f4xx_hal_cortex.c
  ${DRV}/stm32f4xx_hal_dma.c
  ${DRV}/stm32f4xx_hal_dma_ex.c
  ${DRV}/stm32f4xx_hal_exti.c
  ${DRV}/stm32f4xx_hal_flash.c
  ${DRV}/stm32f4xx_hal_flash_ex.c
  ${DRV}/stm32f4xx_hal_flash_ramfunc.c
  ${DRV}/stm32f4xx_hal_gpio.c
  ${DRV}/stm32f4xx_hal_pwr.c
  ${DRV}/stm32f4xx_hal_pwr_ex.c
  ${DRV}/stm32f4xx_hal_rcc.c
  ${DRV}/stm32f4xx_hal_rcc_ex.c
  ${DRV}/stm32f4xx_hal_tim.c
  ${DRV}/stm32f4xx_hal_tim_ex.c
  ${DRV}/stm32f4xx_hal_uart.c
  ${DRV}/stm32f4xx_ll_adc.c
  ${RT}/croutine.c
  ${RT}/event_groups.c
  ${RT}/list.c
  ${RT}/queue.c
  ${RT}/stream_buffer.c
  ${RT}/tasks.c
  ${RT}/timers.c
  ${RT}/CMSIS_RTOS_V2/cmsis_os2.c
  ${RT}/portable/MemMang/heap_4.c
  ${RT}/portable/GCC/ARM_CM4F/port.c
)

target_include_directories(stm32cubemx PUBLIC
  ${CMAKE_SOURCE_DIR}/Core/Inc
  ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
  ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
  ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
  ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
  ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
  ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
  ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

target_compile_definitions(stm32cubemx PUBLIC
  STM32F407xx
  USE_HAL_DRIVER
)
