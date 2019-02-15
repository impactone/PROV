#ifndef __POSPID_H
#define __POSPID_H

#include "sys.h"
#include "stdlib.h"

#define pwmMax 5000   //PID����޷�ֵ
#define T      0.002f //PID��������

//λ��ʽPID�ṹ��
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

//��չ��ȥ��������λ������
extern Pos_PID PID_BaseControl_V_Struct1;
extern Pos_PID PID_BaseControl_V_Struct2;
extern Pos_PID PID_BaseControl_V_Struct3;	
extern Pos_PID PID_RotateControl;
extern Pos_PID PID_XControl;
extern Pos_PID PID_YControl;

void Attitude_posPID_Init(Pos_PID *PID,u16 I_Max); //��̬PID������ʼ������
void posPID_Init_Struct(void);           //��̬PID��ʼ�����趨����
void Set_posPID(Pos_PID *pid,float p,float i,float d);                //PID�����趨����,��Ҫ����λ����
float posPID_Calculate(Pos_PID * PID, float measured, float expect);  //λ��ʽPID���㺯��
float posPID_Calculate2(Pos_PID * PID, float measured, float expect); //λ��ʽPID���㺯��(2.0)
float PID_BaseControl_Calculate(Pos_PID *PID , float measured ,float expect );

#endif




