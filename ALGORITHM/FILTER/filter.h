#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"

#define aveNum_Acc 10
#define aveNum_Gyr 10
#define aveNum_Mag 10

#define M_PI_F 3.1415926f   //PI
#define T_Cal  0.002f       //�������� 2msһ��
#define Freq   300          //һ�׵�ͨ�˲�Ƶ�� ��λ��Hz
#define lpf_factor T_Cal/(T_Cal + 1/(2*M_PI_F*Freq))  //�˲����Ӽ���  


void GyrOffSet(void);        //���������ȷ������
void AccOffSet(void);        //���ٶ����ȷ������
void IMU_Filter(void);       //�˲�����
void GyroFilter(void);       //�������˲�
void AccFilter(void);        //���ٶ��˲�
void MagFilter(void);        //�������˲�
float Lpf(float PreData,float NewData);  //һ�׵�ͨ�˲���

typedef struct IMUs
{
		short ax;
	  short ay;
	  short az;
	  short gx;
	  short gy;
	  short gz;
	  short mx;
	  short my;
	  short mz;
}IMU_S;

typedef struct IMUf
{
		float ax;
	  float ay;
	  float az;
	  float gx;
	  float gy;
	  float gz;
	  float mx;
	  float my;
	  float mz;
}IMU_F;

extern IMU_F IMU_f;



#define SAMPLE_FREQ 300.0f    //��ͨƵ��
#define CUTOFF_FREQ 1000.0f   //����Ƶ��
void LPF2pInit(void);        //���׵�ͨ�˲�����ʼ������
void LPF2pSetCutoffFreq_1(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_2(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_3(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_4(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_5(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_6(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_7(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_8(float sample_freq, float cutoff_freq);
void LPF2pSetCutoffFreq_9(float sample_freq, float cutoff_freq);
float LPF2pApply_1(float sample);
float LPF2pApply_2(float sample);
float LPF2pApply_3(float sample);
float LPF2pApply_4(float sample);
float LPF2pApply_5(float sample);
float LPF2pApply_6(float sample);
float LPF2pApply_7(float sample);
float LPF2pApply_8(float sample);
float LPF2pApply_9(float sample);


#endif


