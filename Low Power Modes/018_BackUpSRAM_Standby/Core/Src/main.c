/*
 * main.c
 *
 *  Created on: Mar 22, 2026
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
void Error_handler(void);
void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void printmsg(char *format,...);

UART_HandleTypeDef huart2;

int main(void)
{
	uint32_t *pBackupSRAMbse=0;

	char write_buf[]="Hello";

	HAL_Init();

	GPIO_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	UART2_Init();

	//1. Turn on the clock in RCC register for backup sram
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	//2. Enable write access to the backup domain
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	pBackupSRAMbse=(uint32_t*)BKPSRAM_BASE;

	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

		printmsg("Woke up from the Standby mode\r\n");
		uint8_t data=(uint8_t)*pBackupSRAMbse;
		if(data != 'H')
			printmsg("Backup SRAM data is lost\r\n");
		else
			printmsg("Backup SRAM data is safe\r\n");
	}
	else
	{
		for(uint32_t i=0;i<strlen(write_buf)+1;i++)
		{
			*(pBackupSRAMbse+i)=write_buf[i];
		}
	}

	printmsg("Press user button to enter Standby mode\r\n");
	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) != GPIO_PIN_RESET);
	//when user pushes button it comes out of the while loop

	printmsg("Going to Standby mode\r\n");

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	//enable backup voltage regulator
	HAL_PWREx_EnableBkUpReg();

	HAL_PWR_EnterSTANDBYMode();

	while(1)

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

void UART2_Init(void)
{
	huart2.Instance=USART2;
	huart2.Init.BaudRate=921600;
	huart2.Init.WordLength=UART_WORDLENGTH_8B;
	huart2.Init.StopBits=UART_STOPBITS_1;
	huart2.Init.Parity=UART_PARITY_NONE;
	huart2.Init.HwFlowCtl=UART_HWCONTROL_NONE;
	huart2.Init.Mode=UART_MODE_TX_RX;


	if(HAL_UART_Init(&huart2) != HAL_OK)
		Error_handler();
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

void GPIO_Init(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef buttongpio;

	buttongpio.Pin=GPIO_PIN_13;
	buttongpio.Mode=GPIO_MODE_INPUT;
	buttongpio.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &buttongpio);
}

void Error_handler(void)
{
	while(1);
}

