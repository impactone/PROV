#include "sys.h"
#include "usart.h"	
#include "bsp.h"
#include "stm32f4xx_dma.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////
//?車豕?辰???∩迆??,?∫3?printfo‘那y,??2?D豕辰a????use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//㊣那℅??aD豕辰a米??∫3?o‘那y                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//?“辰?_sys_exit()辰?㊣邦?a那1車?～??¯?迆?㏒那?    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//???“辰?fputco‘那y 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//?-?﹞﹞⊿?赤,?㊣米?﹞⊿?赤赤那㊣?   
	USART3->DR = (u8) ch;      
	return ch;
}
#endif







































char RxDebugMsg[DEBUG_MSG_LENGTH];
char TxDebugMsg[DEBUG_MSG_LENGTH];

void uart_init(u32 bound1,u32 bound2){
   //GPIO???迆谷豕??
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_USART6,ENABLE);//那1?邦USART1o赤USART6那㊣?車
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3|RCC_APB1Periph_UART4|RCC_APB1Periph_UART5,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);   //?aDMA2
	
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//米豕∩yDMA?谷???? 
	

		
		
		
	/*3?那??‘DMA   USART6_RX*/  
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
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}//米豕∩yDMA?谷???? 

	/*3?那??‘DMA   USART6_TX*/  
	DMA_InitStructure.DMA_Channel               = DMA_Channel_5; 
  DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&(USART6->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)TxDebugMsg;
  DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize            = 0;   //﹞∩?y???芍㊣?㏒?2?豕?谷?米??赤??0
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

		
	//∩??迆1??車|辰y???∩車?車3谷?
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
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //赤?赤足?∩車?那?3?
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //谷?角-
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
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//℅?3∟?a8??那y?Y??那?
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//辰???赤㏒?1??
	USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D㏒?谷??
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T車2?t那y?Y芍¯????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//那?﹞⊿?㏒那?
  USART_Init(USART1, &USART_InitStructure); //3?那??‘∩??迆1
	USART_Init(USART2, &USART_InitStructure); //3?那??‘∩??迆2
	USART_Init(USART3, &USART_InitStructure); //3?那??‘∩??迆3
	USART_Init(UART4, &USART_InitStructure); //3?那??‘∩??迆4
	USART_Init(UART5, &USART_InitStructure); //3?那??‘∩??迆5
	USART_InitStructure.USART_BaudRate = bound2;
	USART_Init(USART6, &USART_InitStructure); //3?那??‘∩??迆6
	
  USART_Cmd(USART1, ENABLE);  //那1?邦∩??迆1 
	USART_Cmd(USART2, ENABLE);  //那1?邦∩??迆2
	USART_Cmd(USART3, ENABLE);  //那1?邦∩??迆3
	USART_Cmd(UART4, ENABLE);   //那1?邦∩??迆4
	USART_Cmd(UART5, ENABLE);   //那1?邦∩??迆5
	USART_Cmd(USART6, ENABLE);  //那1?邦∩??迆6
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(USART6, USART_FLAG_IDLE);
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a???車那??D??
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//?a???車那??D??
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//?a???車那??D??
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//?a???車那??D??
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//?a???車那??D??
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

/*﹞⊿?赤那y?Y?邦D?*/
u16 MsgUpdate(char *Data)
{
	char *Packgage = TxDebugMsg;
	u16 length = 0;
	while (*Data)
	{
		*Packgage++ = *Data++;
		length++;
	}
	return length;  /*﹞米??那y?Y3∟?豕*/
}

/*米豕∩yESP82663?那??‘赤那3谷*/
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

/*芍??車?芍???“米?﹞t???¯*/
/*protocol?谷???米  "TCP"  "UDP"*/
/*ip:   ﹞t???¯米?IP米??﹞*/
/*port: ﹞t???¯?a米????迆*/
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
	
	/*豕ㄓ米?米迆辰?∩??D米?米～足?米?D??⊿*/
	RecFrom8266 = 0;
	while (!RecFrom8266){}; 
	/*米豕∩y車|∩e*/
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
	      /*豕ㄓ米?米迆辰?∩??D米?米～足?米?D??⊿*/
	      RecFrom8266 = 0;
	      while (!RecFrom8266){}; 
			}
			RecFrom8266 = 0;
		}

	}
}

/*?a??ESP8266米?赤?∩?*/
void ESP8266_StartTrans(void)
{
	char CommStr[50] = "AT+CIPMODE=1\r\n";
	u16 Num;	
	
	Num = MsgUpdate(CommStr);
	DebugMsgSend(Num);
	
	
	/*豕ㄓ米?米迆辰?∩??D米?米～足?米?D??⊿*/
	RecFrom8266 = 0;
	while (!RecFrom8266){}; 
	/*米豕∩y車|∩e*/
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
		
	/*米豕∩y車|∩e*/
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

/*芍??車???“WIFI米?SSID*/
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
	
	/*米豕∩y車|∩e*/
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

/*?a??角∩℅?8266米?那y?Y?邦芍?*/
u8 ESP8266_Decode(void)
{
	return RxDebugMsg[0] - 48;
}

/*米¯那?D??⊿那?3?*/
void DebugMsgSend(u16 Num)
{
  if(DMA2_Stream6->NDTR)   
  {
      return;  /*DMA?y?迆﹞⊿?赤那y?Y*/
  }
	DMA_Cmd(DMA2_Stream6, DISABLE);                      
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}		
	DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6 | DMA_FLAG_HTIF3);	
	DMA_SetCurrDataCounter(DMA2_Stream6,Num);          
	DMA_Cmd(DMA2_Stream6, ENABLE);                      
}



//RecStatus車D豕y??
//     bit2               bit1                bit0
//那?﹞??車那?米?芍?0x0D   那?﹞??車那?米?芍?0x0A  ?a那??車那?㊣那??㏒?1㊣赤那??a那??車那?
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

    if (RecStatus & 0x01)  //那?﹞??a那??車那?
		{  
			if (RecStatus&0x02)  //那?﹞?辰??-?車那?米?芍?0x0A
			{
				if (Res == 0x0D)   //?車那?芍?0x0A??o車?迄?車那?米?0x0D㏒??米?¯?車那?赤那3谷辰???那y?Y～邦
				{
					RecFromUsart = 1;
					Manifold_CNT++;
					Num = 0;
					RecStatus = 0;
				}
				else
				{
					RecBuf[Num++] = Res; //?y3㏒?車那?那y?Y
					RecStatus = 0x01;    //??3y辰??車那?米?0x0A米?㊣那??
					if (Num >= 500)      //?車那?那y?Y3∟?豕1y3∟㏒??車那?∩赤?車
					{
					  RecFromUsart = 0;
					  Num = 0;
					  RecStatus = 0;						
					}
				}
			}
			else
			{
          RecBuf[Num++] = Res; //?y3㏒?車那?那y?Y
					if (Num >= 500)      //?車那?那y?Y3∟?豕1y3∟㏒??車那?∩赤?車
					{
					  RecFromUsart = 0;
					  Num = 0;
					  RecStatus = 0;						
					}
			}
		  if (Res == 0x0A) RecStatus |= 1<<1;  //℅?㊣??D???車那?	
		}
		else
		{
			if (Res == 0x0D) RecStatus |= 1<<2;  //℅?㊣??a那??車那?
			else
				if ((Res == 0x0A) && (RecStatus&0x04))
					RecStatus = 0x01; //?a那??車那?
		}
  } 

} 

void USART2_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART2);	//?芍豕??車那?米?米?那y?Y
		USART_RX_BUF2[USART_RX_STA2++] = Res;	   	
    UltraSonic1_CNT++;		
  } 

} 
	
void USART3_IRQHandler(void)                
{
	u8 Res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART3);	//?芍豕??車那?米?米?那y?Y
		USART_RX_BUF3[USART_RX_STA3++] = Res;	  
    UltraSonic2_CNT++;		
  } 

} 
	
void UART4_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(UART4);	//?芍豕??車那?米?米?那y?Y
		USART_RX_BUF4[USART_RX_STA4++] = Res;	
    UltraSonic3_CNT++;		
  } 

} 
	
void UART5_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(UART5);	//?芍豕??車那?米?米?那y?Y
		USART_RX_BUF5[USART_RX_STA5++] = Res;	
    UltraSonic4_CNT++;		
  } 

} 

/**
  * ∩??迆???D?D??
  * ?迆?車米?那y?Y辰?o車㏒??足2a?車??角∩米?辰???℅??迆?迆車D??車D那y?Y
  * 豕?1???車D㏒??辰IDLE??1㏒?2迆谷迆?D??
  * ?芍豕?SRo赤DR??∩??¯?谷??3yIDLE㊣那????
  */
void USART6_IRQHandler(void)                
{
  u8 Res;
	/*??3yIDLE??∩??¯*/
	Res = USART6->SR;
	Res = USART6->DR;
  
  /*?車那?????*/	
  RecFrom8266 = 1;   
	
	/*2?豕??芍那?﹞?,2?豕?赤那米～*/
  Res = DEBUG_MSG_LENGTH - DMA2_Stream1->NDTR;
	RxDebugMsg[Res] = '\0';
	
	/*????DMA*/
	DMA_Cmd(DMA2_Stream1, DISABLE);
  DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);
  while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);
  DMA_SetCurrDataCounter(DMA2_Stream1, DEBUG_MSG_LENGTH);
  DMA_Cmd(DMA2_Stream1, ENABLE);
	
} 
 



