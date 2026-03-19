/*
 * it.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

#include "main.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

