#include "bsp.h"

//������       checked
//5110         cheked
//��ɫ������    checked
//������        checked
//���          checked
//���          checked
//CAN          checked 
//����         checked
//OLED         checked
int main(void)
{ 
   u8      KeyMode;                 //����ѡ���ģʽ
//	u16     SystemCounter = 0;       //ϵͳ��ʱ
//	s32     xExpect = 0,yExpect = 0; //��������ֵ
//	float   angExpect = 0;           //�Ƕ�����ֵ ������
//	float   X = 0,Y = 0;             //ȫ�ֶ�λ����	
//	RecStru MANIFOLD_Data[MANIFOLD_DATA_LENTH];        //��������  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	uart_init(9600,115200);	
//  delay_init(168);
	BSP_Init(&KeyMode);
 // OLED_Init();
  while(1)
	{	
		
			if(isOver)
			{				
				printf("red : %d,green: %d,blue:%d\r\n",red1,green1,blue1);
				isOver=0;
			}
			

		
	//	OLED_Print6x8Str(00, 20, 85, 8, (uint8_t *)"WIFI CONNECTED", INV_OFF, IS);
	//	OLED_Print6x8Str(00, 20, 85, 8, (uint8_t *)"WIFI CONNECTED", INV_OFF, IS);
//		/*״̬��ѯ*/
//		StatusCheck(MANIFOLD_Data,
//		            &angExpect,
//		            &X,&Y,
//		            &xExpect,&yExpect,
//								&KeyMode,
//		            SystemCounter);
//		
//		/*ʱ��Ƭ��ѯ*/
//		TimerCheck(X,Y,
//		           &KeyMode,
//		           &angExpect,
//		           &xExpect,&yExpect,
//		           &SystemCounter);
	}
}		      
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	/****************���Գ���***********************
	
//			OLED_PrintNum(Yaw*57.3f,"%.3f",25,42,37,8);
//			OLED_PrintNum(angExpect*57.3f,"%.1f",89,42,37,8);
//			OLED_PrintNum(xExpect,"%.1f",89,26,31,8);
//			OLED_PrintNum(yExpect,"%.1f",89,34,31,8);			
//			OLED_PrintNum(Get_Dis(),"%.1f",25,10,43,8);   
//			OLED_PrintNum(LaserRadar.ang,"%.3f",00,52,120,8);	

//	 OLED_PrintNum((*x),"%.1f",25,26,31,8);
//	 OLED_PrintNum((*y),"%.1f",25,34,31,8);

//	  OLED_Print6x8Str(00, 10, 25, 8, (uint8_t *)"Dis:", INV_OFF, IS);
//		OLED_Print6x8Str(00, 26, 25, 8, (uint8_t *)"XRe:", INV_OFF, IS);
//		OLED_Print6x8Str(00, 34, 25, 8, (uint8_t *)"YRe:", INV_OFF, IS);
//		OLED_Print6x8Str(00, 42, 25, 8, (uint8_t *)"aRe:", INV_OFF, IS);
//		OLED_Print6x8Str(64, 26, 25, 8, (uint8_t *)"xEx:", INV_OFF, IS);
//		OLED_Print6x8Str(64, 34, 25, 8, (uint8_t *)"yEx:", INV_OFF, IS);
//		OLED_Print6x8Str(64, 42, 25, 8, (uint8_t *)"aEx:", INV_OFF, IS);

		if (__10ms)  //10ms���̱ջ�
		{			
			BaseControl(-V1,-V2,-V3); 
			__10ms = 0;
		}
		if (__20ms)  //20ms������
		{
			if (ScanDis())
			{
				UltraSonicNum = UltraSonic_1;
			}
			__20ms = 0;
		}
		if (__30ms)  //30msλ�˱ջ�
		{
			Closed_Loop(angExpect,X,xExpect,Y,yExpect,&V1,&V2,&V3,KeyMode);
			__30ms = 0;
		}
		if (__100ms)  //100ms����Ƶ��
		{ 
			SystemCounter++;
//      Monitor_Task();
			
//			DebugMsgSend(sprintf(TxDebugMsg,
//			"Yaw = %.3f angExpect = %.1f xExpect = %d yExpect = %d Dis = %.1f X = %.1f Y = %.1f\r\n",
//			 Yaw*57.3f,angExpect*57.3f,xExpect,yExpect,Get_Dis(),X,Y));
    		
//			if (SystemCounter >= 50) KeyMode = 8;
//			StdAct(&X,&xExpect,&Y,&yExpect,&angExpect,SystemCounter);

			�����ջ�����
			if (SystemCounter >= 50) 
			{
				angExpect = 3.14f;
			}
			if (SystemCounter >= 100)
			{
				angExpect = 0;
				SystemCounter = 0;
			}
			

			__100ms = 0;
		}

		switch (Selector)
		{
			case KEY0_PRES:
				OLED_Print6x8Str(47, 0, 37, 8, (uint8_t *)"MODE 1", INV_ON, IS);		  
			  (*mode) = 1;
				break;
			case KEY1_PRES:
        OLED_Print6x8Str(47, 0, 37, 8, (uint8_t *)"MODE 2", INV_ON, IS);
			  (*mode) = 2;			  
				break;
			case KEY2_PRES:
        OLED_Print6x8Str(47, 0, 37, 8, (uint8_t *)"MODE 3", INV_ON, IS);
				(*mode) = 3;
			  break;
			default:
				break;
		}	
	
		RotatorControl1(RIGHT1);
		RotatorControl2(RIGHT1);
		RotatorControl(OPEN);
		WindMotor1(START);
		WindMotor2(START);
		Wind1 = 15;
		Wind2 = 15;
		printf("\r\n111\r\n");
		IMU_GetAngles(angles);
		RM3510Cmd(1000,1000,1000);
		printf("\r\n%d %d %d\r\n",baseMotor[0],baseMotor[1],baseMotor[2]);
		printf("\r\n%f %f %f\r\n",angles[0],angles[1],angles[2]);

		if (isOver)
		{
			printf("\r\n%d %d %d      %d %d %d\r\n",red1,blue1,green1,red2,blue2,green2);
		  isOver = 0;
		}
		
		if (__20ms)
		{
			if (ScanDis())
			{
				printf("\r\n%d %d %d %d\r\n",Dis_1,Dis_2,Dis_3,Dis_4);
				UltraSonicNum = UltraSonic_1;
			}
			__20ms = 0;
		}
		
		  DebugMsgSend(sprintf(TxDebugMsg,
			"X=%.1f Y =%.1f\r\n",
			MANIFOLD_Data[2].F/10.0f,MANIFOLD_Data[3].F/10.0f));
		***************************************/


