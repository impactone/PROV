#include "filter.h"
#include "math.h"
#include "attitude.h"
#include "usart.h"


float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	       //��ʼ����Ԫ��
float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f; 

//��������invSqrt(void)
//��������ƽ�����ĵ���
//�ú����Ǿ����Carmack��ƽ�����㷨��Ч�ʼ��ߣ�ʹ��ħ��0x5f375a86
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
���ƣ�   ��̬���㺯��
���ܣ�   ������Ԫ��
���룺   ��
�����   ��
************************************/
void UpdateQ(void)
{
	
    float invNorm;                 //����ģ�ĵ���
    float vx,vy,vz;                //��Ԫ��ת����������ʸ��
		float wx,wy,wz;								 //��Ԫ���ںϴ���������ת�����ĵشų�ʸ��
    float ex,ey,ez;								 //���ʸ�����ɼ��ٶȼƺ���Ԫ��ת����������ʸ�����
    float ax,ay,az;                //��һ��֮��ļ��ٶ�����
		float mx,my,mz;								 //��һ��֮��ĵشų�����
	  float gx,gy,gz;
		float bx,bz;									 //�شų�����������
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
		 
		
		//��ȡ���һ��������,���ٶȼƺʹ���������
		ax = IMU_f.ax;
		ay = IMU_f.ay;
		az = IMU_f.az;
		gx = IMU_f.gx * 0.001064f;   //ת���ɻ���ÿ��
    gy = IMU_f.gy * 0.001064f;	 //ת���ɻ���ÿ��
		gz = IMU_f.gz * 0.001064f;   //ת���ɻ���ÿ��
		mx = IMU_f.mx;
		my = IMU_f.my;
		mz = IMU_f.mz;
 
    //���ٶȹ�һ��
    invNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax = ax * invNorm;
    ay = ay * invNorm;
    az = az * invNorm;
		//��������ƽ��յ����ݵĻ�
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
        // �ò���������PI����������ƫ
        gx = gx + Kp*ex + exInt;
        gy = gy + Kp*ey + eyInt;
        gz = gz + Kp*ez + ezInt;
			
    }
    // ��Ԫ��΢�ַ���
    dq0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
    dq1 = q1 + (q0*gx + q2*gz - q3*gy)  * halfT;
    dq2 = q2 + (q0*gy - q1*gz + q3*gx)  * halfT;
    dq3 = q3 + (q0*gz + q1*gy - q2*gx)  * halfT;  

    // ��Ԫ���淶��
    invNorm = invSqrt(dq0*dq0 + dq1*dq1 + dq2*dq2 + dq3*dq3);
    q0 = dq0 * invNorm;
    q1 = dq1 * invNorm;
    q2 = dq2 * invNorm;
    q3 = dq3 * invNorm;
}

/***********************************
���ƣ�   ��ȡ����ǶȺ���
���ܣ�   ��������Ƕ�
���룺   һ������Ϊ3�������׵�ַ
�����   ��
************************************/
void IMU_GetAngles(float *yaw)
{
  static int Yaw_Counter = 0;
	static float Yaw_Temp = 0.0f;
	static float lastYaw_Temp = 0.0f;

	(*yaw) = atan2( 2.0f * ( q1*q2 + q0*q3 ),1.0f - 2.0f * ( q2*q2 + q3*q3 ) );       //Yaw
	
	lastYaw_Temp = Yaw_Temp;
	Yaw_Temp = (*yaw);
	if(Yaw_Temp-lastYaw_Temp>=5.526)  //yaw��ǶȾ����������������
	{
		Yaw_Counter--;
	}
	else if (Yaw_Temp-lastYaw_Temp<=-5.526)
	{
		Yaw_Counter++;
	}	
	(*yaw) = Yaw_Temp + Yaw_Counter * 6.28f;
}

