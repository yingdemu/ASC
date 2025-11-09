#include "stm32f10x.h"                  // Device header
#include "PWM.h"


void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;    //Motor2 B14 B15
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	
	PWM_Init();												//初始化直流电机的底层PWM
}

void Motor_SetPWM_Motor1(int16_t PWM)
{
	if (PWM >= 0)							
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);	
		GPIO_SetBits(GPIOB, GPIO_Pin_13);	
		PWM_SetCompare3_Motor1(PWM);				
	}
	else									
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);	
		PWM_SetCompare3_Motor1(-PWM);				

	}
}

void Motor_SetPWM_Motor2(int16_t PWM)
{
	if (PWM >= 0)							
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);	
		GPIO_SetBits(GPIOB, GPIO_Pin_15);	
		PWM_SetCompare4_Motor2(PWM);				
	}
	else									
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_14);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_15);	
		PWM_SetCompare4_Motor2(-PWM);				
	}
}

