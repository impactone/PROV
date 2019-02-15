#include "magnet.h"
#include "task.h"

void Mag_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);	

   GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_4;
	 GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
	 MAG_A = 0;
	 MAG_B = 0;
	 MAG_C = 0;
	 MAG_D = 0;
	
}

/*抬板*/
void RaiseUp(void)
{
	 MAG_C = 1;
	 MAG_D = 1;	
}

/*放板*/
void PutDown(void)
{
	 MAG_C = 0;
	 MAG_D = 0;	
}

u8 PushWhiteOnce(u16 SysCnt)
{
	static u16 last_cnt = 0;
	static u8 cylinder_status = 1;  //气缸状态 1为里面 2为向外推ing 3为往里推ing
	switch (cylinder_status)
	{
		case 1:
			MAG_A = 1;
		  last_cnt = SysCnt;
		  cylinder_status = 2;
			break;
		case 2:
			if (SysCnt - last_cnt > PUT_BALL_TIME)
			{
				MAG_A = 0;
				cylinder_status = 3;
				last_cnt = SysCnt;
			}
			break;
		case 3:
			if (SysCnt - last_cnt > PUT_BALL_TIME)
			{
				cylinder_status = 1;
				return 1;
			}
			break;
		default:
			break;
	}
	return 0;
}

u8 PushWhite(u8 times,u16 SysCnt)
{
	static u16 last_cnt = 0;
	static u16 times_status = 1;
  
  if (times == 1)
	{
		if (PushWhiteOnce(SysCnt)) 
			return 1;
	}else
	{
		switch (times_status)
		{
			case 1:
				if (PushWhiteOnce(SysCnt))
				{
					times_status = 2;
				  last_cnt = SysCnt;
				}
				break;
			case 2:
				if (SysCnt - last_cnt > PUT_BALL_WAIT_TIME)
				{
					times_status = 3;
					PushWhiteOnce(SysCnt);  /*小小的加个速*/
				}
				break;
			case 3:
				if (PushWhiteOnce(SysCnt))
				{
					times_status = 1;
					return 1;
				}
			default:
				break;
		}
	}
	return 0;
}

u8 PushBlackOnce(u16 SysCnt)
{
	static u16 last_cnt = 0;
	static u8 cylinder_status = 1;  //气缸状态 1为里面 2为向外推ing 3为往里推ing
	switch (cylinder_status)
	{
		case 1:
			MAG_B = 1;
		  last_cnt = SysCnt;
		  cylinder_status = 2;
			break;
		case 2:
			if (SysCnt - last_cnt > PUT_BALL_TIME)
			{
				MAG_B = 0;
				cylinder_status = 3;
				last_cnt = SysCnt;
			}
			break;
		case 3:
			if (SysCnt - last_cnt > PUT_BALL_TIME)
			{
				cylinder_status = 1;
				return 1;
			}
			break;
		default:
			break;
	}
	return 0;
}

u8 PushBlack(u8 times,u16 SysCnt)
{
	static u16 last_cnt = 0;
	static u16 times_status = 1;
  
  if (times == 1)
	{
		if (PushBlackOnce(SysCnt)) 
			return 1;
	}else
	{
		switch (times_status)
		{
			case 1:
				if (PushBlackOnce(SysCnt))
				{
					times_status = 2;
				  last_cnt = SysCnt;
				}
				break;
			case 2:
				if (SysCnt - last_cnt > PUT_BALL_WAIT_TIME)
				{
					times_status = 3;
					PushBlackOnce(SysCnt);  /*小小的加个速*/
				}
				break;
			case 3:
				if (PushBlackOnce(SysCnt))
				{
					times_status = 1;
					return 1;
				}
			default:
				break;
		}
	}
	return 0;
}

