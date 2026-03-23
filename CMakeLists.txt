cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Project_stm32f407)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# STM32CubeMX HAL + FreeRTOS (must exist — see cmake/stm32cubemx/CMakeLists.txt)
set(STM32CUBEMX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32cubemx")
if(NOT EXISTS "${STM32CUBEMX_DIR}/CMakeLists.txt")
  message(FATAL_ERROR
    "Missing ${STM32CUBEMX_DIR}/CMakeLists.txt — CMake cannot configure. "
    "Restore this file from the repo or regenerate the STM32CubeMX CMake export.")
endif()
add_subdirectory("${STM32CUBEMX_DIR}")

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Application sources (CubeMX Core/Src + micro-ROS)
set(STARTUP_ASM "${CMAKE_SOURCE_DIR}/startup_stm32f407xx.s")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${STARTUP_ASM}
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/freertos.c
    Core/Src/adc.c
    Core/Src/dma.c
    Core/Src/tim.c
    Core/Src/usart.c
    Core/Src/odom_uart_rx.c
    Core/Src/mq2_smoke.c
    Core/Src/mq2_serial_test.c
    Core/Src/stm32f4xx_it.c
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_hal_timebase_tim.c
    Core/Src/system_stm32f4xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    Core/Src/microros_app.c
    Core/Src/microros_transport_uart4.c
)

set_source_files_properties(${STARTUP_ASM} PROPERTIES LANGUAGE ASM)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
    microros/include
    microros/include/microros_include
)
# Add project symbols (macros)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    STM32F407xx
    USE_HAL_DRIVER
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    stm32cubemx
    m
    ${CMAKE_CURRENT_SOURCE_DIR}/microros/include/libmicroros.a
)
