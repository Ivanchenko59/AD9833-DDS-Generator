/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include <stdio.h>
#include <string.h>
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
#define ST7735_SCK_Pin GPIO_PIN_5
#define ST7735_SCK_GPIO_Port GPIOA
#define ST7735_SDA_Pin GPIO_PIN_7
#define ST7735_SDA_GPIO_Port GPIOA
#define ENC_S1_Pin GPIO_PIN_8
#define ENC_S1_GPIO_Port GPIOA
#define ENC_S2_Pin GPIO_PIN_9
#define ENC_S2_GPIO_Port GPIOA
#define ENC_BTN_Pin GPIO_PIN_10
#define ENC_BTN_GPIO_Port GPIOA
#define ST7735_CS_Pin GPIO_PIN_6
#define ST7735_CS_GPIO_Port GPIOB
#define ST7735_RST_Pin GPIO_PIN_7
#define ST7735_RST_GPIO_Port GPIOB
#define ST7735_DC_Pin GPIO_PIN_8
#define ST7735_DC_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
