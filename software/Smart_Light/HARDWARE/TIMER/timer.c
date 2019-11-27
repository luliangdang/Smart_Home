/************************************************************
 ** 项  目  名  称：基于物联网的智能寝室云端系统(智能灯部分)
 **
 ** 单 片 机 型 号：STM32F407VGT6
 **
 ** 文  件  名  称：timer.c
 **
 ** 创    建    人：全振赫
 **
 ** 描          述：智能灯系统定时器相关函数文件
 **
 ** 文件 创建 日期：2019年8月12日
 **
 ** 日志：
 ** Data							Author					Note
 @2019/8/12					  全振赫					创建本文件
************************************************************/

#include "timer.h"
#include "led.h"
#include "usart.h"
#include "esp8266.h"

extern vu16 USART3_RX_STA;

//定时器7中断服务程序
void TIM7_IRQHandler(void)
{
		if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
		{
				USART3_RX_STA|=1<<15;	//标记接收完成
				TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
				TIM_Cmd(TIM7, DISABLE);  //关闭TIM7
//				printf("%s\n", USART3_RX_BUF);
		}
}

//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM7_Int_Init(u16 arr,u16 psc)
{	
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
		
		//定时器TIM7初始化
		TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断

				
		NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

u8 check_flag;
//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//是更新中断
		{
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIM3更新中断标志
				check_flag = 1;
		}
}

void TIM3_Init(u16 arr,u16 psc)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM7时钟使能    
		
		//定时器TIM7初始化
		TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断

				
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		TIM_Cmd(TIM3, ENABLE);
}
	 
