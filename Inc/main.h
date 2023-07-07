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
#include "ST7735.h"
#include "MAX6675.h"
#include "GFX_FUNCTIONS.h"

//#include "GFX_FUNCTIONS.h"
#include "fonts.h"
#include "string.h"
#include "stdio.h"
#include <home_screen.h>
#include <math.h>
#include "PID.h"
#include "encoder.h"
#include <stdbool.h>
#include "menu.h"
#include <stdlib.h>
#include "CRN_Flash.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern uint8_t encoder_a_input;
extern uint8_t encoder_b_input;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* USER CODE BEGIN EFP */
void UpdatePWM(uint32_t pwm_amount);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define DISPL_MOSI_Pin GPIO_PIN_3
#define DISPL_MOSI_GPIO_Port GPIOC
#define Enc_A_Pin GPIO_PIN_0
#define Enc_A_GPIO_Port GPIOA
#define Enc_B_Pin GPIO_PIN_1
#define Enc_B_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_2
#define USART_RX_GPIO_Port GPIOA
#define USART_RXA3_Pin GPIO_PIN_3
#define USART_RXA3_GPIO_Port GPIOA
#define HeartBeat_Pin GPIO_PIN_5
#define HeartBeat_GPIO_Port GPIOA
#define MAX6675_MISO_Pin GPIO_PIN_6
#define MAX6675_MISO_GPIO_Port GPIOA
#define MAX6675_CS_Pin GPIO_PIN_7
#define MAX6675_CS_GPIO_Port GPIOA
#define DISPL_SCK_Pin GPIO_PIN_10
#define DISPL_SCK_GPIO_Port GPIOB
#define DISPL_CS_Pin GPIO_PIN_12
#define DISPL_CS_GPIO_Port GPIOB
#define DISPL_RST_Pin GPIO_PIN_6
#define DISPL_RST_GPIO_Port GPIOC
#define DISPL_DC_Pin GPIO_PIN_7
#define DISPL_DC_GPIO_Port GPIOC
#define DISPL_LED_Pin GPIO_PIN_8
#define DISPL_LED_GPIO_Port GPIOC
#define Enc_SW_Pin GPIO_PIN_10
#define Enc_SW_GPIO_Port GPIOA
#define Enc_SW_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define MAX6675_SCK_Pin GPIO_PIN_3
#define MAX6675_SCK_GPIO_Port GPIOB
#define Heater_PWD_Pin GPIO_PIN_4
#define Heater_PWD_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
