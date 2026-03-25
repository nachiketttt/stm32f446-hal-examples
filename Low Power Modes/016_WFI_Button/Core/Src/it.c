/*
 * it.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

#include "main.h"

extern UART_HandleTypeDef huart2;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
