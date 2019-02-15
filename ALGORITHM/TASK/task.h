#ifndef __TASK_H
#define __TASK_H
#include "sys.h"
#include "control.h"

#define OPEN_DOOR_TIME 5      //1��
#define PUT_BALL_TIME  1      //1��
#define PUT_BALL_WAIT_TIME 3  //2��

#define ATTACK_TIME1  20 //2��
#define ATTACK_TIME2  5  //0.5��

#define ATTACK_SIDE_TIME1 30 //3s
#define ATTACK_SIDE_TIME2 10 //1s

#define CALI_INTERVAL_TIME 10 //���һ��1sУ׼һ��

#define SAFE_DIS 10   //��ȫ����Ϊ30����

#define LASER_GUIDE_SPEED 5000
#define LASER_BACK_DIS   6.081    //����㵽��ڵľ���
#define LASER_Z_DIS      1.135    //����㵽�������Ͼ���
#define LASER_H_DIS      18.734    //����㵽�������Ͼ���

#define LASER_SAFE_DIS 50 //50cm

enum Step {Step1,Step2,Step3,Step4,Step5,Step6,Step7,Step8,
           Step9,Step10,Step11,Step12,Step13,Step14,Step15,Step16,
           Step17,Step18,Step19,Step20,StepAttack,StepPutWhite,StepPutBlack};

typedef struct {
	float x;
	float y;
	float dis;
	u8 status;
}Enemy;

typedef struct{
	u8 front;
	u8 back;
}Index;


void Task_Init(void);
void EnemyMsg_Init(void);
void EnemyMsg_Reset(void);
void Index_Init(void);
u8 Whirl(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt);
u8 Attack_Task2(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt);
void Attack_Task(s32 *xExpect,s32 *yExpect,
                 float *angExpect,
								 float xReal,float yReal,
								 u16 SysCnt,
								 u8 mode);
void MonitorEnemy_Task(u8 mode,float xReal,float yReal,float *angExpect);
void WatchByLaser(u8 direction);
void WatchByUltraSonic(void);
u8 PutBall(enum Step step,u16 SysCnt);
void Move_Task(u8 mode,
               float *angExpect,
							 float x,float y,
							 s32 *xExpect,s32 *yExpect,
							 u16 SysCnt);

u8 ColScan(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt);
u8 RowScan(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt);

#endif 

