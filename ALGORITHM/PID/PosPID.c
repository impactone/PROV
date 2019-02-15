#include "PosPID.h"
#include "usart.h"
#include "Config.h"

//底盘PID变量声明
Pos_PID PID_BaseControl_V_Struct1;
Pos_PID PID_BaseControl_V_Struct2;
Pos_PID PID_BaseControl_V_Struct3;			    

//角度闭环PID
Pos_PID PID_RotateControl;

//近距离坐标闭环PID
Pos_PID PID_XControl;
Pos_PID PID_YControl;

//底盘PID计算周期
#define Base_T 0.01f

//底盘速度闭环PID参数
#define Base_V_P 3.3f   //3.0
#define Base_V_I 0.35f  //0.08  0.2
#define Base_V_D 0.0001f   //0.0

#define Base_V_Kp Base_V_P
#define Base_V_Ti Base_V_P*(Base_T/Base_V_I)  
#define Base_V_Td Base_V_P*(Base_V_D/Base_T) 

/***********************************
名称：   姿态PID参数初始化函数
功能：   使输入的PID各参数置零
输入：   一个位置式PID结构体
输出：   无
************************************/
void Attitude_posPID_Init(Pos_PID *PID,u16 I_Max)
{
  PID->expectation        = 0.0;            //遥控给的期望值
  PID->Err_k			        = 0.0;            //当前误差值e(k)
  PID->Err_k_1		        = 0.0;            //k-1时刻误差值e(k-1)
  PID->Err_k_2	          = 0.0;            //k-2时刻误差值e(k-2)
  PID->SumErr             = 0.0;		        //误差和
  PID->Kp				          = 0.0;            //比例系数，通过串口在线调PID参数再写入Flash
  PID->Ti				          = 0.0;            //积分系数，通过串口在线调PID参数再写入Flash
  PID->Td				          = 0.0;            //微分系数，通过串口在线调PID参数再写入Flash
  PID->Ouput_deltaUk      = 0.0;		        //PID计算后的输出量U(k) - U(k-1)
  PID->Ouput_deltaUk_Max  = pwmMax;		      //限制输出量最大值
  PID->Ouput_deltaUk_Min  = -pwmMax;        //限制输出量最小值
  PID->PID_Integral_Max   = I_Max;		      //限制积分项最大值
  PID->PID_Integral_Min   = -I_Max;			  //限制积分项最小值
}

/***********************************
名称：   姿态PID初始参数设定函数
功能：   设定所有PID的初始参数
输入：   无
输出：   无
************************************/
void posPID_Init_Struct(void)
{
	
	Attitude_posPID_Init(&PID_BaseControl_V_Struct1,900);
	Attitude_posPID_Init(&PID_BaseControl_V_Struct2,900);
	Attitude_posPID_Init(&PID_BaseControl_V_Struct3,900);


	Attitude_posPID_Init(&PID_RotateControl,65000); //5000 
	Attitude_posPID_Init(&PID_XControl,2000);
	Attitude_posPID_Init(&PID_YControl,2000);	
	
  Set_posPID(&PID_BaseControl_V_Struct1,Base_V_Kp,Base_V_Ti,Base_V_Td);
	Set_posPID(&PID_BaseControl_V_Struct2,Base_V_Kp,Base_V_Ti,Base_V_Td);
	Set_posPID(&PID_BaseControl_V_Struct3,Base_V_Kp,Base_V_Ti,Base_V_Td);

  Set_posPID(&PID_RotateControl,3900.0f,6.5f,0.30f);  //70 0 0.03 || 4000 1.8 0.3
	Set_posPID(&PID_XControl,200.0f,0.0f,1.3f);
	Set_posPID(&PID_YControl,200.0f,0.0f,1.3f);
	
}

/***********************************
名称：   PID参数设定函数
功能：   设定给定PID结构体的参数
输入：   一个位置式结构体和P,I,D的值
输出：   无
************************************/
void Set_posPID(Pos_PID *pid,float p,float i,float d)
{
		pid->Kp = p;
	  pid->Ti = i;
	  pid->Td = d;
}

/***********************************
名称：   位置式PID计算函数
功能：   计算PID输出值
输入：   一个位置式PID结构体，测量值，期望值
输出：   PID计算结果
************************************/
float posPID_Calculate(Pos_PID * PID, float measured, float expect)
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	

  PID->expectation = expect;
  PID->Err_k   = PID->expectation - measured;
	
	if (abs(PID->Err_k) <= 0.007)
  {		
		Set_posPID(&PID_RotateControl,3900.0f,0.0f,0.30f);
		PID->SumErr = 0;
	}
	else
		Set_posPID(&PID_RotateControl,3900.0f,6.5f,0.30f);
	
  PID->SumErr += PID->Err_k;

  //计算位置式PID的三项
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);

	//对积分项进行限幅，防爆
  if(Value_Integral > PID->PID_Integral_Max)
  {
     PID->SumErr -= PID->Err_k;
	   Value_Integral = PID->PID_Integral_Max;
  }
  if(Value_Integral < PID->PID_Integral_Min)
  {
  	 PID->SumErr -= PID->Err_k;
     Value_Integral = PID->PID_Integral_Min;
  }
  
	//计算PID输出值
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;

	//对输出进行限幅
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }

	//保存k-1次误差值
  PID->Err_k_1 = PID->Err_k;	  
  
	//数据返回
  return PID->Ouput_deltaUk;
}

/***********************************
名称：   位置式PID计算函数(2.0)
功能：   计算PID输出值
输入：   一个位置式PID结构体，测量值，期望值
输出：   PID计算结果
************************************/
float posPID_Calculate2(Pos_PID * PID, float measured, float expect)
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	

  PID->expectation = expect;
  PID->Err_k   = PID->expectation - measured;
	
  PID->SumErr += PID->Err_k;

  //计算位置式PID的三项
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);

	//对积分项进行限幅，防爆
  if(Value_Integral > PID->PID_Integral_Max)
  {
     PID->SumErr -= PID->Err_k;
	   Value_Integral = PID->PID_Integral_Max;
  }
  if(Value_Integral < PID->PID_Integral_Min)
  {
  	 PID->SumErr -= PID->Err_k;
     Value_Integral = PID->PID_Integral_Min;
  }
  
	//计算PID输出值
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;
  
	
	//对输出进行限幅
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }

	//保存k-1次误差值
  PID->Err_k_1 = PID->Err_k;	  
  
	//数据返回
  return PID->Ouput_deltaUk;
}

/***********************************
名称：   位置式PID计算函数
功能：   计算底盘电机PID输出值
输入：   一个位置式PID结构体，测量值，期望值
输出：   PID计算结果
************************************/
float  PID_BaseControl_Calculate(Pos_PID *PID , float measured ,float expect )
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	
  
	/*************对PID参数进行处理***************/
 
	
  PID->expectation = expect;             //给期望值赋值
  PID->Err_k   = (PID->expectation - measured) ;      
  PID->SumErr += PID->Err_k;

	  //计算位置式PID的三项
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);
	
	
	
	//对积分项进行限幅，防爆

  if(Value_Integral > PID->PID_Integral_Max)
  {
     PID->SumErr -= PID->Err_k;
	   Value_Integral = PID->PID_Integral_Max;
  }
  if(Value_Integral < PID->PID_Integral_Min)
  {
  	 PID->SumErr -= PID->Err_k;
     Value_Integral = PID->PID_Integral_Min;
  }
  
	//计算PID输出值
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;

	//对输出进行限幅
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }
 
	//保存k-1次误差值
  PID->Err_k_1 = PID->Err_k;	  
  
	//数据返回
  return PID->Ouput_deltaUk;	
}
