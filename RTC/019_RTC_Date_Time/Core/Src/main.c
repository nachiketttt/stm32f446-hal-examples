/*
 * main.c
 *
 *  Created on: Mar 30, 2026
 *      Author: nachiket
 */
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "stm32f4xx_hal.h"

#define TRUE     1
#define FALSE    0

void UART2_Init(void);
void printmsg(char *format,...);
void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void Error_handler(void);
void RTC_Init(void);
void RTC_CalendarConfig(void);

UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;

int main(void)
{
	HAL_Init();

	GPIO_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	UART2_Init();

	printmsg("This is RTC calendar Test program\r\n");

	RTC_Init();

	RTC_CalendarConfig();

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

	// Add this before HAL_RCC_OscConfig in SystemClock_Config_HSE for small delay for hse to activate
	for(volatile uint32_t i = 0; i < 50000; i++); // let HSE stabilize

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
		Error_handler();

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

	if(HAL_RCC_ClockConfig(&clk_init, FLatecncy) != HAL_OK)
		Error_handler();

	//SYSTICK config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void printmsg(char *format,...)
{
	char str[80];

	//extract the argument list using VA apis
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
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
		Error_handler();
}

void RTC_Init(void)
{
	hrtc.Instance=RTC;
	hrtc.Init.HourFormat=RTC_HOURFORMAT_12;
	hrtc.Init.AsynchPrediv=0x7F;
	hrtc.Init.SynchPrediv=0xFF;
	hrtc.Init.OutPut=RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_LOW;
	hrtc.Init.OutPutType=RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&hrtc) != HAL_OK)
		Error_handler();
}

void RTC_CalendarConfig(void)
{
	//This function does RTC calendar config
	//Configure time as 11:45:10 AM and date as 12 March 2026 Tuesday
	RTC_TimeTypeDef RTC_TimeInit;
	RTC_DateTypeDef RTC_DateInit;

	RTC_TimeInit.Hours=1;
	RTC_TimeInit.Minutes=15;
	RTC_TimeInit.Seconds=10;
	RTC_TimeInit.TimeFormat=RTC_HOURFORMAT12_PM;

	if(HAL_RTC_SetTime(&hrtc, &RTC_TimeInit, RTC_FORMAT_BIN) != HAL_OK)
		Error_handler();

	RTC_DateInit.Date=31;
	RTC_DateInit.Month=RTC_MONTH_MARCH;
	RTC_DateInit.Year=26;
	RTC_DateInit.WeekDay=RTC_WEEKDAY_TUESDAY;

	if(HAL_RTC_SetDate(&hrtc, &RTC_DateInit, RTC_FORMAT_BIN) != HAL_OK)
		Error_handler();
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef ledgpio, buttongpio;

	ledgpio.Pin=GPIO_PIN_5;
	ledgpio.Mode=GPIO_MODE_INPUT;
	ledgpio.Pull=GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &ledgpio);

	buttongpio.Pin=GPIO_PIN_13;
	buttongpio.Mode=GPIO_MODE_IT_FALLING;
	buttongpio.Pull=GPIO_NOPULL;

	HAL_GPIO_Init(GPIOC, &buttongpio);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

char* getDayofweek(uint8_t number)
{
	char *weekday[]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};

	return weekday[number-1];
}

char* getTimeformat(uint8_t number)
{
	char *timeFormat[]={"AM","PM"};

	return timeFormat[number];
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	RTC_TimeTypeDef RTC_TimeRead;
	RTC_DateTypeDef RTC_DateRead;

	HAL_RTC_GetTime(&hrtc, &RTC_TimeRead, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTC_DateRead, RTC_FORMAT_BIN);

	printmsg("Current time is: %02d:%02d:%02d   <%s>   \r\n",RTC_TimeRead.Hours,RTC_TimeRead.Minutes,RTC_TimeRead.Seconds,getTimeformat(RTC_TimeRead.TimeFormat));
	printmsg("Current date is: %02d-%02d-%02d   <%s>   \r\n",RTC_DateRead.Date,RTC_DateRead.Month,RTC_DateRead.Year,getDayofweek(RTC_DateRead.WeekDay));
}

void Error_handler(void)
{
	while(1);
}


