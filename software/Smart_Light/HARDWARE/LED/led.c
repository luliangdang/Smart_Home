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

struct led_message ds_ms;

//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_14 | GPIO_Pin_15);//GPIOF9,F10设置高，灯灭
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);//GPIOA0,A1,A2,A3设置高，灯灭
}

u8 get_led_num(void)
{
		u8 light_num = 0;
		ds_ms.ds1_stat = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0);
		ds_ms.ds2_stat = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
		ds_ms.ds3_stat = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2);
		ds_ms.ds4_stat = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3);
		
		light_num = ds_ms.light_num = ds_ms.ds1_stat+ds_ms.ds2_stat+ds_ms.ds3_stat+ds_ms.ds4_stat;
		
		return light_num;
}

u8 led_contrl(void)
{
	 if(ds_ms.light_stat == 0)
	 {
				DS1 = DS2 = DS3 = DS4 = 0;
	 }
	 else
	 {
			if(ds_ms.light_num>4)
					ds_ms.light_num = 4;
			else if(ds_ms.light_num<0)
					ds_ms.light_num = 0;
		  switch(ds_ms.light_num)
			{
				case 1:
					DS1 = 1;DS2 = DS3 = DS4 = 0;
				break;
				case 2:
					DS1 = DS2 = 1;DS3 = DS4 = 0;
				break;
				case 3:
					DS1 = DS2 = DS3 = 1;DS4 = 0;
				break;
				case 4:
					DS1 = DS2 = DS3 = DS4 = 1;
				break;
				default :
					DS1 = DS2 = DS3 = DS4 = 0;
				break;
			}
	 }
}





