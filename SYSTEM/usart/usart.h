#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define DEBUG_MSG_LENGTH 512

typedef union 
{
	char  C[4];
	float F;
}RecStru;

extern char TxDebugMsg[DEBUG_MSG_LENGTH];
extern char RxDebugMsg[DEBUG_MSG_LENGTH];

extern u8 RecFrom8266;
extern u8 RecFromUsart;
extern char RecBuf[600];    

extern u8 USART_RX_STA2;
extern u8 USART_RX_STA3;
extern u8 USART_RX_STA4;
extern u8 USART_RX_STA5;    
extern u8  USART_RX_BUF2[5]; 
extern u8  USART_RX_BUF3[5]; 
extern u8  USART_RX_BUF4[5]; 
extern u8  USART_RX_BUF5[5]; 



u8 ESP8266_Decode(void);
u16 MsgUpdate(char *Data);
void DebugMsgSend(u16 Num);
void ESP8266_StopTrans(void);
void ESP8266_StartTrans(void);
void WaitForESP8266Ready(void);
void uart_init(u32 bound1,u32 bound2);
void ESP8266_ConnectWIFI(char *ssid,char *password);
void ESP8266_ConnectServer(char *protocol,char *ip,char *port);




#endif


