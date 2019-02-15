#ifndef __MONITOR_H
#define __MONITOR_H

#include "sys.h"
#include "Config.h"

extern u32 Motor0_CNT;
extern u32 Motor1_CNT;
extern u32 Motor2_CNT;
extern u32 RoboModule_CNT;
extern u32 EncoderXY_CNT;
extern u32 EncoderYaw_CNT;
extern u32 EncoderAng_CNT;
extern u32 DjiGyro_CNT;
extern u32 Manifold_CNT;
extern u32 UltraSonic1_CNT;
extern u32 UltraSonic2_CNT;
extern u32 UltraSonic3_CNT;
extern u32 UltraSonic4_CNT;

#define MOTOR_FRAME_RATE       90   //���֡��            100
#define ROBOMADULE_FRAME_RATE  9    //RoboModule֡��      10
#define MANIFOLD_FRAME_RATE    1    //��������֡��         0
#define ULTRASONIC_FRAME_RATE  2    //������֡��           2
#if USE_MPU6050 == 2                
	#define ENCODREXY_FRAME_RATE   4   //��������������֡��   5
	#define ENCODERANG_FRAME_RATE  4   //�������Ƕ�����֡��   5 
  #define DJIGYRO_FRAME_RATE     90 //�󽮵�������֡��     100
#else
	#define ENCODREXY_FRAME_RATE   3   //��������������֡��   3
	#define ENCODREYAW_FRAME_RATE  3   //������Yaw����֡��    3
	#define ENCODERANG_FRAME_RATE  3   //�������Ƕ�����֡��   3
#endif



void Monitor_Init(void);
void Monitor_Task(u8 *mode);

#endif 

