/*
 * main.c
 *
 *  Created on: Mar 6, 2026
 *      Author: nachiket
 */

//PA12 --> CAN1_TX
//PA11 --> CAN1_RX

#include <main.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"

#define TRUE     1
#define FALSE    0

void UART2_Init(void);
void Error_handler(void);
void SystemClock_Config_HSE(uint8_t clock_freq);
void CAN1_Init(void);
void CAN1_TX(void);
void CAN_Filter_Config(void);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;

int main(void)
{
	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	UART2_Init();

	CAN1_Init();

	CAN_Filter_Config();

	if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK)
		Error_handler();

	if(HAL_CAN_Start(&hcan1) != HAL_OK)
		Error_handler();

	CAN1_TX();

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

void CAN1_Init(void)
{
	hcan1.Instance=CAN1;
	hcan1.Init.Mode=CAN_MODE_LOOPBACK;
	hcan1.Init.AutoBusOff=ENABLE;
	hcan1.Init.AutoRetransmission=ENABLE;
	hcan1.Init.AutoWakeUp=DISABLE;
	hcan1.Init.ReceiveFifoLocked=DISABLE;
	hcan1.Init.TimeTriggeredMode=DISABLE;
	hcan1.Init.TransmitFifoPriority=DISABLE;

	//Settings related to CAN bit timings
	hcan1.Init.Prescaler=5;
	hcan1.Init.SyncJumpWidth=CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1=CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2=CAN_BS2_1TQ;

	if(HAL_CAN_Init(&hcan1) != HAL_OK)
		Error_handler();

	/*
	 * APB1 CLK (PCLK1) = 25Mhz
	 * CAN prescaler = 1
	 * Duration of 1 time quanta (1TQ) = PCLK1/CAN_prescaler
	 *                                 = 0.04 micro seconds
	 */
}

void CAN1_TX(void)
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;
	uint8_t our_message[5]={'H','E','L','L','O'};

	TxHeader.DLC=5;
	TxHeader.StdId=0x65D;
	TxHeader.IDE=CAN_ID_STD;
	TxHeader.RTR=CAN_RTR_DATA;

	if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, our_message, &TxMailbox) != HAL_OK)
		Error_handler();
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

/*
void CAN1_RX(void)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t rcvd_msg[5];

	char msg[50];

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, rcvd_msg) != HAL_OK)
		Error_handler();

	sprintf(msg,"Message Received:%s\r\n",rcvd_msg);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}
*/

void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter_init;

	can1_filter_init.FilterActivation=ENABLE;
	can1_filter_init.FilterBank=0;
	can1_filter_init.FilterFIFOAssignment=CAN_RX_FIFO0;
	can1_filter_init.FilterIdHigh=0x0000;
	can1_filter_init.FilterIdLow=0x0000;
	can1_filter_init.FilterMaskIdHigh=0x0000;
	can1_filter_init.FilterMaskIdLow=0x0000;
	can1_filter_init.FilterMode=CAN_FILTERMODE_IDMASK;
	can1_filter_init.FilterScale=CAN_FILTERSCALE_32BIT;

	if(HAL_CAN_ConfigFilter(&hcan1, &can1_filter_init) != HAL_OK)
		Error_handler();
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50];
	sprintf(msg,"Message Transmitted:M0\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50];
	sprintf(msg,"Message Transmitted:M1\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50];
	sprintf(msg,"Message Transmitted:M2\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t rcvd_msg[5];

	char msg[50];

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, rcvd_msg) != HAL_OK)
		Error_handler();

	sprintf(msg,"Message Received:%s\r\n",rcvd_msg);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50];
	sprintf(msg,"CAN Error Detected\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void Error_handler(void)
{
	while(1);
}

