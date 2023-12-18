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
#include "stm32g0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_CS_Pin GPIO_PIN_9
#define SW_CS_GPIO_Port GPIOB
#define RGB_CS_Pin GPIO_PIN_4
#define RGB_CS_GPIO_Port GPIOA
#define BB_CLK_Pin GPIO_PIN_5
#define BB_CLK_GPIO_Port GPIOA
#define BB_MISO_Pin GPIO_PIN_6
#define BB_MISO_GPIO_Port GPIOA
#define BB_MOSI_Pin GPIO_PIN_7
#define BB_MOSI_GPIO_Port GPIOA
#define SH_LD_Pin GPIO_PIN_2
#define SH_LD_GPIO_Port GPIOB
#define STATUS_LED_Pin GPIO_PIN_5
#define STATUS_LED_GPIO_Port GPIOB
#define RS485_CNTL_Pin GPIO_PIN_8
#define RS485_CNTL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define EMPTY_SLOT 0xFFFF

//typedef enum SLOTS_7{
//Slot1 = 0x00,
//Slot2 = 0x01,
//Slot3 = 0x02,
//Slot4 = 0x03,
//Slot5 = 0x04,
//Slot6 = 0x05,
//Slot7 = 0x06,
//}Slot;

typedef struct SLOT_7{
	uint16_t Slot_1  : 1;
	uint16_t Slot_2  : 1;
	uint16_t Slot_3  : 1;
	uint16_t Slot_4  : 1;
	uint16_t Slot_5  : 1;
	uint16_t Slot_6  : 1;
	uint16_t Slot_7  : 1;
	uint16_t Slot_8  : 1;
	uint16_t Slot_9  : 1;
	uint16_t Slot_10 : 1;
	uint16_t Slot_11 : 1;
	uint16_t Slot_12 : 1;
	uint16_t Slot_13 : 1;
	uint16_t Slot_14 : 1;
	uint16_t Slot_15 : 1;
}Slot15_t;

//typedef struct SLOT_PARAMETER{
//	uint8_t EmptyLocaction;
//	uint8_t OcupiedLocation;
//	uint8_t PickupLocation;
//	uint8_t PlaceError;
//	uint8_t PickupError;
//}Slot_Para_t;

typedef enum SLOT_PARAMETER{EmptyLocaction = 0x00,OcupiedLocation,PickupLocation,PlaceError,PickupError}Slot_Para_t;
typedef enum LED_COLOUR{RGBLedOff = 0x00,RGBRed,RGBGreen,RGBBlue,RGBYellow,RGBPink,RGBCyan,RGBWhite}LedColour_t;
typedef enum MODE_STATUS{CheckStatus = 0x00,PresentUpdate,LedUpdate,LoadRequest,PickupRequest}ModeStaus_t;

/* RGBLed.c */
void RGBInit(void);
void registersWrite(int index, int value);
void Red(int startPin, int endPin, int ledBT);
void Green(int startPin, int endPin, int ledBT);
void Blue(int startPin, int endPin, int ledBT);
void Yellow(int startPin, int endPin, int ledBT);
void Pink(int startPin, int endPin, int ledBT);
void Cyan(int startPin, int endPin, int ledBT);
void White(int startPin, int endPin, int ledBT);
void LedOff(int startPin, int endPin, int ledBT);
void AllLedOff(int clearSpeed);
void UpdateSlotLeds(uint16_t Data, Slot_Para_t status);
/* Slot.c */
void SlotCheck(uint32_t channel);
void SlotDataChek(void);
void delay(uint32_t var);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
