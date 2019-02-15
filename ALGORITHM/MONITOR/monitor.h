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

#define MOTOR_FRAME_RATE       90   //电机帧率            100
#define ROBOMADULE_FRAME_RATE  9    //RoboModule帧率      10
#define MANIFOLD_FRAME_RATE    1    //妙算数据帧率         0
#define ULTRASONIC_FRAME_RATE  2    //超声波帧率           2
#if USE_MPU6050 == 2                
	#define ENCODREXY_FRAME_RATE   4   //编码器坐标数据帧率   5
	#define ENCODERANG_FRAME_RATE  4   //编码器角度数据帧率   5 
  #define DJIGYRO_FRAME_RATE     90 //大疆单轴陀螺帧率     100
#else
	#define ENCODREXY_FRAME_RATE   3   //编码器坐标数据帧率   3
	#define ENCODREYAW_FRAME_RATE  3   //编码器Yaw数据帧率    3
	#define ENCODERANG_FRAME_RATE  3   //编码器角度数据帧率   3
#endif



void Monitor_Init(void);
void Monitor_Task(u8 *mode);

#endif 

