#include "sys.h"
#include "usart.h"	
#include "bsp.h"
#include "stm32f4xx_dma.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////
//?����?��???�䨲??,?��3?printfo����y,??2?D����a????use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//������??aD����a��??��3?o����y                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//?����?_sys_exit()��?����?a��1��?��??��?��?�꨺?    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//???����?fputco����y 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//?-?������?��,?����?����?��������?   
	USART3->DR = (u8) ch;      
	return ch;
}
#endif







































char RxDebugMsg[DEBUG_MSG_LENGTH];
char TxDebugMsg[DEBUG_MSG_LENGTH];

void uart_init(u32 bound1,u32 bound2){
   //GPIO???������??
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_USART6,ENABLE);//��1?��USART1o��USART6����?��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3|RCC_APB1Periph_UART4|RCC_APB1Periph_UART5,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);   //?aDMA2
	
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�̨���yDMA?��???? 
	

		
		
		
	/*3?��??��DMA   USART6_RX*/  
	DMA_InitStructure.DMA_Channel               = DMA_Channel_5; 
  DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&(USART6->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)RxDebugMsg;
  DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize            = DEBUG_MSG_LENGTH;
  DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority              = DMA_Priority_Medium;
  DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
	
	DMA_DeInit(DMA2_Stream6);
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}//�̨���yDMA?��???? 

	/*3?��??��DMA   USART6_TX*/  
	DMA_InitStructure.DMA_Channel               = DMA_Channel_5; 
  DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&(USART6->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)TxDebugMsg;
  DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize            = 0;   //����?y???����?��?2?��?��?��??��??0
  DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority              = DMA_Priority_Medium;
  DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream6, &DMA_InitStructure);
		
	USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);
  USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);

		
	//��??��1??��|��y???�䨮?��3��?
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); 
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); 
	
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); 
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);

	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_9 | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��?����?�䨮?��?3?
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��?��-
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
   
	USART_InitStructure.USART_BaudRate = bound1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��?3��?a8??��y?Y??��?
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//��???����?1??
	USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D��?��??
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��2?t��y?Y����????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//��?����?�꨺?
  USART_Init(USART1, &USART_InitStructure); //3?��??����??��1
	USART_Init(USART2, &USART_InitStructure); //3?��??����??��2
	USART_Init(USART3, &USART_InitStructure); //3?��??����??��3
	USART_Init(UART4, &USART_InitStructure); //3?��??����??��4
	USART_Init(UART5, &USART_InitStructure); //3?��??����??��5
	USART_InitStructure.USART_BaudRate = bound2;
	USART_Init(USART6, &USART_InitStructure); //3?��??����??��6
	
  USART_Cmd(USART1, ENABLE);  //��1?����??��1 
	USART_Cmd(USART2, ENABLE);  //��1?����??��2
	USART_Cmd(USART3, ENABLE);  //��1?����??��3
	USART_Cmd(UART4, ENABLE);   //��1?����??��4
	USART_Cmd(UART5, ENABLE);   //��1?����??��5
	USART_Cmd(USART6, ENABLE);  //��1?����??��6
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(USART6, USART_FLAG_IDLE);
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a???����??D??
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//?a???����??D??
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//?a???����??D??
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//?a???����??D??
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//?a???����??D??
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);//?a?????D?D??

	//Usart1 NVIC ????
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_Init(&NVIC_InitStructure);
	
}

/*����?����y?Y?��D?*/
u16 MsgUpdate(char *Data)
{
	char *Packgage = TxDebugMsg;
	u16 length = 0;
	while (*Data)
	{
		*Packgage++ = *Data++;
		length++;
	}
	return length;  /*����??��y?Y3��?��*/
}

/*�̨���yESP82663?��??������3��*/
void WaitForESP8266Ready(void)
{

	while (1)
	{
		if (RecFrom8266)
		{
			//if (strcmp(RxDebugMsg[],"invalid\r\n\0") == 0)
			if (RxDebugMsg[35] == 'i')
			{
				OLED_Print6x8Str(00, 20, 103, 8, (uint8_t *)"ESP8266 is Ready!", INV_OFF, IS);
				return;
			}
			else
			{
				OLED_Print6x8Str(00, 20, 109, 8, (uint8_t *)"Waiting ESP8266...", INV_OFF, IS);
			}
			RecFrom8266 = 0;
		}
	}
}

/*��??��?��???����?��t???��*/
/*protocol?��???��  "TCP"  "UDP"*/
/*ip:   ��t???�¦�?IP��??��*/
/*port: ��t???��?a��????��*/
void ESP8266_ConnectServer(char *protocol,char *ip,char *port)
{
	char CommStr[50] = "AT+CIPSTART=\"";
	u16 Num;	
	strcat(CommStr,protocol);
	strcat(CommStr,"\",\"");
	strcat(CommStr,ip);
	strcat(CommStr,"\",");
	strcat(CommStr,port);
	strcat(CommStr,"\r\n");
	
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);
	
	/*������?�̨���?��??D��?�̡㨬?��?D??��*/
	RecFrom8266 = 0;
	while (!RecFrom8266){}; 
	/*�̨���y��|��e*/
	RecFrom8266 = 0;
	while (1)
	{
		if (RecFrom8266)
		{
			if (strcmp(RxDebugMsg,"CONNECT\r\n\r\nOK\r\n") == 0)
			{
				OLED_Print6x8Str(00, 30, 97, 8, (uint8_t *)"Server is Ready!", INV_OFF, IS);
				return;
			}
			else 
			{
				OLED_Print6x8Str(00, 30, 85, 8, (uint8_t *)"Wait Server...", INV_OFF, IS);
				DebugMsgSend(Num);
	      /*������?�̨���?��??D��?�̡㨬?��?D??��*/
	      RecFrom8266 = 0;
	      while (!RecFrom8266){}; 
			}
			RecFrom8266 = 0;
		}

	}
}

/*?a??ESP8266��?��?��?*/
void ESP8266_StartTrans(void)
{
	char CommStr[50] = "AT+CIPMODE=1\r\n";
	u16 Num;	
	
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);
	
	
	/*������?�̨���?��??D��?�̡㨬?��?D??��*/
	RecFrom8266 = 0;
	while (!RecFrom8266){}; 
	/*�̨���y��|��e*/
	RecFrom8266 = 0;
	while (1)
	{
		if (RecFrom8266)
		{
			if (strcmp(RxDebugMsg,"\r\nOK\r\n") == 0)
			{
				OLED_Print6x8Str(00, 40, 91, 8, (uint8_t *)"Trans is Ready!", INV_OFF, IS);
				break;
			}
			else
			{
				OLED_Print6x8Str(00, 40, 79, 8, (uint8_t *)"Wait Trans...", INV_OFF, IS);
			}
			RecFrom8266 = 0;
		}
	}
	
	strcpy(CommStr,"AT+CIPSEND\r\n");
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);	
		
	/*�̨���y��|��e*/
	RecFrom8266 = 0;
	while (1)
	{
		if (RecFrom8266)
		{
			if (strcmp(RxDebugMsg,"AT+CIPSEND\r\r\n\r\nOK\r\n\r\n>") == 0)
			{
				OLED_Print6x8Str(00, 50, 85, 8, (uint8_t *)"Send is Ready!", INV_OFF, IS);
				return;
			}
			else
			{
				OLED_Print6x8Str(00, 50, 73, 8, (uint8_t *)"Wait Send...", INV_OFF, IS);
			}
			RecFrom8266 = 0;
		}
	}	
}

void ESP8266_StopTrans(void)
{
	char CommStr[50] = "+++";
	u16 Num;	
	
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);		
}

/*��??��???��WIFI��?SSID*/
void ESP8266_ConnectWIFI(char *ssid,char *password)
{  
	char CommStr[50] = "AT+CWJAP=\"";
	u16 Num;
	strcat(CommStr,ssid);
	strcat(CommStr,"\",\"");
	strcat(CommStr,password);
	strcat(CommStr,"\"");
	strcat(CommStr,"\r\n");
	
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);
	
	/*�̨���y��|��e*/
	RecFrom8266 = 0;
	while (1)
	{
		if (RecFrom8266)
		{
			if (strcmp(RxDebugMsg,"\r\nOK\r\n") == 0)
			{
				OLED_Print6x8Str(00, 20, 85, 8, (uint8_t *)"WIFI CONNECTED", INV_OFF, IS);
				return;
			}
			else 
			{
				OLED_Print6x8Str(00, 20, 79, 8, (uint8_t *)"Waiting IP...", INV_OFF, IS);
			}
			RecFrom8266 = 0;
		}
	}
	
}

/*?a??�����?8266��?��y?Y?����?*/
u8 ESP8266_Decode(void)
{
	return RxDebugMsg[0] - 48;
}

/*�̡¨�?D??�騺?3?*/
void DebugMsgSend(u16 Num)
{
  if(DMA2_Stream6->NDTR)   
  {
      return;  /*DMA?y?������?����y?Y*/
  }
	DMA_Cmd(DMA2_Stream6, DISABLE);                      
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}		
	DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6 | DMA_FLAG_HTIF3);	
	DMA_SetCurrDataCounter(DMA2_Stream6,Num);          
	DMA_Cmd(DMA2_Stream6, ENABLE);                      
}



//RecStatus��D��y??
//     bit2               bit1                bit0
//��?��??����?��?��?0x0D   ��?��??����?��?��?0x0A  ?a��??����?����??��?1������??a��??����?
u8 RecStatus = 0;
u8 RecFromUsart = 0;
u8 RecFrom8266 = 0;
char RecBuf[600];

u8 USART_RX_STA2 = 0;
u8  USART_RX_BUF2[5];
u8 USART_RX_STA3 = 0;
u8  USART_RX_BUF3[5];
u8 USART_RX_STA4 = 0;
u8  USART_RX_BUF4[5];
u8 USART_RX_STA5 = 0;
u8  USART_RX_BUF5[5];

void USART1_IRQHandler(void)                	
{
	static u16 Num = 0;
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART1);

    if (RecStatus & 0x01)  //��?��??a��??����?
		{  
			if (RecStatus&0x02)  //��?��?��??-?����?��?��?0x0A
			{
				if (Res == 0x0D)   //?����?��?0x0A??o��?��?����?��?0x0D��??��?��?����?����3����???��y?Y�㨹
				{
					RecFromUsart = 1;
					Manifold_CNT++;
					Num = 0;
					RecStatus = 0;
				}
				else
				{
					RecBuf[Num++] = Res; //?y3��?����?��y?Y
					RecStatus = 0x01;    //??3y��??����?��?0x0A��?����??
					if (Num >= 500)      //?����?��y?Y3��?��1y3���??����?�䨪?��
					{
					  RecFromUsart = 0;
					  Num = 0;
					  RecStatus = 0;						
					}
				}
			}
			else
			{
          RecBuf[Num++] = Res; //?y3��?����?��y?Y
					if (Num >= 500)      //?����?��y?Y3��?��1y3���??����?�䨪?��
					{
					  RecFromUsart = 0;
					  Num = 0;
					  RecStatus = 0;						
					}
			}
		  if (Res == 0x0A) RecStatus |= 1<<1;  //��?��??D???����?	
		}
		else
		{
			if (Res == 0x0D) RecStatus |= 1<<2;  //��?��??a��??����?
			else
				if ((Res == 0x0A) && (RecStatus&0x04))
					RecStatus = 0x01; //?a��??����?
		}
  } 

} 

void USART2_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART2);	//?����??����?��?��?��y?Y
		USART_RX_BUF2[USART_RX_STA2++] = Res;	   	
    UltraSonic1_CNT++;		
  } 

} 
	
void USART3_IRQHandler(void)                
{
	u8 Res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART3);	//?����??����?��?��?��y?Y
		USART_RX_BUF3[USART_RX_STA3++] = Res;	  
    UltraSonic2_CNT++;		
  } 

} 
	
void UART4_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(UART4);	//?����??����?��?��?��y?Y
		USART_RX_BUF4[USART_RX_STA4++] = Res;	
    UltraSonic3_CNT++;		
  } 

} 
	
void UART5_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(UART5);	//?����??����?��?��?��y?Y
		USART_RX_BUF5[USART_RX_STA5++] = Res;	
    UltraSonic4_CNT++;		
  } 

} 

/**
  * ��??��???D?D??
  * ?��?����?��y?Y��?o����??��2a?��??�����?��???��??��?����D??��D��y?Y
  * ��?1???��D��??��IDLE??1��?2������?D??
  * ?����?SRo��DR??��??��?��??3yIDLE����????
  */
void USART6_IRQHandler(void)                
{
  u8 Res;
	/*??3yIDLE??��??��*/
	Res = USART6->SR;
	Res = USART6->DR;
  
  /*?����?????*/	
  RecFrom8266 = 1;   
	
	/*2?��??����?��?,2?��?�����̡�*/
  Res = DEBUG_MSG_LENGTH - DMA2_Stream1->NDTR;
	RxDebugMsg[Res] = '\0';
	
	/*????DMA*/
	DMA_Cmd(DMA2_Stream1, DISABLE);
  DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);
  while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);
  DMA_SetCurrDataCounter(DMA2_Stream1, DEBUG_MSG_LENGTH);
  DMA_Cmd(DMA2_Stream1, ENABLE);
	
} 
 



