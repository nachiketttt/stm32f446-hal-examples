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


