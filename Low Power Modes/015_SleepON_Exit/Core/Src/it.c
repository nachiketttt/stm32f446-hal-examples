/*
 * it.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

#include <main.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htimer6);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}
