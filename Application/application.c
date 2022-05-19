/*
 * application.c
 *
 *  Created on: Feb 28, 2021
 *      Author: PC
 */

#include "application.h"
#include "main.h"
#include "timer.h"
#include "mcu_communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------local------------------------------------------*/
/*----------------------------------varibles---------------------------------------*/
static Timer ledTimer;
static uint8_t cmdButton_1[] ="<FM>PUSH_BUTTON:1<END>\r\n";

/*----------------------------------prototypes function-----------------------------*/
static void eAPP_StartMCUReceive( void );
static void eAPP_UART_Transmit_IT(unsigned char str[],int len);
static void camera_cmd_callback(CMD_Type cmd, const CAMARG *args );

/*----------------------------------local-------------------------------------------*/
int16_t motor_step=0;
uint8_t motor_direction=0;
uint8_t motor_speed=0;
/*----------------------------------extern-----------------------------------------*/
/*----------------------------------varibles---------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/*----------------------------------prototypes function----------------------------*/

/*----------------------------------extern-----------------------------------------*/

void app_main()
{
	TMR_Init();
	TMR_Add( &ledTimer, ledTimer_Callback, TMR_RELOAD_YES );	// Обычный секундный беспонтовый светодиод
	TMR_Start( &ledTimer, 1000);

	CAMCMD_Init(camera_cmd_callback);
	eAPP_StartMCUReceive();
	while(1)
	{
		TMR_ExecuteCallbacks();
		CAMCMD_ProcessMessages();
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		//eAPP_UART_Transmit_IT(cmdButton_1,sizeof(cmdButton_1)-1);
		HAL_Delay(200);
		//HAL_UART_Transmit(&huart2, cmdButton_1,sizeof(cmdButton_1)-1,0xffff);
	}
}

void ledTimer_Callback(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

static void camera_cmd_callback(CMD_Type cmd, const CAMARG *args ){

	switch(cmd){

		case CMD_GET_STAT:
			eAPP_UART_Transmit_IT(cmdButton_1,sizeof(cmdButton_1)-1);
			break;
		case CMD_SET_STEP:
			motor_step = args->parametr.ValueParametr[0];
			HAL_Delay(100);
			break;
		case CMD_STOP_MOTOR:

			break;
		default:
			break;
	}
}

static uint8_t data;
void eAPP_StartMCUReceive( void ){
	HAL_UART_Receive_IT( &huart2, &data, 1 );
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart ==&huart2){
		MCU_HandleByte(data);
	}
	eAPP_StartMCUReceive();
}
void eAPP_UART_Transmit_IT(unsigned char str[],int len){
	HAL_UART_Transmit_IT(&huart2, str,len);
}
