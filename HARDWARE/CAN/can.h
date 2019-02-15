#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"

#define PWM_MODE                                0x01
#define PWM_CURRENT_MODE                        0x02
#define PWM_VELOCITY_MODE                       0x03
#define PWM_POSITION_MODE                       0x04
#define PWM_VELOCITY_POSITION_MODE              0x05
#define CURRENT_VELOCITY_MODE                   0x06
#define CURRENT_POSITION_MODE                   0x07
#define CURRENT_VELOCITY_POSITION_MODE          0x08
	
union Rec
{
		char RecChar[4];
		float RecFloat;
};

typedef struct
{
	float dis[5];
	float ang;
}RadarStruc;
	
extern s16 baseMotor[4];	
extern float Yaw;
extern float Pitch;
extern float Roll;
extern union Rec Coordinate_X;
extern union Rec Coordinate_Y;
extern union Rec Cali_X;
extern union Rec Cali_Y;
extern u8 RecFromCAN;
extern RadarStruc LaserRadar;
										 							 				    
u8 CAN_Mode_Init(void);//CAN≥ı ºªØ
void RoboModule_Reset(unsigned char Group,unsigned char Number);
void RoboModule_Mode(unsigned char Group,unsigned char Number,unsigned char Mode);
void RoboModule_PWM_Velocity_Position_Mode(unsigned char Group,unsigned char Number,short Temp_Velocity,long Temp_Position);
void RoboModule_Config(u16 CAN_ID);
void RM3510Cmd(s16 V1,s16 V2,s16 V3);
void GuideCmd(void);
void GyroMode(u8 mode);
void GetCoordinate(float *x,float *y);

#endif

















