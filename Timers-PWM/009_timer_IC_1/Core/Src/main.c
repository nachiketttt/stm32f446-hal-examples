/*
 * main.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

#include <string.h>
#include <stdio.h>

#include "main.h"
#include "stm32f4xx_hal.h"

void Error_handler(void);

void SystemClock_Config_HSI(uint8_t clock_freq);
void SystemClock_Config_HSE(uint8_t clock_freq);
//void LSE_Configuration(void);
//void HSI_Configuration(void);
void Timer2_Init(void);
void TIMER6_Init(void);

void GPIO_Init(void);
void UART2_Init(void);

TIM_HandleTypeDef htimer6;
TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart2;

uint32_t input_captures[2]={0};
uint8_t count=1;
uint8_t is_capture_done=FALSE;

int main(void)
{
	uint32_t capture_difference=0;
	double timer2_cnt_freq=0;
	double timer2_cnt_res=0;
	double user_signal_time_period=0;
	double user_signal_freq=0;
	char user_msg[100];

	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	UART2_Init();

	Timer2_Init();
	TIMER6_Init();

	__HAL_RCC_BKPSRAM_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	HAL_TIM_Base_Start_IT(&htimer6);

	HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);

	while(1)
	{
		if(is_capture_done)
		{
			if(input_captures[1]>input_captures[0])
			{
				capture_difference=input_captures[1]-input_captures[0];
			}
			else
			{
				capture_difference=(0xFFFFFFFF-input_captures[0])+input_captures[1];
			}

		timer2_cnt_freq=((HAL_RCC_GetPCLK1Freq()*2)/(htimer2.Init.Prescaler+1));
		timer2_cnt_res=1/timer2_cnt_freq;
		user_signal_time_period=capture_difference*timer2_cnt_res;
		user_signal_freq=1/user_signal_time_period;

		//cnt_res is the time period of the timer2
		//and capture difference is the time period of input signal wrt to the timer2 clk at the rising edge

		sprintf(user_msg,"Frequency of the signal applied is %f Hz\r\n",user_signal_freq);
		HAL_UART_Transmit(&huart2, (uint8_t*)user_msg, strlen(user_msg), HAL_MAX_DELAY);

		is_capture_done=FALSE;
		}
	}
	return 0;
}

void SystemClock_Config_HSI(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t flash_latency=0;

	osc_init.OscillatorType=RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	osc_init.HSIState=RCC_HSI_ON;
	osc_init.LSEState=RCC_LSE_ON;
	osc_init.HSICalibrationValue=16;
	osc_init.PLL.PLLState=RCC_PLL_ON;
	osc_init.PLL.PLLSource=RCC_PLLSOURCE_HSI;

	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM=16;
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

		flash_latency=FLASH_LATENCY_1;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM=16;
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

		flash_latency=FLASH_LATENCY_2;
		break;
	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM=16;
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

		flash_latency=FLASH_LATENCY_3;
		break;
	}
	default:
		return;
	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
		Error_handler();

	if(HAL_RCC_ClockConfig(&clk_init, flash_latency) != HAL_OK)
		Error_handler();

	//SYSTICK config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
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

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
		Error_handler();

	//HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

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
	TIM_IC_InitTypeDef timer2IC_Config;

	htimer2.Instance=TIM2;
	htimer2.Init.CounterMode=TIM_COUNTERMODE_UP;
	htimer2.Init.Period=0xFFFFFFFF;
	htimer2.Init.Prescaler=1;
	if(HAL_TIM_IC_Init(&htimer2) != HAL_OK)
		Error_handler();

	timer2IC_Config.ICFilter=0;
	timer2IC_Config.ICPolarity=TIM_ICPOLARITY_RISING;
	timer2IC_Config.ICPrescaler=TIM_ICPSC_DIV1;
	timer2IC_Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
	if(HAL_TIM_IC_ConfigChannel(&htimer2, &timer2IC_Config, TIM_CHANNEL_1) != HAL_OK)
		Error_handler();
}

/*
void LSE_Configuration(void)
{
	//configuring the pin over which the clock is to be outputted
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);
}
*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	#if 1
	if(!is_capture_done)
	{
		if(count==1)
		{
			input_captures[0]=__HAL_TIM_GetCompare(htim, TIM_CHANNEL_1);
			count++;
		}
		else if(count==2)
		{
			input_captures[1]=__HAL_TIM_GetCompare(htim, TIM_CHANNEL_1);
			count=1;
			is_capture_done=TRUE;
		}
	}
	#endif
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

void TIMER6_Init(void)
{
	htimer6.Instance=TIM6;
	htimer6.Init.Prescaler=9;
	htimer6.Init.Period=50-1;
	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK)
		Error_handler();

	 /* cnt_clk =  ftimer   (freq of timer from the source)
	 *           ---------
	 *            psc+1
     *
     * arr(value to put in period)
     * arr = delay x cnt_clk
     *
     * timer counts from 0 so -1 is added in arr value
	 */
}


void Error_handler(void)
{
	while(1);
}

