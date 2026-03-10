/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define alert_smoke_Pin GPIO_PIN_7
#define alert_smoke_GPIO_Port GPIOA
#define alert_smoke_EXTI_IRQn EXTI9_5_IRQn
#define MI1602data_readytoreceive_Pin GPIO_PIN_0
#define MI1602data_readytoreceive_GPIO_Port GPIOD
#define MI1602data_readytoreceive_EXTI_IRQn EXTI0_IRQn
#define MI1602_ss_Pin GPIO_PIN_2
#define MI1602_ss_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define MI48_DATA_READY_Pin GPIO_PIN_0
#define MI48_DATA_READY_GPIO_Port GPIOD
#define MI48_RST_Pin GPIO_PIN_1
#define MI48_RST_GPIO_Port GPIOD
#define MI48_SS_Pin GPIO_PIN_2
#define MI48_SS_GPIO_Port GPIOD
#define MI48_SCK_Pin GPIO_PIN_10
#define MI48_SCK_GPIO_Port GPIOC
#define MI48_MISO_Pin GPIO_PIN_11
#define MI48_MISO_GPIO_Port GPIOC
#define MI48_MOSI_Pin GPIO_PIN_7
#define MI48_MOSI_GPIO_Port GPIO
/*
#define MI48_SSA15_Pin GPIO_PIN_15
#define MI48_SSA15_GPIO_Port GPIOA
#define MI48_SCKC10_Pin GPIO_PIN_10
#define MI48_SCKC10_GPIO_Port GPIOC
#define MI48_MISOC11_Pin GPIO_PIN_11
#define MI48_MISOC11_GPIO_Port GPIOC
#define MI48_MOSIC12_Pin GPIO_PIN_12
#define MI48_MOSIC12_GPIO_Port GPIOC*/
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
