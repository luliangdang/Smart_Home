/************************************************************
 ** 项  目  名  称：基于物联网的智能寝室云端系统(智能灯部分)
 **
 ** 单 片 机 型 号：STM32F407VGT6
 **
 ** 文  件  名  称：esp8266.c
 **
 ** 创    建    人：全振赫
 **
 ** 描          述：智能灯系统wifi相关函数文件
 **
 ** 文件 创建 日期：2019年8月12日
 **
 ** 日志：
 ** Data							Author					Note
 @2019/8/12					  全振赫					创建本文件
************************************************************/
/*									Include Head File														*/
#include "esp8266.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "my_math.h"
#include "usart.h"
#include "timer.h"
#include "led.h"

/* wifi信息结构 */
struct esp_message wifi_netdev;

//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;

void Usart3_Init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;

	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

		USART_DeInit(USART3);  //复位串口3
	
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
		USART_InitStructure.USART_BaudRate = bound;//波特率 
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
		
		USART_Init(USART3, &USART_InitStructure); //初始化串口3
	 
		USART_Cmd(USART3, ENABLE);               //使能串口 
		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		TIM7_Int_Init(100-1,8400-1);		//10ms中断
		USART3_RX_STA=0;		//清零
		TIM_Cmd(TIM7, DISABLE); //关闭定时器7
}

//串口3,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART3_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
    for(j=0; j<i; j++)							//循环发送数据
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        USART_SendData(USART3,USART3_TX_BUF[j]);
    }
}


//串口3中断服务函数
void USART3_IRQHandler(void)
{
		u8 res;	    
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
		{
				res =USART_ReceiveData(USART3);
				if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
				{
						if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
						{
								TIM_SetCounter(TIM7,0);//计数器清空
								if(USART3_RX_STA==0)
										TIM_Cmd(TIM7, ENABLE);  //使能定时器7 
								USART3_RX_BUF[USART3_RX_STA++]=res;		//记录接收到的值	 
						}else 
						{
								USART3_RX_STA|=1<<15;					//强制标记接收完成
						}
				}
		}
}

//esp8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* esp_check_cmd(u8 *str)
{
		char *strx=0;
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
				USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
				strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
		}
		return (u8*)strx;
}

//向esp8266发送命令
//cmd:发送的命令字符串(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 esp_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
		u8 res=0; 
		USART3_RX_STA=0;
		u3_printf("%s\r\n",cmd);	//发送命令
		if(ack&&waittime)		//需要等待应答
		{
				while(--waittime)	//等待倒计时
				{
						delay_ms(10);
						if(USART3_RX_STA&0X8000)//接收到期待的应答结果
						{
								if(esp_check_cmd(ack))break;//得到有效数据 
								USART3_RX_STA=0;
						}
				}
				if(waittime==0)res=1; 
		}
		return res;
}

//清除接收器
//参数：无
//返回值 无
void cleanReceiveData(void)
{
    USART3_RX_STA=0;			//接收计数器清零
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
}

/* 连接服务器 */
u8 esp_tcp_connect(void)
{
		u8 ret = 0;
		char past[50];
		memset(past, 0, sizeof(past));
		sprintf((char *)past, "AT+CIPSTART=\"TCP\",\"%s\",%s", wifi_netdev.remote_ip, wifi_netdev.remote_port);
		ret = esp_send_cmd((u8 *)past, (u8 *)"OK", 1000);
		if (ret == 0)
		{
			 wifi_netdev.stat = WIFI_CONNECT;
		}
		else wifi_netdev.stat  = WIFI_CLOSED;
		return ret;
}

void netdev_set_info(void)
{
		char *ret;
		esp_send_cmd((u8 *)"AT+CIFSR", (u8 *)"+CIFSR", 500);		/* 查询ip地址 */
		while((USART3_RX_STA&0X8000)==0);
//		printf("%c\r\n", USART3_RX_BUF[25]);
//		printf("%c\r\n", USART3_RX_BUF[35]);
//		ret = strstr((char *)USART3_RX_BUF, "\"");
//		printf("%s\r\n", ret);
//		ret++;
//		ret = strstr((char *)ret, "\r");
//		printf("%s\r\n", ret);
		delay_ms(100);
		
		/*	
		wifi_netdev.ip = 
		
		*/
}

void esp_check_stat(void)
{
		if(esp_send_cmd((u8 *)"AT+CIPSTATUS", (u8 *)"OK", 500) == 1)
		{
				wifi_netdev.stat = WIFI_ERROR;
				printf("状态监测出错\r\n");
		}
		else
		{
				printf("得到模块状态\r\n");
				printf("%s\r\n", USART3_RX_BUF);
				switch(USART3_RX_BUF[22])
				{
						case '2':
								wifi_netdev.stat = WIFI_GOTIP;
								esp_tcp_connect();
						break;
						case '3':
								wifi_netdev.stat = WIFI_CONNECT;
						break;
						case '4':
								wifi_netdev.stat = WIFI_CLOSED;
								esp_tcp_connect();
						break;
						case '5':
								wifi_netdev.stat = WIFI_NOIP;
								esp_connect_ap(wifi_netdev.ssid, wifi_netdev.passwd);
								esp_tcp_connect();
						break;
						default:
								wifi_netdev.stat = WIFI_ERROR;
								Esp8266_Init();
						break;
				}
		}
		printf("wifi stat: %d\r\n", wifi_netdev.stat);
		delay_ms(100);
}

u8 esp_connect_ap(char *ssid, char *passwd)
{
		u8 ret = 0;
		char past[100];
		memset(past, 0, sizeof(past));
		sprintf((char *)past, "AT+CWJAP_DEF=\"%s\",\"%s\"",ssid,passwd);
		if(esp_send_cmd((u8 *)past, (u8 *)"OK",1000)==0)
    {
				ret = 0;
        printf("路由器连接成功\r\n");
				wifi_netdev.stat = WIFI_GOTIP;
				wifi_netdev.ssid = ssid;
				wifi_netdev.passwd = passwd;
    }
		else
		{
				ret = 1;
				printf("路由器连接失败\r\n");
				wifi_netdev.stat = WIFI_ERROR;
		}
		return ret;
}

/* CONNECT HEADER */
const char CONNECT_HEAD_PKG1[] = {0x10, 0x2A, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54};
const char CONNECT_HEAD_PKG2[] = {0x04, 0xC0, 0x00, 0x78, 0x00, 0x09};
const char CONNECT_HEAD_PKG3[] = {0x00, 0x06};
const char CONNECT_HEAD_PKG4[] = {0x00, 0x0B};

/* 鉴权信息连接信息 */
const char DEVICE_ID[] = "549670756";
const char PRODUCT_ID[] = "278596";
const char AUTH_INFO[] = "52180720134";
const char USER_ID[] = "63024";

u8 send_connect_pkgs(void)
{
		u16 i = 0;
		u8 ret = 0;
		u8 data_buf[100];
		
//		esp_send_cmd((u8 *)"AT+CIPMODE=1", (u8 *)"OK", 500);
		
		memset(data_buf, 0, sizeof(data_buf));
		for(int j=0; j<sizeof(CONNECT_HEAD_PKG1); )
		{
				data_buf[i++] = CONNECT_HEAD_PKG1[j++];
		}
		for(int j=0; j<sizeof(CONNECT_HEAD_PKG2); )
		{
				data_buf[i++] = CONNECT_HEAD_PKG2[j++];
		}
		for(int j=0; j<sizeof(DEVICE_ID)-1; )
		{
				data_buf[i++] = DEVICE_ID[j++];
		}
		for(int j=0; j<sizeof(CONNECT_HEAD_PKG3); )
		{
				data_buf[i++] = CONNECT_HEAD_PKG3[j++];
		}
		for(int j=0; j<sizeof(PRODUCT_ID)-1; )
		{
				data_buf[i++] = PRODUCT_ID[j++];
		}
		for(int j=0; j<sizeof(CONNECT_HEAD_PKG4); )
		{
				data_buf[i++] = CONNECT_HEAD_PKG4[j++];
		}
		for(int j=0; j<sizeof(AUTH_INFO)-1; )
		{
				data_buf[i++] = AUTH_INFO[j++];
		}
		ret = esp_send_cmd((u8 *)"AT+CIPSEND=46", (u8 *)">", 500);
		for(i = 0; i<44; i++)
		{
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
				USART_SendData(USART3, data_buf[i]);
				printf("%02x", data_buf[i]);
		}
		u3_printf("\r\n");
		return ret;
}

const char DATA_HEAD_PKG1[] = {0x30};
const char DATA_HEAD_PKG2[] = {0x24, 0x64, 0x70, 0x03};

u8 send_data(char *stream, char *data)
{
		int i = 0,j = 0;
		int n = 0;
		u8 ret = 0;
		u8 past[128];
		u8 data_buf[50];
		u8 stream_len = strlen(stream);
		u8 data_len = strlen(data);
		memset(data_buf, 0, sizeof(data_buf));
		printf("stream_len: %d\r\n", stream_len);
		printf("data_len: %d\r\n", data_len);
		
		if(wifi_netdev.stat != WIFI_CONNECT)
		{
				/* 网络丢失，发送失败 */
				LED1 = 1;
				delay_ms(10);
				LED1 = 0;
				delay_ms(10);
				ret = 2;
				return ret;
		}
		
		data_buf[0] = 0x30;
		data_buf[1] = stream_len + data_len + 13;
		data_buf[2] = 0x00;
		data_buf[3] = 0x03;
		for(i=0;i<4;i++)
		{
				data_buf[i+4] = DATA_HEAD_PKG2[i];
		}
		data_buf[8] = (stream_len + data_len + 5) / 255;
		data_buf[9] = (stream_len + data_len + 5) % 255;
		data_buf[10] = '{';
		data_buf[11] = '"';
		for(i=0;i<stream_len;i++)
		{
			 data_buf[12+i] = stream[i];
		}
		data_buf[12+i] = '"';
		data_buf[13+i] = ':';
		for(j=0;j<data_len;j++)
		{
			 data_buf[14+i+j] = data[j];
		}
		data_buf[14+i+j] = '}';
		for (n = 0;n<stream_len+data_len+15;n++)
		{
				printf("%02x", data_buf[n]);
		}
		printf("\r\n");
		memset(past, 0, sizeof(past));
		sprintf((char *)past, "AT+CIPSEND=%d", stream_len+data_len+15);
		ret = esp_send_cmd(past, (u8 *)">", 500);
		for(i = 0; i<stream_len+data_len+15; i++)
		{
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
				USART_SendData(USART3, data_buf[i]);
				printf("%02x", data_buf[i]);
		}
		u3_printf("\r\n");
		u3_printf("\r\n");
		LED1 = 1;
		delay_ms(10);
		LED1 = 0;
		delay_ms(10);
		LED1 = 1;
		delay_ms(10);
		LED1 = 0;
		delay_ms(10);
		delay_ms(100);
}

char SSID[] = {"Kiven"};		//路由器SSID
char password[] = {"asd123456"};	//路由器密码
char ipaddr[]= {"183.230.40.39"};//IP地址
char port[]= {"6002"};				//端口号

//ESP8266模块初始化
void Esp8266_Init(void)
{
    u8 i;
    u8 ret;
		char past[100];			//发送数据缓存
    Usart3_Init(115200);		//初始化串口3波特率为115200
		printf("开始初始化\r\n");
    cleanReceiveData();		//清空接收数据缓存
    esp_send_cmd((u8 *)"AT+RST", (u8 *)"ready",3000);
    delay_ms(2000);
    u3_printf("AT+CWMODE=1\r\n");
		wifi_netdev.stat = WIFI_INIT;
    delay_ms(1000);		//等待模块上电稳定
    
    printf("初始化成功\r\n");
		memset(past, 0, sizeof(past));
    sprintf((char *)past, "AT+CWJAP_DEF=\"%s\",\"%s\"",SSID,password);
    if(esp_send_cmd((u8 *)past, (u8 *)"OK",1000)==0)
    {
        printf("路由器连接成功\r\n");
				wifi_netdev.stat = WIFI_GOTIP;
    }
		else
		{
				ret = 1;
				printf("路由器连接失败\r\n");
				wifi_netdev.stat = WIFI_ERROR;
				goto __exit;
		}
		delay_ms(500);
    i = 0;
		memset(past, 0, sizeof(past));
		sprintf((char *)past, "AT+CWAUTOCONN=1");
		esp_send_cmd((u8 *)past, (u8 *)"OK", 500);
    if(esp_send_cmd((u8 *)"AT+CIPMUX=0", (u8 *)"OK",1000) ==0)		//设置单链接模式
    {
        ret = 0;
        printf("单链接模式设置成功\r\n");
    }
		else
		{
				ret = 1;
        printf("单链接模式设置失败\r\n");
				goto __exit;
		}
		ret = 0;
		memset(past, 0, sizeof(past));
		sprintf((char *)past, "AT+CIPSTART=\"TCP\",\"%s\",%s", ipaddr, port);
    while((esp_send_cmd((u8 *)past, (u8 *)"OK", 1000)) != 0)
    {
				printf("第%d次重连服务器...\n", i++);
        if(i>=10)
				{
						ret = 1;
						goto __exit;
				}
				delay_ms(500);
    }
__exit:
		if(ret == 0)
		{
				printf("连接服务器成功\r\n");
				wifi_netdev.remote_ip = ipaddr;
				wifi_netdev.remote_port = port;
				wifi_netdev.stat = WIFI_CONNECT;
				send_connect_pkgs();
				LED0 = 1;
		}
		else printf("连接服务器失败\r\n");
}


