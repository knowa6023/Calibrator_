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
static void Stepper_motor(int8_t Direction,uint32_t Steps);

/*----------------------------------local-------------------------------------------*/
volatile int32_t motor_step=0;
volatile int32_t motor_direction=0;
uint8_t motor_speed=0;
uint32_t timer_step_counter=0;
/*----------------------------------extern-----------------------------------------*/
/*----------------------------------varibles---------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
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
TIM1->ARR=60;
		//HAL_UART_Transmit(&huart2, cmdButton_1,sizeof(cmdButton_1)-1,0xffff);
	}
}

void ledTimer_Callback(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
//
static void camera_cmd_callback(CMD_Type cmd, const CAMARG *args ){

	switch(cmd){

		case CMD_GET_STAT:
			eAPP_UART_Transmit_IT(cmdButton_1,sizeof(cmdButton_1)-1);
			break;
		case CMD_SET_STEP:
			motor_step = args->parameter32.par_int32;
			if (motor_step>0) Stepper_motor(1, motor_step);
			if (motor_step<0) Stepper_motor(-1, -motor_step);
			if (motor_step==0)
			{
	        	HAL_TIM_Base_Stop_IT(&htim1);
				timer_step_counter=0;
				HAL_GPIO_WritePin(ENABLE_MOTOR_GPIO_Port, ENABLE_MOTOR_Pin, SET);
			}
			break;
		case CMD_STOP_MOTOR:
        	HAL_TIM_Base_Stop_IT(&htim1);
			timer_step_counter=0;
			HAL_GPIO_WritePin(ENABLE_MOTOR_GPIO_Port, ENABLE_MOTOR_Pin, SET);
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

void Stepper_motor(int8_t Direction,uint32_t Steps)
{
	timer_step_counter = 2*Steps-1;
	switch (Direction) {
			case 1:
				HAL_GPIO_WritePin(DIRECTION_MOTOR_GPIO_Port, DIRECTION_MOTOR_Pin, SET);
				break;
			default:
				HAL_GPIO_WritePin(DIRECTION_MOTOR_GPIO_Port, DIRECTION_MOTOR_Pin, RESET);
				break;
		}
	HAL_GPIO_WritePin(ENABLE_MOTOR_GPIO_Port, ENABLE_MOTOR_Pin, RESET);
	HAL_TIM_Base_Start_IT(&htim1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        if(htim->Instance == TIM1) //check if the interrupt comes from TIM1
        {
        	// степ мотор выполгятетс здесь
        	if(timer_step_counter>0)
        	{
        		HAL_GPIO_TogglePin(STEP_MOTOR_GPIO_Port, STEP_MOTOR_Pin);
        		timer_step_counter--;
        		return;
        	}
        	HAL_GPIO_WritePin(STEP_MOTOR_GPIO_Port, STEP_MOTOR_Pin, RESET);
        	HAL_GPIO_WritePin(ENABLE_MOTOR_GPIO_Port, ENABLE_MOTOR_Pin, RESET);
        	HAL_TIM_Base_Stop_IT(&htim1);
        }
}


