#include "routine.h"
#include "bsp.h"

s32 Routine1_X[] = {0,0,0,0,0,0,0};
s32 Routine1_Y[] = {0,0,0,0,0,0,0};
void Routine1(float xReal,s32 *xExpect,float yReal,s32 *yExpect,float *angExpect,u16 SysCnt)
{
	static enum Step RO_Step = Step1;
	switch (RO_Step)
	{
		case Step1:
			(*xExpect) = Routine1_X[Step1];
		  (*yExpect) = Routine1_Y[Step1];
		  RoboModule_PWM_Velocity_Position_Mode(0,2,LASER_GUIDE_SPEED,LASER_LEFT_ANG);
		  if (checkStability((*xExpect)-Routine1_X[Step1],(*yExpect)-Routine1_Y[Step1],0))
			{
				RO_Step = Step2;
			}
			break;
		case Step2:
			(*xExpect) = Routine1_X[Step2];
		  (*yExpect) = Routine1_Y[Step2];
		  if (checkStability((*xExpect)-Routine1_X[Step2],(*yExpect)-Routine1_Y[Step2],0))
			{
				if (NumOfWhiteBall)
				{
					PutBall(StepPutWhite,SysCnt);
				}else
				{
				  RO_Step = Step3;
				}
			}		
			break;
		case Step3:
			(*xExpect) = Routine1_X[Step3];
		  (*yExpect) = Routine1_Y[Step3];
		  if (checkStability((*xExpect)-Routine1_X[Step3],(*yExpect)-Routine1_Y[Step3],0))
			{
				if (isInAInterval(LaserRadar.ang,LASER_LEFT_ANG-2,LASER_LEFT_ANG+2))
				{
					
						Cali_X.RecFloat = Get_Dis() + LASER_Z_DIS - 240;  
						Cali_Y.RecFloat = -1000.0f;
						GuideCmd();
					  RO_Step = Step4;
				}
			}				
			break;
		case Step4:
			(*xExpect) = Routine1_X[Step4];
		  (*yExpect) = Routine1_Y[Step4];	
      if (checkStability((*xExpect)-Routine1_X[Step4],(*yExpect)-Routine1_Y[Step4],0))
			{
				RO_Step = Step5;
			}				
			break;
		case Step5:
			(*angExpect) = 3.14f;
		  if (checkStability((*xExpect)-xReal,(*yExpect)-yReal,Yaw-(*angExpect)))
			{
				if (NumOfBlackBall)
				{
					PutBall(StepPutBlack,SysCnt);
				}else
				{
				  RO_Step = Step6;
				}				
			}
			break;
		default:
			break;
	}
}

