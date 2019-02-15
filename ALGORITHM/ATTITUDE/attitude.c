#include "filter.h"
#include "math.h"
#include "attitude.h"
#include "usart.h"


float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	       //初始化四元数
float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f; 

//函数名：invSqrt(void)
//描述：求平方根的倒数
//该函数是经典的Carmack求平方根算法，效率极高，使用魔数0x5f375a86
static float invSqrt(float number) 
{
    volatile long i;
    volatile float x, y;
    volatile const float f = 1.5F;

    x = number * 0.5F;
    y = number;
    i = * (( long * ) &y);
    i = 0x5f375a86 - ( i >> 1 );
    y = * (( float * ) &i);
    y = y * ( f - ( x * y * y ) );
    return y;
}


/***********************************
名称：   姿态解算函数
功能：   更新四元数
输入：   无
输出：   无
************************************/
void UpdateQ(void)
{
	
    float invNorm;                 //向量模的倒数
    float vx,vy,vz;                //四元数转换出的重力矢量
		float wx,wy,wz;								 //四元数融合磁力计数据转换出的地磁场矢量
    float ex,ey,ez;								 //误差矢量，由加速度计和四元数转换出的重力矢量叉乘
    float ax,ay,az;                //归一化之后的加速度数据
		float mx,my,mz;								 //归一化之后的地磁场数据
	  float gx,gy,gz;
		float bx,bz;									 //地磁场的两个分量
		float hx,hy,hz;
	
		float	q0q0 = q0 * q0;
	  float q0q1 = q0 * q1;
	  float q0q2 = q0 * q2;
	  float q0q3 = q0 * q3;
	  float q1q1 = q1 * q1;
	  float q1q2 = q1 * q2;
	  float q1q3 = q1 * q3;
	  float q2q2 = q2 * q2;
	  float q2q3 = q2 * q3;
	  float q3q3 = q3 * q3; 
		
		float dq0;
		float dq1;
		float dq2;
    float dq3;
		 
		
		//获取最近一次陀螺仪,加速度计和磁力计数据
		ax = IMU_f.ax;
		ay = IMU_f.ay;
		az = IMU_f.az;
		gx = IMU_f.gx * 0.001064f;   //转换成弧度每秒
    gy = IMU_f.gy * 0.001064f;	 //转换成弧度每秒
		gz = IMU_f.gz * 0.001064f;   //转换成弧度每秒
		mx = IMU_f.mx;
		my = IMU_f.my;
		mz = IMU_f.mz;
 
    //加速度归一化
    invNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax = ax * invNorm;
    ay = ay * invNorm;
    az = az * invNorm;
		//如果磁力计接收到数据的话
    if ((mx != 0.0f) && (my != 0.0f) && (mz != 0.0f))  
		{
			invNorm = invSqrt(mx*mx + my*my + mz*mz);          
			mx = mx * invNorm;
			my = my * invNorm;
			mz = mz * invNorm; 
    
			hx = 2.0f*mx*(0.5f - q2q2 - q3q3) + 2.0f*my*(q1q2 - q0q3) + 2.0f*mz*(q1q3 + q0q2);
			hy = 2.0f*mx*(q1q2 + q0q3) + 2.0f*my*(0.5f - q1q1 - q3q3) + 2.0f*mz*(q2q3 - q0q1);
			hz = 2.0f*mx*(q1q3 - q0q2) + 2.0f*my*(q2q3 + q0q1) + 2.0f*mz*(0.5f - q1q1 - q2q2);         
			bx = sqrt((hx*hx) + (hy*hy));
			bz = hz; 
			
			vx = 2.0f*(q1q3 - q0q2);
			vy = 2.0f*(q0q1 + q2q3);
			vz = q0q0 - q1q1 - q2q2 + q3q3;
			
			wx = 2.0f*bx*(0.5f - q2q2 - q3q3) + 2.0f*bz*(q1q3 - q0q2);
			wy = 2.0f*bx*(q1q2 - q0q3) + 2.0f*bz*(q0q1 + q2q3);
			wz = 2.0f*bx*(q0q2 + q1q3) + 2.0f*bz*(0.5f - q1q1 - q2q2);  

			ex = (ay*vz - az*vy) + (my*wz - mz*wy);
			ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
			ez = (ax*vy - ay*vx) + (mx*wy - my*wx);
		}
		else
		{
			vx = 2.0f*(q1q3 - q0q2);
			vy = 2.0f*(q0q1 + q2q3);
			vz = q0q0 - q1q1 - q2q2 + q3q3;
			
			ex = (ay*vz - az*vy);
			ey = (az*vx - ax*vz);
			ez = (ax*vy - ay*vx);
	
		}
   
    if((ex != 0.0f) && (ey != 0.0f) && (ez != 0.0f))
    {
        exInt = exInt + ex * Ki * halfT;
        eyInt = eyInt + ey * Ki * halfT;	
        ezInt = ezInt + ez * Ki * halfT;
        // 用叉积误差来做PI修正陀螺零偏
        gx = gx + Kp*ex + exInt;
        gy = gy + Kp*ey + eyInt;
        gz = gz + Kp*ez + ezInt;
			
    }
    // 四元数微分方程
    dq0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
    dq1 = q1 + (q0*gx + q2*gz - q3*gy)  * halfT;
    dq2 = q2 + (q0*gy - q1*gz + q3*gx)  * halfT;
    dq3 = q3 + (q0*gz + q1*gy - q2*gx)  * halfT;  

    // 四元数规范化
    invNorm = invSqrt(dq0*dq0 + dq1*dq1 + dq2*dq2 + dq3*dq3);
    q0 = dq0 * invNorm;
    q1 = dq1 * invNorm;
    q2 = dq2 * invNorm;
    q3 = dq3 * invNorm;
}

/***********************************
名称：   获取三轴角度函数
功能：   计算三轴角度
输入：   一个长度为3的数组首地址
输出：   无
************************************/
void IMU_GetAngles(float *yaw)
{
  static int Yaw_Counter = 0;
	static float Yaw_Temp = 0.0f;
	static float lastYaw_Temp = 0.0f;

	(*yaw) = atan2( 2.0f * ( q1*q2 + q0*q3 ),1.0f - 2.0f * ( q2*q2 + q3*q3 ) );       //Yaw
	
	lastYaw_Temp = Yaw_Temp;
	Yaw_Temp = (*yaw);
	if(Yaw_Temp-lastYaw_Temp>=5.526)  //yaw轴角度经过处理后变成连续的
	{
		Yaw_Counter--;
	}
	else if (Yaw_Temp-lastYaw_Temp<=-5.526)
	{
		Yaw_Counter++;
	}	
	(*yaw) = Yaw_Temp + Yaw_Counter * 6.28f;
}

