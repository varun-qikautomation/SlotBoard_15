/*
 * Slot.c
 *
 *  Created on: 14-Mar-2023
 *      Author: chethan.k
 */


#include "main.h"
#include "IRremote.h"




extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

uint8_t bitIndexA =0,bitIndexB = 0,bitIndexC = 0,bitIndexD = 0;
uint8_t CapChannelA = 0,CapChannelB = 0,CapChannelC = 0,CapChannelD = 0;
uint8_t ReceivedA = 0,ReceivedB = 0,ReceivedC = 0,ReceivedD = 0;
//uint8_t SlotBuff[8];
Slot15_t Slot;



void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
//	 Readbuff[j++] = __HAL_TIM_GET_COUNTER(&htim2);
    if (__HAL_TIM_GET_COUNTER(&htim2) > 8000)
    {
      CapChannelA = 0;
      bitIndexA = 0;
    }
    else if (__HAL_TIM_GET_COUNTER(&htim2) > 2000)
    {
	  CapChannelA |= (1UL << (7-bitIndexA));   // write 1
      bitIndexA++;
    }
    else if (__HAL_TIM_GET_COUNTER(&htim2) > 1000)
    {
      CapChannelA &= ~(1UL << (7-bitIndexA));  // write 0
      bitIndexA++;
    }
    if(bitIndexA == 8)
    {
//    	if(CapChannelA == 0xAA)
		  ReceivedA = 1;
    	 bitIndexA = 0;
//      i = 0;
    }
//	if( j == 32)
//		j = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
  }

  if(GPIO_Pin == GPIO_PIN_1)
  {
//	Readbuff[j++] = __HAL_TIM_GET_COUNTER(&htim14);
	if (__HAL_TIM_GET_COUNTER(&htim14) > 8000)
	{
	CapChannelB = 0;
	bitIndexB = 0;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim14) > 2000)
	{
	CapChannelB |= (1UL << (7-bitIndexB));   // write 1
	bitIndexB++;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim14) > 1000)
	{
	CapChannelB &= ~(1UL << (7-bitIndexB));  // write 0
	bitIndexB++;
	}
	if(bitIndexB == 8)
	{
//	if(CapChannelB == 0xAA)
	  ReceivedB = 1;
	  bitIndexB = 0;

	}
//	if( j == 32)
//		j = 0;
	__HAL_TIM_SET_COUNTER(&htim14, 0);
  }
  if(GPIO_Pin == GPIO_PIN_2)
  {
//	Readbuff[j++] = __HAL_TIM_GET_COUNTER(&htim14);
	if (__HAL_TIM_GET_COUNTER(&htim16) > 8000)
	{
	CapChannelC = 0;
	bitIndexC = 0;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim16) > 2000)
	{
	CapChannelC |= (1UL << (7-bitIndexC));   // write 1
	bitIndexC++;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim16) > 1000)
	{
	CapChannelC &= ~(1UL << (7-bitIndexC));  // write 0
	bitIndexC++;
	}
	if(bitIndexC == 8)
	{
//	if(CapChannelB == 0xAA)
		ReceivedC = 1;
		bitIndexC = 0;

	}
//	if( j == 32)
//		j = 0;
	__HAL_TIM_SET_COUNTER(&htim16, 0);
  }
  if(GPIO_Pin == GPIO_PIN_3)
  {
//	Readbuff[j++] = __HAL_TIM_GET_COUNTER(&htim14);
	if (__HAL_TIM_GET_COUNTER(&htim17) > 8000)
	{
	CapChannelD = 0;
	bitIndexD = 0;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim17) > 2000)
	{
	CapChannelD |= (1UL << (7-bitIndexD));   // write 1
	bitIndexD++;
	}
	else if (__HAL_TIM_GET_COUNTER(&htim17) > 1000)
	{
	CapChannelD &= ~(1UL << (7-bitIndexD));  // write 0
	bitIndexD++;
	}
	if(bitIndexD == 8)
	{
//	if(CapChannelB == 0xAA)
	  ReceivedD = 1;
	  bitIndexD = 0;

	}
//	if( j == 32)
//		j = 0;
	__HAL_TIM_SET_COUNTER(&htim17, 0);
  }

}



void SlotCheck(uint32_t channel)
{
//uint8_t i = 0,SuccessA = 0,SuccessB = 0;
	sendNEC(0xAA,8,channel);
	delay(10);
	switch(channel)
	{
		case TIM_CHANNEL_1:
						if(ReceivedA)
							Slot.Slot_1 = 0;
						else
							Slot.Slot_1 = 1;
						if(ReceivedB)
							Slot.Slot_5 = 0;
						else
							Slot.Slot_5 = 1;
						if(ReceivedC)
							Slot.Slot_9 = 0;
						else
							Slot.Slot_9 = 1;
						if(ReceivedD)
							Slot.Slot_13 = 0;
						else
							Slot.Slot_13 = 1;
						break;
		case TIM_CHANNEL_2:
						if(ReceivedA)
							Slot.Slot_2 = 0;
						else
							Slot.Slot_2 = 1;
						if(ReceivedB)
							Slot.Slot_6 = 0;
						else
							Slot.Slot_6 = 1;
						if(ReceivedC)
							Slot.Slot_10 = 0;
						else
							Slot.Slot_10 = 1;
						if(ReceivedD)
							Slot.Slot_14 = 0;
						else
							Slot.Slot_14 = 1;
						break;
		case TIM_CHANNEL_3:
						if(ReceivedA)
							Slot.Slot_3 = 0;
						else
							Slot.Slot_3 = 1;
						if(ReceivedB)
							Slot.Slot_7 = 0;
						else
							Slot.Slot_7 = 1;
						if(ReceivedC)
							Slot.Slot_11 = 0;
						else
							Slot.Slot_11 = 1;
						if(ReceivedD)
							Slot.Slot_15 = 0;
						else
							Slot.Slot_15 = 1;
						break;
		case TIM_CHANNEL_4:
						if(ReceivedA)
							Slot.Slot_4 = 0;
						else
							Slot.Slot_4 = 1;
						if(ReceivedB)
							Slot.Slot_8 = 0;
						else
							Slot.Slot_8 = 1;
						if(ReceivedC)
							Slot.Slot_12 = 0;
						else
							Slot.Slot_12 = 1;
						break;
		default:
						break;

	}

	if(ReceivedA)
		ReceivedA = 0;
	if(ReceivedB)
		ReceivedB = 0;
	if(ReceivedC)
		ReceivedC = 0;
	if(ReceivedD)
		ReceivedD = 0;
}

void SlotDataChek(void)
{
//static int slotint = 0;
//	if(slotint == 0){
//		HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
//		HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
//		slotint =1;
//	}
//	}else{
//		HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
//		HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
//		slotint =0;
//	}
//	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
//	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
//	HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
//	HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
	SlotCheck(TIM_CHANNEL_1);
	SlotCheck(TIM_CHANNEL_2);
	SlotCheck(TIM_CHANNEL_3);
	SlotCheck(TIM_CHANNEL_4);
}
