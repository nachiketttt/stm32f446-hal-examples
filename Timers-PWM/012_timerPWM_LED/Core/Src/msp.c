/*
 * msp.c
 *
 *  Created on: Mar 13, 2026
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

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef tim2OC_ch_gpios;

	//1. Enable the clock for timer 2
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. Configure the gpio pins to behave as the timer 2 channel 1, 2, 3, 4
	/*
	 * PA0 -->TIM2_CH1
	 */
	tim2OC_ch_gpios.Pin=GPIO_PIN_0;
	tim2OC_ch_gpios.Mode=GPIO_MODE_AF_PP;
	tim2OC_ch_gpios.Pull=GPIO_NOPULL;
	tim2OC_ch_gpios.Speed=GPIO_SPEED_FREQ_LOW;
	tim2OC_ch_gpios.Alternate=GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &tim2OC_ch_gpios);

	//3. nvic settings
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
	//Dealing with the low level initialisation of the usart2 peripheral

	//1. Enable the clock for the usart2 and gpioa peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
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
