#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define Wind1     TIM3->CCR1
#define Rotator1  TIM3->CCR2
#define Wind2     TIM3->CCR3
#define Rotator2  TIM3->CCR4

#define WhiteDoor  TIM8->CCR1
#define BlackDoor  TIM8->CCR2
#define XXRotator      TIM8->CCR3
#define XXXRotator     TIM8->CCR4

#define START 1
#define STOP  0

//�Ƕȴ�����
#define OPENW   170/9.0f+4.0f
#define CLOSEW  80/9.0f+4.0f 
#define OPENB   40/9.0f+4.0f
#define CLOSEB  140/9.0f+4.0f

#define MID1    90/9.0f+4.0f
#define LEFT1   50/9.0f+4.0f
#define RIGHT1  140/9.0f+4.0f

#define MID2    90/9.0f+4.0f
#define LEFT2   50/9.0f+4.0f
#define RIGHT2  145/9.0f+4.0f

enum RotatorStatus {Mid,Mid2Other,Other2Mid};

void PWM_Configuration(u16 arr,u16 psc);   //PWM��ʼ������
void WindMotor1(u8 mode);                  //�������
void WindMotor2(u8 mode);                  //�������
void RotatorControl1(u8 mode);             //������Ʒ���
void RotatorControl2(u8 mode);             //������Ʒ���
u8 OpenWhiteDoor(u8 isOpen,u16 SysCnt);    //�����򴢲��ҵ���
u8 OpenBlackDoor(u8 isOpen,u16 SysCnt);    //�����򴢲��ҵ���


#endif
