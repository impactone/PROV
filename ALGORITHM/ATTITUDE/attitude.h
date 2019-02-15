#ifndef __MPU6050_DEAL_H__
#define __MPU6050_DEAL_H__


#define Kp     2.00f   //250
#define Ki     0.001f  //0.001
#define halfT  0.001f    //�������ڵ�һ��

static float invSqrt(float number);
void UpdateQ(void);                 //������Ԫ��
void IMU_GetAngles(float *Yaw);     //��ȡYaw��ǶȺ���

#endif
