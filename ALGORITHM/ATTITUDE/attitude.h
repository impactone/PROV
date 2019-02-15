#ifndef __MPU6050_DEAL_H__
#define __MPU6050_DEAL_H__


#define Kp     2.00f   //250
#define Ki     0.001f  //0.001
#define halfT  0.001f    //采样周期的一半

static float invSqrt(float number);
void UpdateQ(void);                 //更新四元数
void IMU_GetAngles(float *Yaw);     //获取Yaw轴角度函数

#endif
