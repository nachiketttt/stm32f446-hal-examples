/*
 * main.c
 *
 *  Created on: Mar 13, 2026
 *      Author: nachiket
 */

#include "main.h"
#include <string.h>
#include <stdio.h>

void Error_handler(void);

void SystemClock_Config_HSE(uint8_t clock_freq);

void Timer2_Init(void);

void GPIO_Init(void);
void UART2_Init(void);

TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart2;


uint32_t pulse1_val=25000;  //500hz
uint32_t pulse2_val=12500;  //1khz
uint32_t pulse3_val=6250;   //2khz
uint32_t pulse4_val=3125;   //4khz

int main(void)
{
	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	Timer2_Init();

	if(HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_1) != HAL_OK)
		Error_handler();

	if(HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_2) != HAL_OK)
		Error_handler();

	if(HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_3) != HAL_OK)
		Error_handler();

	if(HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_4) != HAL_OK)
		Error_handler();

	while(1);

	return 0;

}

void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t flash_latency=0;

	osc_init.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState=RCC_HSE_BYPASS;
	osc_init.LSEState=RCC_LSE_OFF;
	osc_init.HSIState=RCC_HSI_OFF;
	osc_init.PLL.PLLState=RCC_PLL_ON;
	osc_init.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM=4;
		osc_init.PLL.PLLN=50;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		flash_latency=1;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM=4;
		osc_init.PLL.PLLN=84;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		flash_latency=2;
		break;
	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM=4;
		osc_init.PLL.PLLN=120;
		osc_init.PLL.PLLP=2;
		osc_init.PLL.PLLQ=2;
		osc_init.PLL.PLLR=2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider=RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider=RCC_HCLK_DIV2;

		flash_latency=3;
		break;
	}
	default:
		return;
	}

	// Add this before HAL_RCC_OscConfig in SystemClock_Config_HSE for small delay for hse to activate
	for(volatile uint32_t i = 0; i < 50000; i++); // let HSE stabilize

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
		Error_handler();

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

	if(HAL_RCC_ClockConfig(&clk_init, flash_latency) != HAL_OK)
		Error_handler();

	//SYSTICK config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin=GPIO_PIN_5;
	ledgpio.Mode=GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}

void Timer2_Init(void)
{
	TIM_OC_InitTypeDef tim2OC_init;

	htimer2.Instance=TIM2;
	htimer2.Init.Period=0xFFFFFFFF;
	htimer2.Init.Prescaler=1;
	if(HAL_TIM_OC_Init(&htimer2) != HAL_OK)
		Error_handler();

	tim2OC_init.OCMode=TIM_OCMODE_TOGGLE;
	tim2OC_init.OCPolarity=TIM_OCPOLARITY_HIGH;

	tim2OC_init.Pulse=pulse1_val;
	if(HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_1) != HAL_OK)
		Error_handler();

	tim2OC_init.Pulse=pulse2_val;
	if(HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_2) != HAL_OK)
		Error_handler();

	tim2OC_init.Pulse=pulse3_val;
	if(HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_3) != HAL_OK)
		Error_handler();

	tim2OC_init.Pulse=pulse4_val;
	if(HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_4) != HAL_OK)
		Error_handler();


	/*
	 * calculating pulse for output compare (oc) toggle mode

		timer cnt clk freq = x hz (25Mhz)
		desired freq = y hz

		time period for one full cycle of desired freq = 1/y seconds

		and for 50% duty cycle (half cycle) = (1/y)/2 seconds (half of time period of desired freq)

		freq of 50% duty cycle = 2y hz

				     timer cnt clk freq            x hz
		PULSE =   ------------------------  = ---------------
				   freq of 50% duty cycle          2y hz
	 */

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

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	volatile uint32_t ccr_content;
	//tim2_ch1 toggling with freq = 500Hz
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		ccr_content=__HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr_content+pulse1_val);
	}

	//tim2_ch2 toggling with freq = 1kHz
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		ccr_content=__HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr_content+pulse2_val);
	}

	//tim2_ch3 toggling with freq = 2kHz
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		ccr_content=__HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, ccr_content+pulse3_val);
	}

	//tim2_ch4 toggling with freq = 4kHz
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		ccr_content=__HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, ccr_content+pulse4_val);
	}
}

void Error_handler(void)
{
	while(1);
}

