#include "bsp.h"

void BSP_Init(u8 *mode)
{
	  u8 Selector = 0;
	  char cYaw[10];
	  delay_init(168);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  uart_init(115200,115200);	
		
//		Mag_Init();
//	  KEY_Init();
//	  OLED_Init();
//    Task_Init();	
//	  Laser_Init();		  
		  Select_Init();	
//	  Monitor_Init();
//	  CAN_Mode_Init();
//		UltraSonic_Init();  
//	  TIM_Configuration();
// 	  posPID_Init_Struct();
//	  GyroMode(USE_MPU6050);        /*全局定位模块使用大疆单轴还是MPU6050*/
//	  PWM_Configuration(199,8399);
	
//    delay_ms(1000);         //等待陀螺仪和Robomodule稳定
//#if USE_ESP8266	== 1	
//    ESP8266_StopTrans();	
//	  delay_ms(500);
//    ESP8266_ConnectWIFI(SSID,PASSWORD);
//		ESP8266_ConnectServer("TCP","192.168.1.149","8080");
//		ESP8266_StartTrans();
//		OLED_Clear();
//		OLED_Print6x8Str(30, 44, 67, 8, (uint8_t *)"Designed By", INV_ON, IS);
//	  OLED_Print6x8Str(35, 54, 55, 8, (uint8_t *)"SmartFish", INV_ON, IS);
//#else		
//	  OLED_Print6x8Str(30, 44, 67, 8, (uint8_t *)"Designed By", INV_ON, IS);
//	  OLED_Print6x8Str(35, 54, 55, 8, (uint8_t *)"SmartFish", INV_ON, IS);		
//#endif

//	  Wind1 = 10;            
//	  Wind2 = 10;   /*风机初始化结束*/
//		RoboModule_Reset(0,0);
//		delay_ms(500);
//		RoboModule_Config(0x02A);
//		delay_ms(500);
//	  RoboModule_Mode(0,0,PWM_VELOCITY_POSITION_MODE);	

//    OLED_Clear();
//    OLED_Print6x8Str(23, 10, 85, 8, (uint8_t *)"System Inited!", INV_OFF, IS);
//		OLED_Print6x8Str(10, 20, 115, 8, (uint8_t *)"Plese Press Any Key", INV_OFF, IS);
//    OLED_Print6x8Str(28, 30, 121, 8, (uint8_t *)"To Continue!", INV_OFF, IS);
//		OLED_Print6x8Str(30, 44, 67, 8, (uint8_t *)"Designed By", INV_ON, IS);
//	  OLED_Print6x8Str(35, 54, 55, 8, (uint8_t *)"SmartFish", INV_ON, IS);
//		RecFrom8266 = 0;
//		while(!Selector)
//		{
//			sprintf(cYaw,"%.2f",Yaw * 57.3f);
//      OLED_Print6x8Str(00, 54, 31, 8, (uint8_t *)cYaw, INV_OFF, IS);			
//#if USE_ESP8266	== 1	
//			if (RecFrom8266)
//			{
//				Selector = RxDebugMsg[0] - 48;
//				*mode = Selector;
//				RecFrom8266 = 0;
//			}
//#else
//			Selector = KEY_Scan();
//			*mode = Selector;
//#endif
//		}
//		*mode = 1;
//		OLED_Clear();
//	  WindMotor1(START);
//	  WindMotor2(START);
//		SystemInitReady = 1;  //系统初始化完成
}

