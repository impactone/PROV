#include "monitor.h"
#include "usart.h"

u32 Motor0_CNT;
u32 Motor1_CNT;
u32 Motor2_CNT;
u32 RoboModule_CNT;
u32 EncoderXY_CNT;
u32 EncoderYaw_CNT;
u32 EncoderAng_CNT;
u32 DjiGyro_CNT;
u32 Manifold_CNT;
u32 UltraSonic1_CNT;
u32 UltraSonic2_CNT;
u32 UltraSonic3_CNT;
u32 UltraSonic4_CNT;

/*监控任务初始化*/
void Monitor_Init(void)
{
	 Motor0_CNT = 0;
	 Motor1_CNT = 0;
	 Motor2_CNT = 0;
	 RoboModule_CNT = 0;
	 EncoderXY_CNT = 0;
	 EncoderYaw_CNT = 0;
	 EncoderAng_CNT = 0;
	 DjiGyro_CNT = 0;
	 Manifold_CNT =0;
	 UltraSonic1_CNT = 0;
	 UltraSonic2_CNT = 0;
	 UltraSonic3_CNT = 0;
	 UltraSonic4_CNT = 0;
}

/*监控任务*/
void Monitor_Task(u8 *mode)
{
	u16 index = 0;
	 if (Motor0_CNT < MOTOR_FRAME_RATE)            /*右后轮挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"LeftMotor Err!  ");
	 }
	 if (Motor1_CNT < MOTOR_FRAME_RATE)            /*前轮挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"FrontMotor Err!  ");
	 }
	 if (Motor2_CNT < MOTOR_FRAME_RATE)            /*左后轮挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"RightMotor Err!  ");
	 }
//	 if (UltraSonic1_CNT < ULTRASONIC_FRAME_RATE)  /*超声波1挂了*/
//	 {
//		 index += sprintf(&TxDebugMsg[index],"UltraSonic1 Err!  ");
//	 }
//	 if (UltraSonic2_CNT < ULTRASONIC_FRAME_RATE)  /*超声波2挂了*/
//	 {
//		 index += sprintf(&TxDebugMsg[index],"UltraSonic2 Err!  ");
//	 }
//	 if (UltraSonic3_CNT < ULTRASONIC_FRAME_RATE)  /*超声波3挂了*/
//	 {
//		 index += sprintf(&TxDebugMsg[index],"UltraSonic3 Err!  ");
//	 }
//	 if (UltraSonic4_CNT < ULTRASONIC_FRAME_RATE)  /*超声波4挂了*/
//	 {
//		 index += sprintf(&TxDebugMsg[index],"UltraSonic4 Err!  ");
//	 }
	 if (RoboModule_CNT < ROBOMADULE_FRAME_RATE)   /*RoboModule挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"RoboModule Err!  ");
	 }
	 if (EncoderXY_CNT < ENCODREXY_FRAME_RATE)     /*编码器坐标挂了*/  
	 {
		 index += sprintf(&TxDebugMsg[index],"EncoderXY Err!  ");
	 }
	 if (EncoderAng_CNT < ENCODERANG_FRAME_RATE)   /*编码器其他两个角度挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"EncoderAng Err!  ");
	 }
#if USE_MPU6050 == 1
	 if (EncoderYaw_CNT < ENCODREYAW_FRAME_RATE)   /*编码器Yaw挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"EncoderYaw Err!  ");
	 }
#else
	 if (DjiGyro_CNT < DJIGYRO_FRAME_RATE)         /*大疆单轴挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"DjiGyro Err!  ");
	 }
#endif
//	 if (Manifold_CNT < MANIFOLD_FRAME_RATE)       /*妙算挂了*/
//	 {
//		 index += sprintf(&TxDebugMsg[index],"Manifold Err!  ");
//	 }

	 if (index)  /*肯定有东西挂了*/
	 {
		 index += sprintf(&TxDebugMsg[index],"\r\n");
		 DebugMsgSend(index);
		 *mode = 8;
	 }
	 
//		 DebugMsgSend(sprintf(TxDebugMsg,
//			" DjiGyro_CNT = %d UltraSonic1_CNT = %d\r\n",
//			 DjiGyro_CNT,UltraSonic1_CNT));
	 
	 Monitor_Init();   /*重置各计数器*/
}


