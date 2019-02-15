#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#include "usart.h"

/*检查车轮是否稳定的最小转速*/
#define minSpeed 100

/*舵机转动时间*/
#define Rotator1Time 5       //0.3秒
#define Rotator2Time 5       //0.3秒
#define LaserRotatorTime 30   //1.5秒

/*车体中心点到四周距离*/
#define CAR_LEFT_D  0
#define CAR_RIFHT_D 0
#define CAR_FRONT_D 0
#define CAR_BACK_D  0

/*激光分别朝左，后，右打出去所测出的距离*/
#define LASER_LEFT_D  0
#define LASER_BACK_D  0
#define LASER_RIGHT_D 0
#define LASER_SPEED       200
#define LASER_MID_ANG     0
#define LASER_LEFT_ANG    -8000
#define LASER_LEFTM_ANG   -2666
#define LASER_RIGHT_ANG   8000
#define LASER_RIGHTM_ANG  2666

/*被卡时相关参数*/
#define STUCK_TIME 20  //2秒
#define STUCK_DIS  5   //5cm


extern u16 NumOfWhiteBall;
extern u16 NumOfBlackBall;


void BaseControl(s32 v1,s32 v2,s32 v3);
void Laser_Reset(void);
void CarControl(s32 *W,  float angReal, float angExpect,
                s32 *Vx, float xReal,   s32 xExpect,
                s32 *Vy, float yReal,   s32 yExpect);
void TransCoordinate(float angles,s32 *W,s32 *Vx,s32 *Vy,s32 *V1,s32 *V2,s32 *V3);
void SpeedLimit(s32 *V1,s32 *V2,s32 *V3,s32 LimitSpeed);
void Closed_Loop(float angExpect,
                 float xReal,s32 xExpect,float yReal,s32 yExpect,
                 s32 *v1,s32 *v2,s32 *v3,u8 mode);
u8 isWhiteBall1(void);
u8 isWhiteBall2(void);
u8 Selector1(u16 TimeCounter);
u8 Selector2(u16 TimeCounter);
u8 checkStability(float error1,float error2,float error3);
u8 checkStuck(u16 counter,float x,float y,u8 reset);
void StatusCheck(RecStru *manifold_data,
                 float *angExpect,
								 float *x,float *y,
								 s32 *xExpect,s32 *yExpect,
								 u8 *mode,
								 u16 counter);
void TimerCheck(float x,float y,
								u8 *mode,
								float *angExpect,
								s32 *xExpect,s32 *yExpect,
								u16 *SystemCounter);


#endif

