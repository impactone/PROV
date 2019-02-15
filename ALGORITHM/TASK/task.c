#include "task.h"
#include "pwm.h"
#include "magnet.h"
#include "can.h"
#include "manifold.h"
#include "laser.h"
#include "UltraSonic.h"
#include "control.h"


extern u8 ScanPosition;
extern u8 isStartLaser;

enum Step CS_StepN = Step1; //从第一步开始
enum Step RS_StepN = Step1; //从第一步开始
enum Step AR_StepN = Step1; //从第一步开始
enum Step AT_StepN = Step1; //从第一步开始
enum Step Temp_Step;

//攻击点坐标
s32 AttackX = 82;
s32 AttackY = 240;
float AttackAng = -1.57f;

//各边界坐标
s32 BorderX[] = {-205,205};
s32 BorderY[] = {10,400};
float BorderAng1[] = {-1.57f,-3.14f,0,1.57f};
u8 index_table[4];

//各点坐标       1     2   3    4    5    6     7    8    9    10
s32 ColX1[] = {-210, -50, 50,  210, 210, -210, -210, 210, 210, -210};
s32 ColY1[] = {162,  162, 162, 162, 112, 112,   62,  62,  12,  12};
s32 ColX2[] = {210,   50, -50, -210,-210, 210,  210, -210, -210, 210};
float ColAng[2] = {1.57f,-1.57f};
float ColAttackAng[2][2];

//各点坐标       1     2   3    4    5    6     7    8    9    10
s32 RowX1[] = {-205, -50, 50, 205, 205, -205, -205, 205, 205, -205};
s32 RowY1[] = {155,  155, 155, 155, 110, 110,   60,  60,  10,  10};
s32 RowY2[] = {280,  280, 280,280, 320, 320,  370, 370, 405, 405};
float RowAng[2] = {0.0f,3.14f};
float RowAttackAng[2][2];


/*敌人状态*/
/* 8  7  6  5  4  3  2  1*/
/* 空 空 空 空 左 右 前 后*/
Enemy EnemyMsg;

void Task_Init(void)
{
	EnemyMsg_Init();
	Index_Init();
}

void EnemyMsg_Init(void)
{
	EnemyMsg.dis = 0;
	EnemyMsg.status = 0;
	EnemyMsg.x = 0;
	EnemyMsg.y = 0;
	
	ColAttackAng[0][0] = 3.14f;
	ColAttackAng[0][1] = 0.0f;
	ColAttackAng[1][0] = -3.14f;
	ColAttackAng[1][1] = 0.0f;
	
	RowAttackAng[0][0] = -1.57f;
	RowAttackAng[0][1] = 1.57f;
	RowAttackAng[1][0] = 4.71f;
	RowAttackAng[1][1] = 1.57f;
	
}

void EnemyMsg_Reset(void)
{
	EnemyMsg.dis = 0;
	EnemyMsg.status = 0;
	EnemyMsg.x = 0;
	EnemyMsg.y = 0;
}

void Index_Init(void)
{
	index_table[0] = 2;
	index_table[1] = 0;
	index_table[2] = 3;
	index_table[3] = 1;
	
}

u8 Attack_Task2(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt)
{
	static u16 last_cnt = 0;
	switch (AT_StepN)
	{
		case Step1:
			(*xExpect) = RowX1[Step1];
		  (*yExpect) = RowY1[Step1];
		  (*angExpect) = 0.0f;
		  RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);	
		  if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS;  
				Cali_Y.RecFloat = -1000.0f;	
        GuideCmd();				
				AT_StepN = Step2;
			}
			break;
		case Step2:
			(*xExpect) = RowX1[Step2];
		  (*yExpect) = RowY1[Step2];
		  if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				if (NumOfWhiteBall)
				{
					PutBall(StepPutWhite,SysCnt);					
				}else
				{
					AT_StepN = Step3;
				}
			}			
			break;
		case Step3:
			if (NumOfBlackBall)
			{
				(*xExpect) = RowX1[Step3];
				(*yExpect) = RowY1[Step3];
				if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
				{
					PutBall(StepPutBlack,SysCnt);
				}		
			}
			else
			{
				AT_StepN = Step4;
			}			
			break;
		case Step4:
			(*xExpect) = AttackX;
      if (checkStability(xReal-(*xExpect),0,0))
			{
				AT_StepN = Step5;
			}
			break;
		case Step5:
			(*yExpect) = AttackY;	  
      if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				(*angExpect) = AttackAng;
				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);			
				if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
				{
					isStartLaser = 1;   //开激光雷达
					RaiseUp();          //抬板
					Laser_Reset();
					AT_StepN = Step6;
				}
			}		  
			break;
		case Step6:
			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				if (LaserRadar.dis[0] <= LASER_SAFE_DIS) 
				{
					AT_StepN = Step7;
					last_cnt = SysCnt;
					isStartLaser = 0;
				}
				if (LaserRadar.dis[1] <= LASER_SAFE_DIS) 
        {
					AT_StepN = Step13;
					last_cnt = SysCnt;
					isStartLaser = 0;
				}
				if (LaserRadar.dis[2] <= LASER_SAFE_DIS) 
        {
					AT_StepN = Step7;
					last_cnt = SysCnt;
					isStartLaser = 0;
				}
				if (LaserRadar.dis[3] <= LASER_SAFE_DIS) 
        {
					AT_StepN = Step7;
					last_cnt = SysCnt;
					isStartLaser = 0;
				}
				if (LaserRadar.dis[4] <= LASER_SAFE_DIS) 
        {
					AT_StepN = Step10;
					last_cnt = SysCnt;
					isStartLaser = 0;
				}
		  }
      break;
		case Step7:
		 (*yExpect) = xReal - LaserRadar.dis[0];	
		 (*angExpect) = 0.0f;
      PutDown();  //放板		
		  if (SysCnt - last_cnt >= ATTACK_SIDE_TIME1)
			{
				RaiseUp(); //抬板
				last_cnt = SysCnt;
				AT_StepN = Step8;
			}
			break;
		case Step8:
			if (SysCnt - last_cnt >= ATTACK_SIDE_TIME2)
			{
				(*yExpect) = AttackY;
				(*angExpect) = AttackAng;
				AT_StepN = Step9;
				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);	
			}
			break;
		case Step9:
			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{					
				if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
				{
					isStartLaser = 1;   //开激光雷达
					Laser_Reset();
					AT_StepN = Step6;
				}				
			}
			break;
		case Step10:
		 (*yExpect) = xReal + LaserRadar.dis[0];	
		 (*angExpect) = -3.14f;
      PutDown();  //放板		
		  if (SysCnt - last_cnt >= ATTACK_SIDE_TIME1)
			{
				RaiseUp(); //抬板
				last_cnt = SysCnt;
				AT_StepN = Step11;
			}			
			break;
		case Step11:
			if (SysCnt - last_cnt >= ATTACK_SIDE_TIME2)
			{
				(*yExpect) = AttackY;
				(*angExpect) = AttackAng;
				AT_StepN = Step12;
				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);	
			}
			break;
    case Step12:
			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{					
				if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
				{
					isStartLaser = 1;   //开激光雷达
					Laser_Reset();
					AT_StepN = Step6;
				}				
			}
			break; 
    case Step13:
			
			break;
		default:
			break;
	}
}

u8 x_index,y_index;
//u8 Whirl(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt)
//{
//	static u8 arroud_end = 0;
//	static u16 last_cnt = 0;
//	u8 temp_x,temp_y;
//	switch (AR_StepN)
//	{
//		case Step1:
//			if (xReal < 0) x_index = 0;
//			else           x_index = 1;
//		  if (yReal < 240) y_index = 0;
//		  else             y_index = 1;
//		  (*xExpect) = BorderX[x_index];
//		  (*yExpect) = BorderY[y_index];
//      if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))		
//			{
//				(*angExpect) = BorderAng1[(x_index<<1|y_index)];
//				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
//		    AR_StepN = Step2;
//			}
//			break;
//		case Step2:			
//		  if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
//				{
//					Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS;  
//					Cali_Y.RecFloat = -1000.0f;
//					GuideCmd();					
//				}
//				AR_StepN = Step3;
//			}
//			break;
//		case Step3:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				if (arroud_end)
//					if (!y_index)
//						AR_StepN = Step10;
//				temp_x = index_table[(x_index<<1|y_index)] >> 1;
//				temp_y = index_table[(x_index<<1|y_index)] & 0x01;
//				(*xExpect) = BorderX[temp_x];
//				(*yExpect) = BorderY[temp_y];		
//        					
//				x_index = temp_x;
//				y_index = temp_y;
//				AR_StepN = Step3;
//			}
//			break;
//		case Step3:			
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				if (arroud_end)
//					if (!y_index)
//						AR_StepN = Step10;
//				temp_x = index_table[(x_index<<1|y_index)] >> 1;
//				temp_y = index_table[(x_index<<1|y_index)] & 0x01;
//				(*angExpect) = BorderAng1[(x_index<<1|y_index)];
//				x_index = temp_x;
//				y_index = temp_y;		
//        AR_StepN = Step4;				
//			}
//			break;
//		case Step4:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				(*xExpect) = BorderX[x_index];
//				(*yExpect) = BorderY[y_index];
//				AR_StepN = Step5;	
//			}
//			break;
//		case Step5:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				if (arroud_end)
//					if (!y_index)
//						AR_StepN = Step10;				
//				temp_x = index_table[(x_index<<1|y_index)] >> 1;
//				temp_y = index_table[(x_index<<1|y_index)] & 0x01;

//				(*angExpect) = BorderAng1[(x_index<<1|y_index)];
//				x_index = temp_x;
//				y_index = temp_y;	
//        AR_StepN = Step6;				
//			}
//		case Step6:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				(*xExpect) = BorderX[x_index];
//				(*yExpect) = BorderY[y_index];
//				AR_StepN = Step7;
//			}				
//			break;
//		case Step7:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				if (arroud_end)
//					if (!y_index)
//						AR_StepN = Step10;				
//				temp_x = index_table[(x_index<<1|y_index)] >> 1;
//				temp_y = index_table[(x_index<<1|y_index)] & 0x01;

//				(*angExpect) = BorderAng1[(x_index<<1|y_index)];
//				x_index = temp_x;
//				y_index = temp_y;	
//        AR_StepN = Step8;				
//			}			
//			break;		
//		case Step8:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				(*xExpect) = BorderX[x_index];
//				(*yExpect) = BorderY[y_index];
//				AR_StepN = Step9;	
//			}				
//			break;
//		case Step9:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				arroud_end = 1;
//				AR_StepN = Step2;
//			}
//			break;
//		case Step10:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//				(*xExpect) = RowX1[Step2];
//				(*yExpect) = RowY1[Step2];
//				(*angExpect) = 0.0f;
//				AR_StepN = Step11;
//			}
//			break;
//		case Step11:
//			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
//			{
//        if (NumOfWhiteBall)
//				{
//					PutBall(StepPutWhite,SysCnt);					
//				}else
//				{
//					RS_StepN = Step12;
//				}				
//			}			
//			break;
//		case Step12:
//			if (NumOfBlackBall)
//			{
//				(*xExpect) = RowX1[Step3];
//				(*yExpect) = RowY1[Step3];
//				if (checkStability(xReal-RowX1[Step3],yReal-RowY1[Step3],0))
//				{
//					PutBall(StepPutBlack,SysCnt);
//				}		
//			}
//			else
//			{
//				AT_StepN = Step4;
//				return 1;
//			}				
//			break;
//		default:
//			break;
//	}
//	return 0;
//}

///*攻击任务*/
void Attack_Task(s32 *xExpect,s32 *yExpect,
								 float *angExpect,
								 float xReal,float yReal,
								 u16 SysCnt,
								 u8 mode)
{
	static u16 last_cnt = 0;
	static u8 attack_status = 1;  //1为放板 2为去攻击点ing 3为抬板ing
	if (EnemyMsg.status)     //不为零就代表要干
	{		
		switch (attack_status)
		{
			case 1:
				(*xExpect) = EnemyMsg.x; //设定攻击点
				(*yExpect) = EnemyMsg.y;
			  PutDown();  //放板
			  attack_status = 2;
			  last_cnt = SysCnt;
				break;
			case 2:
				if (SysCnt - last_cnt > ATTACK_TIME1)
				{
					RaiseUp();  //抬板
					attack_status = 3;
					last_cnt = SysCnt;
				}
				break;
			case 3:
				if (SysCnt - last_cnt > ATTACK_TIME2)
				{
					if (mode == 1)  //行扫描
					{
					  (*xExpect) = RowX1[Temp_Step-1];
						if (!ScanPosition)
							(*yExpect) = RowY1[Temp_Step-1];
						else
							(*yExpect) = RowY2[Temp_Step-1];		
						if (checkStability(xReal-(*xExpect),yReal-(*yExpect),0))
						{
							attack_status = 4;
						}
					}
					if (mode == 2)  //列扫描
					{
						(*yExpect) = ColY1[Temp_Step-1];	
						if (!ScanPosition)
					    (*xExpect) = ColX1[Temp_Step-1];
						else
							(*xExpect) = ColX2[Temp_Step-1];							
						CS_StepN = Temp_Step;
					}			
				}				
				break;
			case 4:
				if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
				{
					if (Temp_Step == Step6 || Temp_Step == Step10)
					{
						(*angExpect) = 1.57f;
						attack_status = 5;
					}else
					if (Temp_Step == Step8)
					{
						(*angExpect) = -1.57f;
						attack_status = 5;	
					}
				}							
				break;
			case 5:
				if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
				{
					if (Temp_Step == Step6 || Temp_Step == Step10)
					{
						ScanPosition = !ScanPosition;
						(*xExpect) = RowX1[Step5];
						if (!ScanPosition)
							(*yExpect) = RowY1[Step5];
						else
							(*yExpect) = RowY2[Step5];	
            last_cnt = SysCnt;						
            attack_status = 6;						
					}else
					if (Temp_Step == Step8)
					{
						ScanPosition = !ScanPosition;
						(*xExpect) = RowX1[Step1];
						if (!ScanPosition)
							(*yExpect) = RowY1[Step1];
						else
							(*yExpect) = RowY2[Step1];
            last_cnt = SysCnt;						
            attack_status = 6;		
					}					
				}
				break;
			case 6:
				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
				if (SysCnt - last_cnt >= CALI_INTERVAL_TIME)
				{
					if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
					{
						if (xReal < 0)
							Cali_X.RecFloat = Get_Dis() + LASER_H_DIS - 240;  
						else
							Cali_X.RecFloat = 240 - Get_Dis() - LASER_H_DIS; 
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();	
						last_cnt = SysCnt;			
					}
				}
				if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
				{
					if (xReal < 0)
					{
						RS_StepN = Step1;
						EnemyMsg_Reset();  //攻击结束
						attack_status = 1;
					}else
					{
						RS_StepN = Step5;
						EnemyMsg_Reset();  //攻击结束
						attack_status = 1;						
					}
				}					
				break;
			default:
				break;
		}

	}
	
}

/*警戒任务*/
void MonitorEnemy_Task(u8 mode,float xReal,float yReal,float *angExpect)
{
	 WatchByUltraSonic();  
	 switch (mode)
	{
		case 1:  //行扫描
			if (!ScanPosition)  //己方侧扫描
			{
				if (RS_StepN == Step6 || RS_StepN == Step10)
				{
					if (EnemyMsg.status&0x08)  //左边有车
					{
						EnemyMsg.x = xReal - EnemyMsg.dis;
						EnemyMsg.y = yReal;
						Temp_Step = RS_StepN;  //保留现场
						RS_StepN = StepAttack; //准备攻击
						(*angExpect) = RowAttackAng[ScanPosition][0];										
					}					
				}else
					if (RS_StepN == Step8)
					{
						if (EnemyMsg.status&0x04)  //右边有车
						{
							EnemyMsg.x = xReal + EnemyMsg.dis;
							EnemyMsg.y = yReal;
							Temp_Step = RS_StepN;  //保留现场
							RS_StepN = StepAttack; //准备攻击
							(*angExpect) = RowAttackAng[ScanPosition][1];
						}				
					}
			}else  //敌方侧扫描
			{
				if (RS_StepN == Step8)
				{
					if (EnemyMsg.status&0x08)  //左边有车
					{
						EnemyMsg.x = xReal + EnemyMsg.dis;
						EnemyMsg.y = yReal;
						Temp_Step = RS_StepN;  //保留现场
						RS_StepN = StepAttack; //准备攻击
						(*angExpect) = RowAttackAng[ScanPosition][1];										
					}						
				}else
				if (RS_StepN == Step6 || RS_StepN == Step10)
				{
					if (EnemyMsg.status&0x04)  //右边有车
					{
						EnemyMsg.x = xReal - EnemyMsg.dis;
						EnemyMsg.y = yReal;
						Temp_Step = RS_StepN;  //保留现场
						RS_StepN = StepAttack; //准备攻击
						(*angExpect) = RowAttackAng[ScanPosition][0];
					}					
				}
			}
			break;
		case 2:  //列扫描
			if (CS_StepN == Step4 || CS_StepN == Step8)
			{
				if (EnemyMsg.status&0x08)  //左方有车
				{
					EnemyMsg.x = xReal;
					EnemyMsg.y = yReal + EnemyMsg.dis;
					Temp_Step = CS_StepN;
					CS_StepN = StepAttack;
					(*angExpect) = ColAttackAng[ScanPosition][0];
					ScanPosition = !ScanPosition;
				}
			}else
			if (CS_StepN == Step6)
			{
				if (EnemyMsg.status&0x04)  //右方有车
				{
					EnemyMsg.x = xReal;
					EnemyMsg.y = yReal - EnemyMsg.dis;
					Temp_Step = CS_StepN;
					CS_StepN = StepAttack;
					(*angExpect) = ColAttackAng[ScanPosition][1];
					ScanPosition = !ScanPosition;
				}				
			}
			break;
		case 3:
			break;
		default:
			break;
	}
}

/*1为左边 2为右边*/
void WatchByLaser(u8 direction)
{
	float laser_dis;
		if (direction == 1)
		{
			RoboModule_PWM_Velocity_Position_Mode(0,2,5000,LASER_LEFT_ANG);
			if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-10,LASER_LEFT_ANG+10))
			{
				laser_dis = Get_Dis();
				if (laser_dis <= SAFE_DIS)
				{
					EnemyMsg.dis = laser_dis;
					EnemyMsg.status = 1<<3;  //左边有人
				}
			}
		}
		else
		{
			RoboModule_PWM_Velocity_Position_Mode(0,2,5000,LASER_RIGHT_ANG);
			if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-10,LASER_RIGHT_ANG+10))
			{
				laser_dis = Get_Dis();
				if (laser_dis <= SAFE_DIS)
				{
					EnemyMsg.dis = laser_dis;
					EnemyMsg.status = 1<<2;  //右边有人
				}
			}			
		}
}

void WatchByUltraSonic(void)
{	
	static u8 ultrasnoic_times = 3;
	static u8 warnig1_cnt = 0;
	static u8 warnig2_cnt = 0;      
	if (ultrasnoic_times--)
	{
		if (Dis_1 <= SAFE_DIS)
		{
			warnig1_cnt++;
			if (warnig1_cnt >= 2)
			{
				EnemyMsg.dis = Dis_1;
				EnemyMsg.status = 1<<3;  //左边有人
			}
		}
		if (Dis_2 <= SAFE_DIS)
		{
			warnig2_cnt++;
			if (warnig2_cnt >= 2)
			{
				EnemyMsg.dis = Dis_2;
				EnemyMsg.status = 1<<2;  //右边有人
			}
		}			
	}
	else
  {
		warnig1_cnt = 0;
		warnig2_cnt = 0;
		ultrasnoic_times = 3;
	}		
}

/*放球*/
#define PUSH_TIMES(x) x>=3?2:1
u8 PutBall(enum Step step,u16 SysCnt)
{
	if (step == StepPutWhite)   //放白球
	{
		if (OpenWhiteDoor(1,SysCnt))  //1.舱门已开启
		{
			if (PushWhite(PUSH_TIMES(NumOfWhiteBall),SysCnt))  //2.推球已完成
			{
				OpenWhiteDoor(0,SysCnt);  //3.关舱门
				NumOfWhiteBall = 0;       //4.球已清空
				return 1;                 //放球结束
			}			
		}
	}else
	if (step == StepPutBlack)   //放黑球
	{
		if (OpenBlackDoor(1,SysCnt))  //1.舱门已开启
		{
			if (PushBlack(PUSH_TIMES(NumOfBlackBall),SysCnt))  //2.推球已完成
			{
				OpenBlackDoor(0,SysCnt);  //3.关舱门
				NumOfBlackBall = 0;       //4.球已清空
				return 1;                 //放球结束
			}
		}
	}
  return 0;
}

/*移动任务*/
void Move_Task(u8 mode,
               float *angExpect,
							 float x,float y,
							 s32 *xExpect,s32 *yExpect,
               u16 SysCnt)
{
	switch (mode)
	{
		case 1:
			RowScan(x,xExpect,y,yExpect,angExpect,SysCnt);
		  break;
		case 2:
			ColScan(x,xExpect,y,yExpect,angExpect,SysCnt);
		  break;
		case 3:
			//AttackMode();
		  break;
		default:
			break;
	}
}




//列扫描
u8 ColScan(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt)
{
	s32 *ColPositionX;
	s32 *ColPositionY;
	if (ScanPosition == 0)   /*己方侧列扫描*/
	{
		ColPositionX = ColX1;
		ColPositionY = ColY1;
	}
	if (ScanPosition == 1)   /*敌方侧列扫描*/
	{
		ColPositionX = ColX2;  
		ColPositionY = ColY1;  /*纵坐标不变*/
	}
	switch (CS_StepN) 
	{
		case Step1:
			(*xExpect) = ColPositionX[Step1];
		  (*yExpect) = ColPositionY[Step1];
		  if (checkStability(xReal-ColPositionX[Step1],yReal-ColPositionY[Step1],0))
			{
				CS_StepN = Step2;
				(*xExpect) = ColPositionX[Step2];
		    (*yExpect) = ColPositionY[Step2];
			}
			break;
		case Step2:
		  if (checkStability(xReal-ColPositionX[Step2],yReal-ColPositionY[Step2],0))
			{
				if (NumOfWhiteBall)
				{				
						PutBall(StepPutWhite,SysCnt);					
				}else
				{
					CS_StepN = Step3;
				}
			}			
			break;
		case Step3:
			if (NumOfBlackBall)
			{
				(*xExpect) = ColPositionX[Step3];
				(*yExpect) = ColPositionY[Step3];
				if (checkStability(xReal-ColPositionX[Step3],yReal-ColPositionY[Step3],0))
				{
						PutBall(StepPutBlack,SysCnt);
				}		
			}
			else
			{
				CS_StepN = Step4;				
			}
			break;
		case Step4:
			(*xExpect) = ColPositionX[Step4];
		  (*yExpect) = ColPositionY[Step4];
		  (*angExpect) = ColAng[ScanPosition];
		  MonitorEnemy_Task(2,xReal,yReal,angExpect);
		  if (checkStability(xReal-ColPositionX[Step4],yReal-ColPositionY[Step4],Yaw-ColAng[ScanPosition]))
			{
				CS_StepN = Step5;
			}			
			break;			
		case Step5:
			(*xExpect) = ColPositionX[Step5];
		  (*yExpect) = ColPositionY[Step5];
		  (*angExpect) = ColAng[ScanPosition];
		  if (checkStability(xReal-ColPositionX[Step5],yReal-ColPositionY[Step5],Yaw-ColAng[ScanPosition]))
			{
				CS_StepN = Step6;
				(*xExpect) = ColPositionX[Step6];
		    (*yExpect) = ColPositionY[Step6];
			}			
			break;
		case Step6:
		  if (checkStability(xReal-ColPositionX[Step6],yReal-ColPositionY[Step6],0))
			{
				CS_StepN = Step7;
				(*xExpect) = ColPositionX[Step7];
		    (*yExpect) = ColPositionY[Step7];
			}			
			break;
		case Step7:
		  if (checkStability(xReal-ColPositionX[Step7],yReal-ColPositionY[Step7],0))
			{
				CS_StepN = Step8;
				(*xExpect) = ColPositionX[Step8];
		    (*yExpect) = ColPositionY[Step8];
			}			
			break;
		case Step8:
		  if (checkStability(xReal-ColPositionX[Step8],yReal-ColPositionY[Step8],0))
			{
				CS_StepN = Step9;
				(*xExpect) = ColPositionX[Step9];
		    (*yExpect) = ColPositionY[Step9];
			}			
			break;
		case Step9:
		  if (checkStability(xReal-ColPositionX[Step9],yReal-ColPositionY[Step9],0))
			{
				CS_StepN = Step10;
				(*xExpect) = ColPositionX[Step10];
		    (*yExpect) = ColPositionY[Step10];
			}			
			break;
		case StepAttack:  //干它
			Attack_Task(xExpect,yExpect,angExpect,xReal,yReal,SysCnt,1);
			break;
		default:
			
			break;
		
	}
	return CS_StepN;
}


//行扫描
u8 RowScan(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt)
{
	static u16 last_cnt = 0;
	s32 *RowPositionX;
	s32 *RowPositionY;
	if (ScanPosition == 0)   /*己方侧行扫描*/
	{
		RowPositionX = RowX1;
		RowPositionY = RowY1;
	}
	if (ScanPosition == 1)   /*敌方侧行扫描*/
	{
		RowPositionX = RowX1;  /*横坐标不变*/
		RowPositionY = RowY2;  
	}
	switch (RS_StepN) 
	{
		case Step1:
			(*xExpect) = RowPositionX[Step1];
		  (*yExpect) = RowPositionY[Step1];
		  (*angExpect) = RowAng[ScanPosition];
		  if (checkStability(xReal-RowPositionX[Step1],yReal-RowPositionY[Step1],Yaw-(*angExpect)))
			{
				RS_StepN = Step2;
			}
			break;
		case Step2:
			(*xExpect) = RowPositionX[Step2];
		  (*yExpect) = RowPositionY[Step2];
		  if (checkStability(xReal-RowPositionX[Step2],yReal-RowPositionY[Step2],0))
			{
				if (NumOfWhiteBall)
				{
					PutBall(StepPutWhite,SysCnt);					
				}else
				{
					RS_StepN = Step3;
				}
			}			
			break;
		case Step3:
			if (NumOfBlackBall)
			{
				(*xExpect) = RowPositionX[Step3];
				(*yExpect) = RowPositionY[Step3];
				if (checkStability(xReal-RowPositionX[Step3],yReal-RowPositionY[Step3],0))
				{
					PutBall(StepPutBlack,SysCnt);
				}		
			}
			else
			{
				RS_StepN = Step4;
			}
			break;
		case Step4:
			(*xExpect) = RowPositionX[Step4];
		  (*yExpect) = RowPositionY[Step4];
		  if (checkStability(xReal-RowPositionX[Step4],yReal-RowPositionY[Step4],0))
			{
				if (!ScanPosition)
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_RIGHT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-2,LASER_RIGHT_ANG+2))
					{
						Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step5;
					}
				}else
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
					{
						Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step5;
					}						
				}
			}			
			break;			
		case Step5:
			(*xExpect) = RowPositionX[Step5];
		  (*yExpect) = RowPositionY[Step5];
			(*angExpect) = RowAng[ScanPosition];		  
		  if (checkStability(xReal-RowPositionX[Step5],yReal-RowPositionY[Step5],Yaw-RowAng[ScanPosition]))
			{
				RS_StepN = Step6;
				(*xExpect) = RowPositionX[Step6];
		    (*yExpect) = RowPositionY[Step6];
			}			
			break;
		case Step6:
			MonitorEnemy_Task(1,xReal,yReal,angExpect);
		  if (checkStability(xReal-RowPositionX[Step6],yReal-RowPositionY[Step6],0))
			{
				if (!ScanPosition)
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
					{
						Cali_X.RecFloat = Get_Dis() + LASER_Z_DIS - 240;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step7;
						(*xExpect) = RowPositionX[Step7];
						(*yExpect) = RowPositionY[Step7];
					}
			  }else
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_RIGHT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-2,LASER_RIGHT_ANG+2))
					{
						Cali_X.RecFloat = Get_Dis() + LASER_Z_DIS - 240;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step7;
						(*xExpect) = RowPositionX[Step7];
						(*yExpect) = RowPositionY[Step7];	
					}						
				}
			}			
			break;
		case Step7:
		  if (checkStability(xReal-RowPositionX[Step7],yReal-RowPositionY[Step7],0))
			{
				RS_StepN = Step8;
				(*xExpect) = RowPositionX[Step8];
				(*yExpect) = RowPositionY[Step8];
				
			}			
			break;
		case Step8:
			MonitorEnemy_Task(1,xReal,yReal,angExpect);
		  if (checkStability(xReal-RowPositionX[Step8],yReal-RowPositionY[Step8],0))
			{
				RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
				if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
				{
					Cali_X.RecFloat = -1000.0f;   
					if (!ScanPosition)
					  Cali_Y.RecFloat = Get_Dis() + LASER_H_DIS - 30;
					else
						Cali_Y.RecFloat = 480 - Get_Dis() - LASER_H_DIS - 30;
					GuideCmd();
					RS_StepN = Step9;
				  (*xExpect) = RowPositionX[Step9];
		      (*yExpect) = RowPositionY[Step9];
				}
			}			
			break;
		case Step9:
		  if (checkStability(xReal-RowPositionX[Step9],yReal-RowPositionY[Step9],0))
			{
				if (!ScanPosition)
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_RIGHT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-2,LASER_RIGHT_ANG+2))
					{
						Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS; 
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step10;
						(*xExpect) = RowPositionX[Step10];
						(*yExpect) = RowPositionY[Step10];
						last_cnt = SysCnt;
					}
			  }else
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
					{
						Cali_X.RecFloat = 240 - Get_Dis() + LASER_Z_DIS; 
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
						RS_StepN = Step10;
						(*xExpect) = RowPositionX[Step10];
						(*yExpect) = RowPositionY[Step10];
						last_cnt = SysCnt;
					}						
				}
			}			
			break;
		case Step10:
			MonitorEnemy_Task(1,xReal,yReal,angExpect);
		  RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
		  if (SysCnt - last_cnt >= CALI_INTERVAL_TIME)
			{
				if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
				{
					Cali_X.RecFloat = -1000.0f;   
					if (!ScanPosition)
					  Cali_Y.RecFloat = Get_Dis() + LASER_H_DIS - 30;
					else
						Cali_Y.RecFloat = 480 - Get_Dis() - LASER_H_DIS - 30;
					GuideCmd();
					last_cnt = SysCnt;
				}				
			}
		  if (checkStability(xReal-RowPositionX[Step10],yReal-RowPositionY[Step10],0))
			{
	      RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
				if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
				{
					Cali_X.RecFloat = -1000.0f;   
					if (!ScanPosition)
						Cali_Y.RecFloat = Get_Dis() + LASER_H_DIS - 30;
					else
						Cali_Y.RecFloat = 480 - Get_Dis() - LASER_H_DIS - 30;
					GuideCmd();
					RS_StepN = Step11;
				}			
			}					
			break;
		case Step11:
				if (!ScanPosition)
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
					{
						Cali_X.RecFloat = Get_Dis() + LASER_Z_DIS - 240;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
					  ScanPosition = !ScanPosition; //换一边扫
					  (*angExpect) = -1.57f;
						RS_StepN = Step12;
						last_cnt = SysCnt;
					}
				}else
				{
					RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_RIGHT_ANG);
					if (isInAInterval(LaserRadar.ang,LASER_RIGHT_ANG-2,LASER_RIGHT_ANG+2))
					{
						Cali_X.RecFloat = Get_Dis() + LASER_Z_DIS - 240;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
					  ScanPosition = !ScanPosition; //换一边扫
					  (*angExpect) = 4.71f;
						RS_StepN = Step12;
						last_cnt = SysCnt;
					}						
				}
			
			break;
		case Step12:
			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				(*xExpect) = RowX1[Step1];
				if (!ScanPosition)
					(*yExpect) = RowY1[Step1];
				else
					(*yExpect) = RowY2[Step1];
				RS_StepN = Step13;
			}				
			break;
		case Step13:
			RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_MID_ANG);
		  if (SysCnt - last_cnt >= CALI_INTERVAL_TIME)
			{
				if (isInAInterval(LaserRadar.ang,LASER_MID_ANG-2,LASER_MID_ANG+2))
				{
					Cali_X.RecFloat = Get_Dis() + LASER_H_DIS - 240;  
					Cali_Y.RecFloat = -1000.0f;
					GuideCmd();	
          last_cnt = SysCnt;			
				}
		  }
			if (checkStability(xReal-(*xExpect),yReal-(*yExpect),Yaw-(*angExpect)))
			{
				RS_StepN = Step1;
			}
			break;
		case StepAttack:  //干它
			Attack_Task(xExpect,yExpect,angExpect,xReal,yReal,SysCnt,1);
			break;
		default:
			
			break;
		
	}

	return RS_StepN;
  
}

