#include "UltraSonic.h"
#include "usart.h"

u16 Dis_1 = 65535;
u16 Dis_2 = 65535;
u16 Dis_3 = 65535;
u16 Dis_4 = 65535;

u8 UltraSonicNum = UltraSonic_1;  //´Ó1ºÅ³¬Éù²¨¿ªÊ¼É¨

//³¬Éù²¨³õÊ¼»¯
void UltraSonic_Init(void)
{
	  USART_SendData(USART2,0X55);	
}

//³µÉí¾àÀëÉ¨Ãè
u8 ScanDis(void)
{
	
	switch (UltraSonicNum)
	{
		case UltraSonic_1:
			if (USART_RX_STA2 >= 2)
			{				
				Dis_1 = (USART_RX_BUF2[0]<<8)|USART_RX_BUF2[1];
				Dis_1 = Dis_1 / 10;
				USART_RX_STA2 = 0;
			}
			USART_SendData(USART3,0X55);
			UltraSonicNum = UltraSonic_2;
			break;
		case UltraSonic_2:
			if (USART_RX_STA3 >= 2)
			{				
				Dis_2 = (USART_RX_BUF3[0]<<8)|USART_RX_BUF3[1];
				Dis_2 = Dis_2 / 10;
				USART_RX_STA3 = 0;
			}
			USART_SendData(UART4,0X55);
			UltraSonicNum = UltraSonic_3;
			break; 
		case UltraSonic_3:
			if (USART_RX_STA4 >= 2)
			{				
				Dis_3 = (USART_RX_BUF4[0]<<8)|USART_RX_BUF4[1];	
        Dis_3 = Dis_3 / 10;				
				USART_RX_STA4 = 0;				
			}
      USART_SendData(UART5,0X55);
			UltraSonicNum = UltraSonic_4;
			break;
		case UltraSonic_4:
			if (USART_RX_STA5 >= 2)
			{				
				Dis_4 = (USART_RX_BUF5[0]<<8)|USART_RX_BUF5[1];
				Dis_4 = Dis_4 / 10;
				USART_RX_STA5 = 0;			
				return 1;
			}
			USART_SendData(USART2,0X55);
			UltraSonicNum = UltraSonic_1;
		default:
			break;
	}
	return 0;
	
}
