/*
 * main.c
 *
 *  Created on: Mar 22, 2026
 *      Author: nachiket
 */

#include <main.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"

#define TRUE     1
#define FALSE    0

void UART2_Init(void);
void Error_handler(void);
void SystemClock_Config_HSE(uint8_t clock_freq);
void TIMER6_Init(void);
void GPIO_Init(void);

extern uint8_t some_data[];
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htimer6;

int main(void)
{
	HAL_Init();

	//SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	UART2_Init();

	TIMER6_Init();

	/*
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	*/

	HAL_PWR_EnableSleepOnExit();

	//making the SR 0 with every start to avoid any other interrupts
	TIM6->SR=0;

	//Lets start the timer in IT mode
	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

	return 0;
}

void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t FLatecncy=0;

	osc_init.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState=RCC_HSE_BYPASS;
	osc_init.PLL.PLLState=RCC_PLL_ON;
	osc_init.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM=8;
		osc_init.PLL.PLLN=100;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		FLatecncy=FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM=8;
		osc_init.PLL.PLLN=168;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		FLatecncy=FLASH_ACR_LATENCY_2WS;
		break;
	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM=8;
		osc_init.PLL.PLLN=240;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		FLatecncy=FLASH_ACR_LATENCY_3WS;
		break;
	}
	case SYS_CLOCK_FREQ_180_MHZ:
	{
		//Enable the clock for the power controller
		__HAL_RCC_PWR_CLK_ENABLE();

		//Set regulator voltage scale as 1
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		//Turn on the over drive mode  of the voltage regulator
		__HAL_PWR_OVERDRIVE_ENABLE();

		osc_init.PLL.PLLM=8;
		osc_init.PLL.PLLN=360;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		FLatecncy=FLASH_ACR_LATENCY_3WS;
		break;
	}
	default:
		return;
	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
		Error_handler();

	if(HAL_RCC_ClockConfig(&clk_init, FLatecncy) != HAL_OK)
		Error_handler();

	//SYSTICK config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void UART2_Init(void)
{
	huart2.Instance=USART2;
	huart2.Init.BaudRate=115200;
	huart2.Init.WordLength=UART_WORDLENGTH_8B;
	huart2.Init.StopBits=UART_STOPBITS_1;
	huart2.Init.Parity=UART_PARITY_NONE;
	huart2.Init.HwFlowCtl=UART_HWCONTROL_NONE;
	huart2.Init.Mode=UART_MODE_TX_RX;
	if(HAL_UART_Init(&huart2) != HAL_OK)
	{
		//there is problem
		Error_handler();
	}
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin=GPIO_PIN_5;
	ledgpio.Mode=GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);

	ledgpio.Pin=GPIO_PIN_12;
	ledgpio.Mode=GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}

void TIMER6_Init(void)
{
	htimer6.Instance=TIM6;
	htimer6.Init.Prescaler=4999;
	htimer6.Init.Period=32-1;
	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK)
		Error_handler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t*)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY) != HAL_OK)
		Error_handler();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

void Error_handler(void)
{
	while(1);
}

