#include "IRremote.h"
#include "main.h"
///////////////////////////// счётчик микросекунд ///////////////////////////////
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim1;

//void DWT_Init()
//{
//    SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
//	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
//}
/////////////////////////////////////////////////////////////////////////////////

#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

void sendNEC (unsigned long data,  int nbits, uint32_t channel)
{
	// Set IR carrier frequency

	// Header
	mark(NEC_HDR_MARK,channel);
	space(NEC_HDR_SPACE,channel);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			mark(NEC_BIT_MARK,channel);
			space(NEC_ONE_SPACE,channel);
		} else {
			mark(NEC_BIT_MARK,channel);
			space(NEC_ZERO_SPACE,channel);
		}
	}

	// Footer
	mark(NEC_BIT_MARK,channel);
	space(0,channel);  // Always end with the LED off
}
void sendRaw(unsigned int buf[], unsigned int len, uint8_t hz,uint32_t channel)
{
//	enableIROut(hz);

	for(uint16_t i = 0; i < len; i++)
	{
		if(i & 1) space(buf[i],channel);
		else mark(buf[i],channel);
	}

	space(0,channel);
}

void mark(unsigned int time,uint32_t channel)
{
	HAL_TIM_PWM_Start(&htim1, channel);
	if (time > 0) custom_delay_usec(time);
}

void space(unsigned int time,uint32_t channel)
{
	HAL_TIM_PWM_Stop(&htim1, channel);
	if(time > 0) custom_delay_usec(time);
}

void enableIROut(uint8_t khz)
{
	uint16_t pwm_freq = 0;
	uint16_t pwm_pulse = 0;
	pwm_freq = MYSYSCLOCK / (khz * 1000) - 1;
	pwm_pulse = pwm_freq / 3;

	HAL_TIM_Base_DeInit(&htim1);

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = pwm_freq;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim1);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);
	HAL_TIM_PWM_Init(&htim1);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pwm_pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
	Error_Handler();
	}
	HAL_TIM_MspPostInit(&htim1);
}

void custom_delay_usec(unsigned long us)
{
	__HAL_TIM_SET_COUNTER(&htim3,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim3) < us);  // wait for the counter to reach the us input in the parameter

}

