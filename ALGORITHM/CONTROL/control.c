#include "bsp.h"
#include "control.h"
#include <stdlib.h>

#define L  1.0f //��λ ��248mm

u8 ScanPosition = 0;
u8 isStartLaser = 0;

//���̵���ջ�
s16 speed_buf[3];
void BaseControl(s32 v1,s32 v2,s32 v3)
{
	
	  speed_buf[0] = PID_BaseControl_Calculate(&PID_BaseControl_V_Struct1, baseMotor[0], v1);
	  speed_buf[1] = PID_BaseControl_Calculate(&PID_BaseControl_V_Struct2, baseMotor[1], v2);
		speed_buf[2] = PID_BaseControl_Calculate(&PID_BaseControl_V_Struct3, baseMotor[2], v3);
	
	RM3510Cmd(speed_buf[0],speed_buf[1],speed_buf[2]);
}


#define CAR_SPEED 4000.0f
//�����˶��ջ�
void CarControl(s32 *W,  float angReal, float angExpect,
                s32 *Vx, float xReal,   s32 xExpect,
                s32 *Vy, float yReal,   s32 yExpect)
{
    (*W)  = posPID_Calculate(&PID_RotateControl,angReal,angExpect);
	  (*Vx) = posPID_Calculate2(&PID_XControl,xReal,xExpect);
	  (*Vy) = posPID_Calculate2(&PID_YControl,yReal,yExpect);
		if ( (*Vx) > CAR_SPEED)  (*Vx) = CAR_SPEED;  //��Ŀ��̫Զ�������˶�
		if ( (*Vx) < -CAR_SPEED) (*Vx) = -CAR_SPEED;
    if ( (*Vy) > CAR_SPEED)  (*Vy) = CAR_SPEED;
		if ( (*Vy) < -CAR_SPEED) (*Vy) = -CAR_SPEED;
}

//��������ϵ�ٶ��������ٶ�ת��
void TransCoordinate(float angles,s32 *W,s32 *Vx,s32 *Vy,s32 *V1,s32 *V2,s32 *V3)
{
	//���ʧ�ܣ���������û�и���
//		(*V1) = SIN[30-angles] * (*Vx) + COS[30-angles] * (*Vy) + (*W) * L;
//	  (*V2) = -COS[angles] * (*Vx) - SIN[angles] * (*Vy) + (*W) * L;
//	  (*V3) = SIN[30+angles] * (*Vx) - COS[30+angles] * (*Vy) + (*W) * L;
			(*V1) = sin(0.5236f-angles) * (*Vx) + cos(0.5236f-angles) * (*Vy) + (*W) * L;
	    (*V2) = -cos(angles) * (*Vx) - sin(angles) * (*Vy) + (*W) * L;
	    (*V3) = sin(0.5236f+angles) * (*Vx) - cos(0.5236f+angles) * (*Vy) + (*W) * L;

	
}
//�����ٶ��޷�
void SpeedLimit(s32 *V1,s32 *V2,s32 *V3,s32 LimitSpeed)
{
	if ((*V1) >= LimitSpeed)  (*V1) = LimitSpeed;
	if ((*V1) <= -LimitSpeed) (*V1) = -LimitSpeed;
	if ((*V2) >= LimitSpeed)  (*V2) = LimitSpeed;
	if ((*V2) <= -LimitSpeed) (*V2) = -LimitSpeed;
	if ((*V3) >= LimitSpeed)  (*V3) = LimitSpeed;
	if ((*V3) <= -LimitSpeed) (*V3) = -LimitSpeed;
}

//λ�˱ջ�
#define Max(x,y) (abs(x)>abs(y)?x:y)
#define Min(x,y) (abs(x)>abs(y)?y:x)
void Closed_Loop(float angExpect,
                 float xReal,s32 xExpect,float yReal,s32 yExpect,
                 s32 *v1,s32 *v2,s32 *v3,u8 mode)
{
	s32 W,Vx,Vy; //������̬����

  CarControl(&W,Yaw,angExpect,&Vx,xReal,xExpect,&Vy,yReal,yExpect);  //Step1 �ջ�	
	switch (mode)
	{
		case 8:
			Vx = 0;
		  Vy = 0;
		  W = 0;
		  break;
		case 'w'-48:
			Vx = 0;
		  Vy = 2000;
			break;
		case 's'-48:
			Vx = 0;
		  Vy = -2000;			
			break;
		case 'a'-48:
			Vx = -2000;
		  Vy = 0;
		  break;
		case 'd'-48:
			Vx = 2000;
		  Vy = 0;
			break;
		default:
			break;
	}
	TransCoordinate(Yaw,&W,&Vx,&Vy,v1,v2,v3);                          //Step2 ת��
	SpeedLimit(v1,v2,v3,pwmMax);                                       //Step3 �޷�	
}

//�ж��Ƿ��ǰ���  1Ϊ��
u8 isWhiteBall1(void)
{
	if (red1+green1+blue1 <= 80)
		return 0;
	else
	  return 1;
}

//�ж��Ƿ��ǰ���  1Ϊ��
u8 isWhiteBall2(void)
{
	if (red2+green2+blue2 <= 90)
	  return 0;
	else
		return 1;
}

//������1
u16 NumOfWhiteBall = 0;
u16 NumOfBlackBall = 0;
u8 Selector1(u16 TimeCounter)
{
	static enum RotatorStatus Rotator1Status = Mid;
	static  u8 NumOfdelay = 0;
	static u16 preCounter = 0;
	if (!isBall1) NumOfdelay++; 
  if ((NumOfdelay >= 2) & (Rotator1Status == Mid))
	{
		if (isWhiteBall1())
		{
			NumOfWhiteBall++;
			RotatorControl1(RIGHT1);
			Rotator1Status = Mid2Other;
			preCounter = TimeCounter;
			return 0;  //����ȥ
		}
		else
		{
			NumOfBlackBall++;
			RotatorControl1(LEFT1);
			Rotator1Status = Mid2Other;
			preCounter = TimeCounter;			
			return 0;  //����ȥ
		}
	}
	if (TimeCounter - preCounter >= Rotator1Time)
	{
		if (Rotator1Status == Mid2Other)
		{
			RotatorControl1(MID1);
			Rotator1Status = Other2Mid;
			preCounter = TimeCounter;
			return 0; //����������ν��
		}
		else
		{
			Rotator1Status = Mid;
			NumOfdelay = 0;       //���һ��ʹ��
			preCounter = TimeCounter;
		}
	}
	return 0;
}

//������2
u8 Selector2(u16 TimeCounter)
{
	static enum RotatorStatus Rotator2Status = Mid;
	static  u8 NumOfdelay = 0;
	static u16 preCounter = 0;
	if (!isBall2) NumOfdelay++; 
  if ((NumOfdelay >= 2) & (Rotator2Status == Mid))
	{
		if (isWhiteBall2())
		{
			NumOfWhiteBall++;
			RotatorControl2(RIGHT2);
			Rotator2Status = Mid2Other;
			preCounter = TimeCounter;
			return 0;  //����ȥ
		}
		else
		{
			NumOfBlackBall++;
			RotatorControl2(LEFT2);
			Rotator2Status = Mid2Other;
			preCounter = TimeCounter;			
			return 0;  //����ȥ
		}
	}
	if (TimeCounter - preCounter >= Rotator2Time)
	{
		if (Rotator2Status == Mid2Other)
		{
			RotatorControl2(MID2);
			Rotator2Status = Other2Mid;
			preCounter = TimeCounter;
			return 0; //����������ν��
		}
		else
		{
			Rotator2Status = Mid;
			NumOfdelay = 0;       //���һ��ʹ��
			preCounter = TimeCounter;
		}
	}
	return 0;
}

//�鿴�����ȶ���
//����1 �ȶ�  ����0 ���ȶ�
u8 checkStability(float error1,float error2,float error3)
{
	/*�����ȶ�������������
	  1.�趨ֵ��Ŀ��ֵ���
	  2.���ֵ��ٶȻ�������ת
	*/
	if ((abs(error1) <= 2.0f) && 
		 (abs(error2) <= 2.0f)  && 
	   (abs(error3) <= 0.1f)  &&
		 (abs(baseMotor[0]) <= minSpeed) &&
	   (abs(baseMotor[1]) <= minSpeed) &&
	   (abs(baseMotor[2]) <= minSpeed))
	  return 1;
	else
		return 0;
}

/*��鳵���Ƿ񱻿�*/
/*�������뺯��һ��Ҫ�����ò���*/
/*���û��ȫ�ӣ��е�ʧ��*/
u8 isStuck = 0;
u8 checkStuck(u16 counter,float x,float y,u8 reset)
{
	static u16 last_counter = 0;
	static float last_x = 0,last_y = 0;
	u8 stuck_flag = 0;
	if (reset)
	{
		last_counter = 0;
		last_x = 0;
		last_y = 0;
	}else
	{
		if (counter - last_counter > STUCK_TIME)
		{
			if ((x - last_x < STUCK_DIS) && ((y - last_y) < STUCK_DIS))
				stuck_flag = 1;
			else
				stuck_flag = 0;
			last_x = x;
			last_y = y;
			last_counter = counter;
		}
		if (stuck_flag) isStuck = 1;
	}
	return stuck_flag;
}

void Laser_Reset(void)
{
	LaserRadar.dis[0] = 65535;
	LaserRadar.dis[1] = 65535;
	LaserRadar.dis[2] = 65535;
	LaserRadar.dis[3] = 65535;
	LaserRadar.dis[4] = 65535;
}

/*����ɨ��*/
void LaserScan(void)
{ 
	static u8 LaserStatus = 1;
	
	if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-100,LASER_LEFT_ANG+100)) 
	{
		LaserRadar.dis[0] = Get_Dis();
		LaserStatus = 2;
	}
	if (isInAInterval(LaserRadar.ang,LASER_LEFTM_ANG-100,LASER_LEFTM_ANG+100))
		LaserRadar.dis[1] = Get_Dis();
	if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-100,LASER_MID_ANG+100))
		LaserRadar.dis[2] = Get_Dis();
	if (isInAInterval(LaserRadar.ang,LASER_RIGHTM_ANG-100,LASER_RIGHTM_ANG+100))
		LaserRadar.dis[3] = Get_Dis();
		if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-100,LASER_RIGHT_ANG+100))
	{
		LaserRadar.dis[4] = Get_Dis();
		LaserStatus = 1;
	}
	/*���ⷽ���л�*/
	switch (LaserStatus)
	{
		case 1:
			RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_SPEED,LASER_LEFT_ANG);
			break;
		case 2:
			RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_SPEED,LASER_RIGHT_ANG);
			break;
		default:
			break;
	}
	
}

/*״̬��ѯ*/
void StatusCheck(RecStru *manifold_data,
                 float *angExpect,
								 float *x,float *y,
								 s32 *xExpect,s32 *yExpect,
								 u8 *mode,
								 u16 counter)
{
		if (RecFromUsart)  //���ڽ��յ���������
		{					
			ProcessMANIFOLD(manifold_data,angExpect,*x,xExpect,*y,yExpect,counter);
			RecFromUsart = 0;
		}
		if (RecFromCAN)    //CAN���յ�ȫ��������Ϣ
		{
			GetCoordinate(x,y);
		  RecFromCAN = 0;
		}
		if (RecFrom8266)   //���ڽ��յ�wifi����
		{
			(*mode) = ESP8266_Decode();
			RecFrom8266 = 0;
		}
		if (isOver)        //��ɫ���������һ�μ��
		{
		  Selector1(counter); 
			Selector2(counter); 
			isOver = 0;
		}
		if (isStartLaser)
		{
		  LaserScan();       //����ɨ��
		}
//  		DebugMsgSend(sprintf(TxDebugMsg,
//			"L = %.1f LM = %.1f M = %.1f RM = %.1f R = %.1f\r\n",
//			 LaserRadar.dis[0],LaserRadar.dis[1],LaserRadar.dis[2],LaserRadar.dis[3],LaserRadar.dis[4]));

}

/*ʱ��Ƭ��ѯ*/
void TimerCheck(float x,float y,
								u8 *mode,
								float *angExpect,
								s32 *xExpect,s32 *yExpect,
								u16 *SystemCounter)
{
	static s32 V1 = 0,V2 = 0,V3 = 0;  //���������ٶ�
	
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
			Closed_Loop(*angExpect,x,*xExpect,y,*yExpect,&V1,&V2,&V3,*mode);
			__30ms = 0;
		}
		if (__100ms)  //100ms����Ƶ��
		{ 
			(*SystemCounter)++;
      Monitor_Task(mode);

			
//			Move_Task(*mode,angExpect,x,y,xExpect,yExpect,(*SystemCounter));
			
			
			
//			if ((*mode) == 1 || (*mode) == 2)
//			{
//				if ((*SystemCounter) <= COUNT_1MIN)
//					Move_Task(*mode,angExpect,x,y,xExpect,yExpect,(*SystemCounter));
//				else
//					if (Whirl(x,xExpect,y,yExpect,angExpect,(*SystemCounter)))
//					{
//						Attack_Task2(x,xExpect,y,yExpect,angExpect,(*SystemCounter));
//					}
//			}
//			if ((*mode) == 3)
//			{
//				Attack_Task2(x,xExpect,y,yExpect,angExpect,(*SystemCounter));
//			}
			
			checkStuck((*SystemCounter),x,y,0);
			
			/*�����ջ�����*/
		/*	if ((*SystemCounter) >= 50) 
			{
				(*angExpect) = 3.14f;
			}
			if ((*SystemCounter) >= 100)
			{
				(*angExpect) = 0;
				(*SystemCounter) = 0;
			}*/

			__100ms = 0;
		}	
}



