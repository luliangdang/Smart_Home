#ifndef __LED_H
#define __LED_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

struct led_message
{
		u8 light_stat;		//灯光状态
		int light_num;		//灯光数量
		int last_num;
		u8 ds1_stat;			//1号状态
		u8 ds2_stat;			//2号状态
		u8 ds3_stat;			//3号状态
		u8 ds4_stat;			//4号状态
};
extern struct led_message ds_ms;

//LED端口定义
#define LED0 PEout(14)	// LED1
#define LED1 PEout(15)	// LED2
#define DS1	 PAout(0)
#define DS2  PAout(1)
#define DS3  PAout(2)
#define DS4  PAout(3)

void LED_Init(void);//初始化
u8 get_led_num(void);
u8 led_contrl(void);

#endif
