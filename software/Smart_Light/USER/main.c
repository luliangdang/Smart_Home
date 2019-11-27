#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "adc.h"
#include "esp8266.h"
#include "key.h"
#include "exti.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

extern u8 check_flag;

int main(void)
{
		u8 key;
		u8 buf[50];
		u8 light_num = 0; 
		float temp = 0;
		u16 counter = 0;
		u32 refresh_counter = 0;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
		delay_init(168);  //初始化延时函数
		uart_init(115200);//初始化串口波特率为115200
		LED_Init();
		Adc_Init();
		KEY_Init();
		EXTIX_Init();
		Esp8266_Init();
		delay_ms(500);
		while(1)
		{
				counter++;
				refresh_counter++;
				if(USART3_RX_STA&0X8000)
				{
						USART3_RX_STA = 0;
						printf("%s", USART3_RX_BUF);
						
						printf("get data\r\n");
						printf("%s\r\n", USART3_RX_BUF);
						if(strstr((const char *)USART3_RX_BUF, (const char*)"+IPD,47"))
						{
								ds_ms.light_stat = 1;
								ds_ms.light_num = 1;
								led_contrl();
								esp_tcp_connect();
						}
						else if(strstr((const char *)USART3_RX_BUF, (const char*)"+IPD,48"))
						{
								ds_ms.light_stat = 1;
								ds_ms.light_num = 2;
								led_contrl();
								esp_tcp_connect();
						}
						else if(strstr((const char *)USART3_RX_BUF, (const char*)"+IPD,49"))
						{
								ds_ms.light_stat = 1;
								ds_ms.light_num = 3;
								led_contrl();
								esp_tcp_connect();
						}
						else if(strstr((const char *)USART3_RX_BUF, (const char*)"+IPD,50"))
						{
								ds_ms.light_stat = 1;
								ds_ms.light_num = 4;
								led_contrl();
								esp_tcp_connect();
						}
						else if(strstr((const char *)USART3_RX_BUF, (const char*)"+IPD,51"))
						{
								ds_ms.light_stat = 0;
								led_contrl();
								esp_tcp_connect();
						}
						else if(strstr((const char*)USART3_RX_BUF, (const char*)"ERROR") )
						{
								check_flag = 1;
						}
						memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
				}
				if(USART_RX_STA&0X8000)
				{
						USART_RX_STA = 0;
						u3_printf("%s\r\n", USART_RX_BUF);
				}
				if(check_flag ==1)
				{
						check_flag = 0;
						esp_check_stat();
						netdev_set_info();
				}
				if(ds_ms.last_num != ds_ms.light_num)
				{
						light_num = get_led_num();
						memset(buf, 0, sizeof(buf));
						sprintf((char *)buf, "%d", light_num);
						send_data("light_num", (char *)buf);
						ds_ms.last_num = ds_ms.light_num;
				}
				
				if(counter%50==0)
				{
						counter = 0;
						temp = Get_Temprate()/100.0;
						
						memset(buf, 0, sizeof(buf));
						sprintf((char *)buf, "%.1f", temp);
//						printf("temp: %s\r\n", buf);
						send_data("temp", (char *)buf);
						light_num = get_led_num();
						memset(buf, 0, sizeof(buf));
						sprintf((char *)buf, "%d", light_num);
						send_data("light_num", (char *)buf);
						ds_ms.last_num = ds_ms.light_num;
						delay_ms(100);
				}
				if(refresh_counter%100==0)
				{
						check_flag = 1;
				}
				delay_ms(100);
		}
}
