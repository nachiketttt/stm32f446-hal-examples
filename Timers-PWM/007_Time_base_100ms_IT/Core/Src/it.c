/*
 * it.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

#include "main.h"

extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);
}
