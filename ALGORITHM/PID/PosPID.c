#include "PosPID.h"
#include "usart.h"
#include "Config.h"

//����PID��������
Pos_PID PID_BaseControl_V_Struct1;
Pos_PID PID_BaseControl_V_Struct2;
Pos_PID PID_BaseControl_V_Struct3;			    

//�Ƕȱջ�PID
Pos_PID PID_RotateControl;

//����������ջ�PID
Pos_PID PID_XControl;
Pos_PID PID_YControl;

//����PID��������
#define Base_T 0.01f

//�����ٶȱջ�PID����
#define Base_V_P 3.3f   //3.0
#define Base_V_I 0.35f  //0.08  0.2
#define Base_V_D 0.0001f   //0.0

#define Base_V_Kp Base_V_P
#define Base_V_Ti Base_V_P*(Base_T/Base_V_I)  
#define Base_V_Td Base_V_P*(Base_V_D/Base_T) 

/***********************************
���ƣ�   ��̬PID������ʼ������
���ܣ�   ʹ�����PID����������
���룺   һ��λ��ʽPID�ṹ��
�����   ��
************************************/
void Attitude_posPID_Init(Pos_PID *PID,u16 I_Max)
{
  PID->expectation        = 0.0;            //ң�ظ�������ֵ
  PID->Err_k			        = 0.0;            //��ǰ���ֵe(k)
  PID->Err_k_1		        = 0.0;            //k-1ʱ�����ֵe(k-1)
  PID->Err_k_2	          = 0.0;            //k-2ʱ�����ֵe(k-2)
  PID->SumErr             = 0.0;		        //����
  PID->Kp				          = 0.0;            //����ϵ����ͨ���������ߵ�PID������д��Flash
  PID->Ti				          = 0.0;            //����ϵ����ͨ���������ߵ�PID������д��Flash
  PID->Td				          = 0.0;            //΢��ϵ����ͨ���������ߵ�PID������д��Flash
  PID->Ouput_deltaUk      = 0.0;		        //PID�����������U(k) - U(k-1)
  PID->Ouput_deltaUk_Max  = pwmMax;		      //������������ֵ
  PID->Ouput_deltaUk_Min  = -pwmMax;        //�����������Сֵ
  PID->PID_Integral_Max   = I_Max;		      //���ƻ��������ֵ
  PID->PID_Integral_Min   = -I_Max;			  //���ƻ�������Сֵ
}

/***********************************
���ƣ�   ��̬PID��ʼ�����趨����
���ܣ�   �趨����PID�ĳ�ʼ����
���룺   ��
�����   ��
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
���ƣ�   PID�����趨����
���ܣ�   �趨����PID�ṹ��Ĳ���
���룺   һ��λ��ʽ�ṹ���P,I,D��ֵ
�����   ��
************************************/
void Set_posPID(Pos_PID *pid,float p,float i,float d)
{
		pid->Kp = p;
	  pid->Ti = i;
	  pid->Td = d;
}

/***********************************
���ƣ�   λ��ʽPID���㺯��
���ܣ�   ����PID���ֵ
���룺   һ��λ��ʽPID�ṹ�壬����ֵ������ֵ
�����   PID������
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

  //����λ��ʽPID������
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);

	//�Ի���������޷�������
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
  
	//����PID���ֵ
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;

	//����������޷�
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }

	//����k-1�����ֵ
  PID->Err_k_1 = PID->Err_k;	  
  
	//���ݷ���
  return PID->Ouput_deltaUk;
}

/***********************************
���ƣ�   λ��ʽPID���㺯��(2.0)
���ܣ�   ����PID���ֵ
���룺   һ��λ��ʽPID�ṹ�壬����ֵ������ֵ
�����   PID������
************************************/
float posPID_Calculate2(Pos_PID * PID, float measured, float expect)
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	

  PID->expectation = expect;
  PID->Err_k   = PID->expectation - measured;
	
  PID->SumErr += PID->Err_k;

  //����λ��ʽPID������
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);

	//�Ի���������޷�������
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
  
	//����PID���ֵ
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;
  
	
	//����������޷�
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }

	//����k-1�����ֵ
  PID->Err_k_1 = PID->Err_k;	  
  
	//���ݷ���
  return PID->Ouput_deltaUk;
}

/***********************************
���ƣ�   λ��ʽPID���㺯��
���ܣ�   ������̵��PID���ֵ
���룺   һ��λ��ʽPID�ṹ�壬����ֵ������ֵ
�����   PID������
************************************/
float  PID_BaseControl_Calculate(Pos_PID *PID , float measured ,float expect )
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	
  
	/*************��PID�������д���***************/
 
	
  PID->expectation = expect;             //������ֵ��ֵ
  PID->Err_k   = (PID->expectation - measured) ;      
  PID->SumErr += PID->Err_k;

	  //����λ��ʽPID������
  Value_Proportion    =  PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative    =  PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);
	
	
	
	//�Ի���������޷�������

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
  
	//����PID���ֵ
  PID->Ouput_deltaUk = Value_Proportion + Value_Integral + Value_Derivative;

	//����������޷�
  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {
			PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
	}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {
      PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
  }
 
	//����k-1�����ֵ
  PID->Err_k_1 = PID->Err_k;	  
  
	//���ݷ���
  return PID->Ouput_deltaUk;	
}
