#include "can.h"
#include "usart.h"
#include "attitude.h"
#include "math.h"
#include "oled.h"
#include "Config.h"
#include "laser.h"
#include "monitor.h"

s16 baseMotor[4];       //接收来自底盘电机速度
RadarStruc LaserRadar;
float Yaw = 0;
float Pitch = 0;
float Roll = 0;

union Rec angYaw;
union Rec angPitch;
union Rec angRoll;
union Rec Coordinate_X;
union Rec Coordinate_Y;
union Rec Cali_X;
union Rec Cali_Y;
u8 RecFromCAN = 0;

//CAN1--与底盘电机通信
//CAN2--与正交编码器通信
u8 CAN_Mode_Init(void)
{ 
     CAN_InitTypeDef        CAN;
     CAN_FilterInitTypeDef  CAN_FILTER;
     GPIO_InitTypeDef       GPIO;
     NVIC_InitTypeDef       NVICType;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

     GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);	
     GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);
     GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2); 

	  GPIO.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
     GPIO.GPIO_Mode = GPIO_Mode_AF;
     GPIO_Init(GPIOA, &GPIO);
	
     GPIO.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_12 ;
     GPIO.GPIO_Mode = GPIO_Mode_AF;
     GPIO_Init(GPIOB, &GPIO);

     NVICType.NVIC_IRQChannel = CAN1_RX0_IRQn;
     NVICType.NVIC_IRQChannelPreemptionPriority = 1;
     NVICType.NVIC_IRQChannelSubPriority = 1;
     NVICType.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVICType);
     
	   NVICType.NVIC_IRQChannel = CAN1_TX_IRQn;
     NVICType.NVIC_IRQChannelPreemptionPriority = 0;
     NVICType.NVIC_IRQChannelSubPriority = 0;
     NVICType.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVICType);  

     NVICType.NVIC_IRQChannel = CAN2_RX0_IRQn;
     NVICType.NVIC_IRQChannelPreemptionPriority = 2;
     NVICType.NVIC_IRQChannelSubPriority = 2;
     NVICType.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVICType);
     
	   NVICType.NVIC_IRQChannel = CAN2_TX_IRQn;
     NVICType.NVIC_IRQChannelPreemptionPriority = 2;
     NVICType.NVIC_IRQChannelSubPriority = 2;
     NVICType.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVICType);  
		
		 CAN_DeInit(CAN1);
     CAN_DeInit(CAN2);
     CAN_StructInit(&CAN);

     CAN.CAN_TTCM = DISABLE;
     CAN.CAN_ABOM = DISABLE;    
     CAN.CAN_AWUM = DISABLE;    
     CAN.CAN_NART = DISABLE;    
     CAN.CAN_RFLM = DISABLE;    
     CAN.CAN_TXFP = ENABLE;     
     CAN.CAN_Mode = CAN_Mode_Normal; 
     CAN.CAN_SJW  = CAN_SJW_1tq;
     CAN.CAN_BS1 = CAN_BS1_9tq;
     CAN.CAN_BS2 = CAN_BS2_4tq;
     CAN.CAN_Prescaler = 3;
		 CAN_Init(CAN1, &CAN);
     CAN_Init(CAN2, &CAN);
     
     CAN_FILTER.CAN_FilterNumber = 14;
     CAN_FILTER.CAN_FilterMode   = CAN_FilterMode_IdMask;
     CAN_FILTER.CAN_FilterScale  = CAN_FilterScale_32bit;
     CAN_FILTER.CAN_FilterIdHigh = 0x0000;
     CAN_FILTER.CAN_FilterIdLow  = 0x0000;
     CAN_FILTER.CAN_FilterMaskIdHigh = 0x0000;
     CAN_FILTER.CAN_FilterMaskIdLow  = 0x0000;
     CAN_FILTER.CAN_FilterFIFOAssignment = 0;
     CAN_FILTER.CAN_FilterActivation=ENABLE;
     CAN_FilterInit(&CAN_FILTER);
		 
     CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
		 CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);
		 
		 CAN_FILTER.CAN_FilterNumber = 0;
		 CAN_FilterInit(&CAN_FILTER);
		 CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
		 CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
		 
	   return 0;
}   
 
static unsigned char can1_tx_success_flag = 0;
void CAN1_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME) != RESET) 
	  {
	     CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
       can1_tx_success_flag = 1;
    }
}

static unsigned char can2_tx_success_flag = 0;
void CAN2_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2,CAN_IT_TME) != RESET) 
	  {
	     CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
       can2_tx_success_flag = 1;
    }
}

void CAN1_RX0_IRQHandler(void)
 {
     CanRxMsg canRxMsg;
     if (CAN_GetITStatus(CAN1,CAN_IT_FMP0) != RESET) 
     {         
         CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
         CAN_Receive(CAN1, CAN_FIFO0, &canRxMsg);		 
				 
         switch(canRxMsg.StdId)
         {
						 case 0x201:  /*右后轮*/
						   baseMotor[0] = ((s16)canRxMsg.Data[2]<<8)|canRxMsg.Data[3];
							 Motor0_CNT++;
							 break;
						 case 0x202:  /*前轮*/
						   baseMotor[1] = ((s16)canRxMsg.Data[2]<<8)|canRxMsg.Data[3];
						   Motor1_CNT++;
							 break;
						 case 0x203:  /*左后轮*/
						   baseMotor[2] = ((s16)canRxMsg.Data[2]<<8)|canRxMsg.Data[3];
						   Motor2_CNT++;
							 break;
//						 case 0x204:  /*并没有这个轮子*/
//						   baseMotor[3] = ((s16)canRxMsg.Data[2]<<8)|canRxMsg.Data[3];
//							 break;	
						 case 0x02B:  /*RoboModule反馈的位置信息*/
							 LaserRadar.ang = ((canRxMsg.Data[4]<<24)|(canRxMsg.Data[5]<<16)|(canRxMsg.Data[6]<<8)|(canRxMsg.Data[7]));
						   RoboModule_CNT++;
							 break;
             default:
							 break;
         }           		 
     }
 }

void CAN2_RX0_IRQHandler(void)
 {
     CanRxMsg canRxMsg;
     if (CAN_GetITStatus(CAN2,CAN_IT_FMP0) != RESET) 
     {
         CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
         CAN_Receive(CAN2, CAN_FIFO0, &canRxMsg); 
			   switch (canRxMsg.StdId)
				 {
					 case 0x001:  /*全局坐标*/
						 Coordinate_X.RecChar[0] = canRxMsg.Data[0];
						 Coordinate_X.RecChar[1] = canRxMsg.Data[1];
						 Coordinate_X.RecChar[2] = canRxMsg.Data[2];
						 Coordinate_X.RecChar[3] = canRxMsg.Data[3];
						 Coordinate_Y.RecChar[0] = canRxMsg.Data[4];
						 Coordinate_Y.RecChar[1] = canRxMsg.Data[5];
						 Coordinate_Y.RecChar[2] = canRxMsg.Data[6];
						 Coordinate_Y.RecChar[3] = canRxMsg.Data[7];
						 RecFromCAN = 1;						
             EncoderXY_CNT++;					 
						 break;
#if USE_MPU6050 == 1
					 case 0x002:  /*MPU6050_YAW*/
						 angYaw.RecChar[0] = canRxMsg.Data[0];
						 angYaw.RecChar[1] = canRxMsg.Data[1];
						 angYaw.RecChar[2] = canRxMsg.Data[2];
						 angYaw.RecChar[3] = canRxMsg.Data[3];
						 Yaw = angYaw.RecFloat;
					   EncoderYaw_CNT++;
						 break;
#else
					 case 0x401:  /*DjiGyro_Yaw*/
						 Yaw = 0.0001745f*((int32_t)(canRxMsg.Data[0]<<24)|(int32_t)(canRxMsg.Data[1]<<16) | (int32_t)(canRxMsg.Data[2]<<8) | (int32_t)(canRxMsg.Data[3]));
					   DjiGyro_CNT++;
						 break;
#endif
					 case 0x005:  /*MPU6050_Pitch_Roll*/
						 angRoll.RecChar[0] = canRxMsg.Data[0];
					   angRoll.RecChar[1] = canRxMsg.Data[1];
					   angRoll.RecChar[2] = canRxMsg.Data[2];
					   angRoll.RecChar[3] = canRxMsg.Data[3];					   
					   angPitch.RecChar[0] = canRxMsg.Data[4];
					   angPitch.RecChar[1] = canRxMsg.Data[5];
					   angPitch.RecChar[2] = canRxMsg.Data[6];
					   angPitch.RecChar[3] = canRxMsg.Data[7];
					   Roll = angRoll.RecFloat;
					   Pitch = angPitch.RecFloat;
					   EncoderAng_CNT++;
					   break;
				   default:
						 break;
				 }

     }
 }
 
//RoboModule复位
void RoboModule_Reset(unsigned char Group,unsigned char Number)
{
    unsigned short can_id = 0x000;
    CanTxMsg tx_message;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8

		can_id |= Group<<8;
    can_id |= Number<<4;
    
    tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID   
    tx_message.Data[0] = 0x55;
    tx_message.Data[1] = 0x55;
    tx_message.Data[2] = 0x55;
    tx_message.Data[3] = 0x55;
    tx_message.Data[4] = 0x55;
    tx_message.Data[5] = 0x55;
    tx_message.Data[6] = 0x55;
    tx_message.Data[7] = 0x55;
    
    can1_tx_success_flag = 0;
    CAN_Transmit(CAN1,&tx_message);
    while(can1_tx_success_flag == 0); //如果CAN芯片是TJA1050，注释掉这个判断。
}
/****************************************************************************************
                                     模式选择指令
mode的取值范围如下：
PWM_MODE
PWM_CURRENT_MODE
PWM_VELOCITY_MODE
PWM_POSITION_MODE
PWM_VELOCITY_POSITION_MODE
CURRENT_VELOCITY_MODE
CURRENT_POSITION_MODE
CURRENT_VELOCITY_POSITION_MODE
*****************************************************************************************/
void RoboModule_Mode(unsigned char Group,unsigned char Number,unsigned char Mode)
{
    unsigned short can_id = 0x001;
    CanTxMsg tx_message;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8

    can_id |= Group<<8;
    can_id |= Number<<4;
    
    tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID    
    tx_message.Data[0] = Mode;
    tx_message.Data[1] = 0x55;
    tx_message.Data[2] = 0x55;
    tx_message.Data[3] = 0x55;
    tx_message.Data[4] = 0x55;
    tx_message.Data[5] = 0x55;
    tx_message.Data[6] = 0x55;
    tx_message.Data[7] = 0x55;
    
    can1_tx_success_flag = 0;
    CAN_Transmit(CAN1,&tx_message);
    while(can1_tx_success_flag == 0); //如果CAN芯片是TJA1050，注释掉这个判断。
}

/****************************************************************************************
                                  PWM速度位置模式下的数据指令
temp_pwm的取值范围如下：
0 ~ +5000

temp_velocity的取值范围如下：
0 ~ +32767

temp_position的取值范围如下：
32位有符号整型
*****************************************************************************************/
void RoboModule_PWM_Velocity_Position_Mode(unsigned char Group,unsigned char Number,short Temp_Velocity,long Temp_Position)
{
    unsigned short can_id = 0x006;
	  short Temp_PWM = 5000;          //参数直接给满，没必要改
    CanTxMsg tx_message;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
	  can_id |= Group<<8;
    can_id |= Number<<4;    
    
    tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID    
    tx_message.Data[0] = (unsigned char)((Temp_PWM>>8)&0xff);
    tx_message.Data[1] = (unsigned char)(Temp_PWM&0xff);
    tx_message.Data[2] = (unsigned char)((Temp_Velocity>>8)&0xff);
    tx_message.Data[3] = (unsigned char)(Temp_Velocity&0xff);
    tx_message.Data[4] = (unsigned char)((Temp_Position>>24)&0xff);
    tx_message.Data[5] = (unsigned char)((Temp_Position>>16)&0xff);
    tx_message.Data[6] = (unsigned char)((Temp_Position>>8)&0xff);
    tx_message.Data[7] = (unsigned char)(Temp_Position&0xff);
    
    can1_tx_success_flag = 0;
    CAN_Transmit(CAN1,&tx_message);
    while(can1_tx_success_flag == 0); //如果CAN芯片是TJA1050，注释掉这个判断。
		
}

void RoboModule_Config(u16 CAN_ID)
{
	  CanTxMsg tx_message;
	
	  tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
	
		//配置返回信息
    tx_message.StdId = CAN_ID;      //帧ID为传入参数的CAN_ID    
    tx_message.Data[0] = 0x01;      //200ms返回一次
    tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x55;
    tx_message.Data[3] = 0x55;
    tx_message.Data[4] = 0x55;
    tx_message.Data[5] = 0x55;
    tx_message.Data[6] = 0x55;
    tx_message.Data[7] = 0x55;

    can1_tx_success_flag = 0;
    CAN_Transmit(CAN1,&tx_message);
    while(can1_tx_success_flag == 0); //如果CAN芯片是TJA1050，注释掉这个判断。	
}


#define h8b(num) ((num) >> 8)
#define l8b(num) ((num)&0xff)
void RM3510Cmd(s16 V1,s16 V2,s16 V3) 
{
     CanTxMsg canTxMsg;
     canTxMsg.StdId = 0x200;
     canTxMsg.IDE   = CAN_ID_STD;
     canTxMsg.RTR   = CAN_RTR_DATA;
     canTxMsg.DLC   = 8;
     canTxMsg.Data[0] = h8b(V1);
     canTxMsg.Data[1] = l8b(V1);
     canTxMsg.Data[2] = h8b(V2);
     canTxMsg.Data[3] = l8b(V2);
     canTxMsg.Data[4] = h8b(V3);
     canTxMsg.Data[5] = l8b(V3);
     canTxMsg.Data[6] = 0;
     canTxMsg.Data[7] = 0;
	   can1_tx_success_flag = 0;
     CAN_Transmit(CAN1,&canTxMsg);

	   while(can1_tx_success_flag == 0);
}

//校准命令
void GuideCmd(void)
{
     CanTxMsg canTxMsg;
     canTxMsg.StdId = 0x003;
     canTxMsg.IDE   = CAN_ID_STD;
     canTxMsg.RTR   = CAN_RTR_DATA;
     canTxMsg.DLC   = 8;
     canTxMsg.Data[0] = Cali_X.RecChar[0];
     canTxMsg.Data[1] = Cali_X.RecChar[1];
     canTxMsg.Data[2] = Cali_X.RecChar[2];
     canTxMsg.Data[3] = Cali_X.RecChar[3];
     canTxMsg.Data[4] = Cali_Y.RecChar[0];
     canTxMsg.Data[5] = Cali_Y.RecChar[1];
     canTxMsg.Data[6] = Cali_Y.RecChar[2];
     canTxMsg.Data[7] = Cali_Y.RecChar[3];
	   can2_tx_success_flag = 0;
     CAN_Transmit(CAN2,&canTxMsg);

	   while(can2_tx_success_flag == 0);		
}

/*选择全局定位陀螺仪模式，1为MPU6050 2为大疆单轴*/
void GyroMode(u8 mode)
{
     CanTxMsg canTxMsg;
     canTxMsg.StdId = 0x004;
     canTxMsg.IDE   = CAN_ID_STD;
     canTxMsg.RTR   = CAN_RTR_DATA;
     canTxMsg.DLC   = 8;
     canTxMsg.Data[0] = mode;
     canTxMsg.Data[1] = 0;
     canTxMsg.Data[2] = 0;
     canTxMsg.Data[3] = 0;
     canTxMsg.Data[4] = 0;
     canTxMsg.Data[5] = 0;
     canTxMsg.Data[6] = 0;
     canTxMsg.Data[7] = 0;
	   can2_tx_success_flag = 0;
     CAN_Transmit(CAN2,&canTxMsg);

	   while(can2_tx_success_flag == 0);		
}

//获取全局坐标
void GetCoordinate(float *x,float *y)
{
	(*x) = Coordinate_X.RecFloat;
	(*y) = Coordinate_Y.RecFloat;
}














