/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOE
#define OLED_RST_Pin GPIO_PIN_6
#define OLED_RST_GPIO_Port GPIOF
#define OLED_DC_Pin GPIO_PIN_8
#define OLED_DC_GPIO_Port GPIOF
#define Buzzer_Pin GPIO_PIN_9
#define Buzzer_GPIO_Port GPIOF
#define UKEY_Pin GPIO_PIN_0
#define UKEY_GPIO_Port GPIOA
#define E1A_Pin GPIO_PIN_1
#define E1A_GPIO_Port GPIOA
#define XJ_L_Pin GPIO_PIN_2
#define XJ_L_GPIO_Port GPIOA
#define XJ_R_Pin GPIO_PIN_3
#define XJ_R_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOA
#define E2A_Pin GPIO_PIN_6
#define E2A_GPIO_Port GPIOA
#define Regulator1_Pin GPIO_PIN_0
#define Regulator1_GPIO_Port GPIOB
#define Regulator2_Pin GPIO_PIN_1
#define Regulator2_GPIO_Port GPIOB
#define Button1_Pin GPIO_PIN_10
#define Button1_GPIO_Port GPIOB
#define Button2_Pin GPIO_PIN_12
#define Button2_GPIO_Port GPIOB
#define Button3_Pin GPIO_PIN_14
#define Button3_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOG
#define E2B_Pin GPIO_PIN_7
#define E2B_GPIO_Port GPIOC
#define IN3_Pin GPIO_PIN_8
#define IN3_GPIO_Port GPIOC
#define IN4_Pin GPIO_PIN_9
#define IN4_GPIO_Port GPIOC
#define ENA_Pin GPIO_PIN_8
#define ENA_GPIO_Port GPIOA
#define ENB_Pin GPIO_PIN_9
#define ENB_GPIO_Port GPIOA
#define IN1_Pin GPIO_PIN_10
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_11
#define IN2_GPIO_Port GPIOA
#define E1B_Pin GPIO_PIN_15
#define E1B_GPIO_Port GPIOA
#define Pipe_Pin GPIO_PIN_11
#define Pipe_GPIO_Port GPIOC
#define TX_Pin GPIO_PIN_12
#define TX_GPIO_Port GPIOC
#define RX_Pin GPIO_PIN_2
#define RX_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
