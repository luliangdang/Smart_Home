/************************************************************
 ** 项  目  名  称：基于物联网的智能寝室云端系统(智能灯部分)
 **
 ** 单 片 机 型 号：STM32F407VGT6
 **
 ** 文  件  名  称：led.c
 **
 ** 创    建    人：全振赫
 **
 ** 描          述：智能灯系统led驱动相关函数文件
 **
 ** 文件 创建 日期：2019年8月12日
 **
 ** 日志：
 ** Data							Author					Note
 @2019/8/12					  全振赫					创建本文件
************************************************************/

#include "led.h"  


//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_8 | GPIO_Pin_9);//GPIOF9,F10设置高，灯灭

}





