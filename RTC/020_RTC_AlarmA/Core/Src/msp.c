/*
 * msp.c
 *
 *  Created on: Mar 30, 2026
 *      Author: nachiket
 */

#include "main.h"

void HAL_MspInit(void)
{
	//here we will do low level processor specific inits
	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR|=(0x7<<16); //usg fault, mem fault and bus fault system exceotions enbaled

	//3. Configure the priority for the sytem exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
	//Dealing with the low level initialisation of the usart2 peripheral

	//1. Enable the clock for the usart2 and gpioa peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_SLEEP_DISABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. Do the pin muxing configurations
	gpio_uart.Pin=GPIO_PIN_2; //USART2_TX
	gpio_uart.Mode=GPIO_MODE_AF_PP;
	gpio_uart.Pull=GPIO_PULLUP;
	gpio_uart.Speed=GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate=GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin=GPIO_PIN_3; //USART2_RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. Enable the IRQ and set up the priority (NVIC Settings)
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef RCC_RTCPeriClkInit;

	//1. Turn on LSE
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState=RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;

	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		Error_handler();

	//2. Select LSE as RTCCLK
	RCC_RTCPeriClkInit.PeriphClockSelection=RCC_PERIPHCLK_RTC;
	RCC_RTCPeriClkInit.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;

	if(HAL_RCCEx_PeriphCLKConfig(&RCC_RTCPeriClkInit) != HAL_OK)
		Error_handler();

	//3. Enable the RTC clock
	__HAL_RCC_RTC_ENABLE();

	//4. Enable the RTC alarm IRQ in the NVIC
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
