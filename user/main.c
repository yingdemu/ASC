//第一次失败：电机输出函数的PWM写成了uint

#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Encoder.h"
#include "Timer.h"
#include "PWM.h"
#include "Motor.h"
#include "string.h"      //如果没有在删一些东西的时候，删掉主体文件，就应该差不多
#include "key.h"

uint8_t KeyNum;

/*定义变量*/
float Target_Mode1, Actual_Mode1, Out_Mode1;			//目标值，实际值，输出值   
float Kp, Ki, Kd;				//比例项，积分项，微分项的权重   //p i d两个模式用的同一套参数
float Kp2, Ki2, Kd2;				//比例项，积分项，微分项的权重   //p i d两个模式用的同一套参数

float Error0_Mode1, Error1_Mode1, Error2_Mode1;		//本次误差，上次误差，上上次误差

float Target_Mode2, Actual_Mode2, Out_Mode2;			//目标值，实际值，输出值   
float Error0_Mode2, Error1_Mode2, Error2_Mode2;		//本次误差，上次误差，上上次误差


uint16_t Length;//用来计算串口接收长度
uint16_t i;//用来遍历赋值给Target
int16_t sign;//串口接收时决定speed正负
uint8_t Mode=0;//0----模式一，1----模式2
int main(){
	Timer_Init();
	OLED_Init();
	Motor_Init();
	Key_Init();
	Serial_Init();
	Encoder_Init();		
	
	//pid调参区，（有时间的话，可以做个按键调参）
	Kp=0.07;//太大超调(0.1),震荡多
	Ki=0.04;
	Kd=0.01;//加了反而超调了
	
	Kp2=0.1;//太大超调(0.1),震荡多
	Ki2=0.05;
	Kd2=0.05;//加了反而超调了       //没有重力因素和卡带因素是准的

	Target_Mode1=0;
	
while(1){
	
	KeyNum = Key_GetNum();	
	if(KeyNum ==1)
	{
		Mode++;
		if(Mode==2)
		{
			Mode=0;
		}
	}
	if(Mode==0)
	{
		OLED_ShowString(2,6,"Mode1"); 

		//模式1的功能
		if (Serial_RxFlag == 1)		//如果接收到数据包
		{	
			sign=1;
			Target_Mode1=0;
			Length=strlen(Serial_RxPacket);
			for(i=0;i<Length;i++)
			{
				if(Serial_RxPacket[i]=='-')
				{
					sign=-1;
				}else{
				Target_Mode1=(Target_Mode1*10+(Serial_RxPacket[i]-'0'));
			}
		}
			Target_Mode1*=sign;
			Serial_RxFlag=0;
		}
	

	printf("%f,%f,%f\r\n",Target_Mode1,Actual_Mode1,Out_Mode1);
}else{
		//模式2的功能

		OLED_ShowString(2,6,"Mode2"); 

	//Target为电机1的位置，Actual为电机2的位置，Out驱动电机2
	printf("%f,%f,%f\r\n",Target_Mode2,Actual_Mode2,Out_Mode2);

	

}
	

}

}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET)
	{
		Key_Tick();
		Count++;
		if(Count>=10)
		{
			Count=0;	
			if(Mode==0){
								
			Actual_Mode1=Encoder_Get_Motor1();			
						
			/*获取本次误差、上次误差和上上次误差*/
			Error2_Mode1 = Error1_Mode1;			//获取上上次误差
			Error1_Mode1 = Error0_Mode1;			//获取上次误差
			Error0_Mode1 = Target_Mode1 - Actual_Mode1;	//获取本次误差，目标值减实际值，即为误差值
			
			Out_Mode1 += Kp * (Error0_Mode1 - Error1_Mode1) + Ki * Error0_Mode1+ Kd * (Error0_Mode1 - 2 * Error1_Mode1 + Error2_Mode1);
			
			if (Out_Mode1 > 100) {Out_Mode1 = 100;}		
			if (Out_Mode1 < -100) {Out_Mode1 = -100;}	
			
			//给电机1驱动
			Motor_SetPWM_Motor1(Out_Mode1);
		}
		else{
				Target_Mode2+=Encoder_Get_Motor1();
				Actual_Mode2+=Encoder_Get_Motor2();			

				Error2_Mode2 = Error1_Mode2;			//获取上上次误差
				Error1_Mode2 = Error0_Mode2;			//获取上次误差
				Error0_Mode2 = Target_Mode2 - Actual_Mode2;	//获取本次误差，目标值减实际值，即为误差值
			
				Out_Mode2+= Kp2 * (Error0_Mode2 - Error1_Mode2) + Ki2 * Error0_Mode2+ Kd2 * (Error0_Mode2 - 2 * Error1_Mode2 + Error2_Mode2);

				if (Out_Mode2 > 100) {Out_Mode2 = 100;}		
				if (Out_Mode2 < -100) {Out_Mode2 = -100;}	
		
				Motor_SetPWM_Motor2(Out_Mode2);
		
		
		}
		
		
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);

	}
}
