#ifndef __ESP_H
#define __ESP_H
#include "sys.h"
#include "delay.h"

#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1								//0,不接收;1,接收.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   											//接收数据状态

/* ONENET SERVER INFO */
#define	ONENET_SERVER	"183.230.10.39"
#define ONENET_PORT		6002

/* esp8266当前状态 */
#define WIFI_INIT			1<<0		//WiFi已初始化
#define WIFI_GOTIP		1<<1		//WiFi已连接AP，获得IP地址
#define WIFI_CONNECT	1<<2		//已建立TCP或UDP传输
#define WIFI_CLOSED		1<<3		//断开网络连接
#define WIFI_NOIP			1<<4		//断开AP连接
#define WIFI_ERROR		1<<5		//其他错误

struct esp_message
{
		u8 stat;						//esp8266模块状态
		char *ip;						//esp8266模块ip地址
		char *mac_addr;			//esp8266模块MAC地址
		
		char *ssid;					//esp8266模块接连AP
		char *passwd;
	
		int socket;					//esp8266模块sockect接口
		char *remote_ip;		//esp8266模块远端地址
		char *remote_port;	//esp8266模块远端端口
};

extern struct esp_message wifi_netdev;

void Usart3_Init(u32 bound);	//串口3初始化
void Esp8266_Init(void);			//esp8266初始化

extern s8 sendAT(char *sendStr,char *searchStr,u32 outTime);//发送AT指令函数
extern void cleanReceiveData(void);    //清除接收器数据
extern char * my_strstr(char *FirstAddr,char *searchStr);	//strstr函数

void u3_printf(char* fmt,...);		//串口2发送数据函数

u8 esp_connect_ap(char *ssid, char *passwd);		//连接AP
u8 esp_tcp_connect(void);

void netdev_set_info(void);		//设置网卡信息
void esp_check_stat(void);		//check wifi stat
u8 send_connect_pkgs(void);		//发送连接数据包
u8 send_data(char *stream, char *data);

#endif




