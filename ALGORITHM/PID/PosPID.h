#ifndef __POSPID_H
#define __POSPID_H

#include "sys.h"
#include "stdlib.h"

#define pwmMax 5000   //PID输出限幅值
#define T      0.002f //PID控制周期

//位置式PID结构体
typedef struct {
  float expectation;          
  float Err_k;			  
  float Err_k_1;		  
  float Err_k_2;		 
  float SumErr;             
  float Kp;				 
  float Ti;				
  float Td;				 
  float Ouput_deltaUk;		
  float Ouput_deltaUk_Max;		
  float Ouput_deltaUk_Min;		
  float PID_Integral_Max;				
  float PID_Integral_Min;				
}Pos_PID;

//拓展出去，方便上位机调用
extern Pos_PID PID_BaseControl_V_Struct1;
extern Pos_PID PID_BaseControl_V_Struct2;
extern Pos_PID PID_BaseControl_V_Struct3;	
extern Pos_PID PID_RotateControl;
extern Pos_PID PID_XControl;
extern Pos_PID PID_YControl;

void Attitude_posPID_Init(Pos_PID *PID,u16 I_Max); //姿态PID参数初始化函数
void posPID_Init_Struct(void);           //姿态PID初始参数设定函数
void Set_posPID(Pos_PID *pid,float p,float i,float d);                //PID参数设定函数,主要给上位机用
float posPID_Calculate(Pos_PID * PID, float measured, float expect);  //位置式PID计算函数
float posPID_Calculate2(Pos_PID * PID, float measured, float expect); //位置式PID计算函数(2.0)
float PID_BaseControl_Calculate(Pos_PID *PID , float measured ,float expect );

#endif




