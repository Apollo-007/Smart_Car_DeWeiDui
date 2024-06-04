/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define XJ_R_Pin GPIO_PIN_0
#define XJ_R_GPIO_Port GPIOA
#define XJ_L_Pin GPIO_PIN_1
#define XJ_L_GPIO_Port GPIOA
#define Pipe_Pin GPIO_PIN_0
#define Pipe_GPIO_Port GPIOB
#define EncoderA_L_Pin GPIO_PIN_6
#define EncoderA_L_GPIO_Port GPIOC
#define EncoderB_L_Pin GPIO_PIN_7
#define EncoderB_L_GPIO_Port GPIOC
#define IN1_L_Pin GPIO_PIN_8
#define IN1_L_GPIO_Port GPIOA
#define IN2_L_Pin GPIO_PIN_9
#define IN2_L_GPIO_Port GPIOA
#define IN1_R_Pin GPIO_PIN_10
#define IN1_R_GPIO_Port GPIOA
#define IN2_R_Pin GPIO_PIN_11
#define IN2_R_GPIO_Port GPIOA
#define Button1_Pin GPIO_PIN_15
#define Button1_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_2
#define OLED_RST_GPIO_Port GPIOD
#define OLED_DC_Pin GPIO_PIN_3
#define OLED_DC_GPIO_Port GPIOB
#define Button2_Pin GPIO_PIN_4
#define Button2_GPIO_Port GPIOB
#define Button3_Pin GPIO_PIN_5
#define Button3_GPIO_Port GPIOB
#define EncoderA_R_Pin GPIO_PIN_6
#define EncoderA_R_GPIO_Port GPIOB
#define EncoderB_R_Pin GPIO_PIN_7
#define EncoderB_R_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOB
#define UKEY_Pin GPIO_PIN_9
#define UKEY_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
