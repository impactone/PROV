#include "delay.h"
#include "filter.h"
#include "mpu6050.h"
#include "math.h"
#include "usart.h"

#define AVE 1
#define ACC_FILTER_MODE AVE
#define GYR_FILTER_MODE AVE

float gx_offset,gy_offset,gz_offset; //陀螺仪零点
float ax_offset,ay_offset;           //加速度计零点
#if ACC_FILTER_MODE == AVE
	int16_t ACC_FIFO[3][aveNum_Acc] = {0};  
#endif
#if GYR_FILTER_MODE == AVE
	int16_t GYR_FIFO[3][aveNum_Gyr] = {0};
#endif
#if MAG_FILTER_MODE == AVE
	int16_t MAG_FIFO[3][aveNum_Mag] = {0};
#endif


IMU_S IMU_s;
IMU_F IMU_f;
/***********************************
名称：   获取原始数据函数
功能：   获取陀螺仪,加速度计和磁力计原始数据
输入：   无
输出：   0 读取成功  其他 读取失败
************************************/
u8 ReadIMU(void)
{
	u8 status = 0; 
	  if (MPU_Get_Accelerometer(&IMU_s.ax,&IMU_s.ay,&IMU_s.az))     status |= (1 << 1);
		else
		{
	    IMU_f.ax = (float)IMU_s.ax;
	    IMU_f.ay = (float)IMU_s.ay;
	    IMU_f.az = (float)IMU_s.az;			
		}
	  if (MPU_Get_Gyroscope(&IMU_s.gx,&IMU_s.gy,&IMU_s.gz))         status |= (1 << 2);
		else
		{
			IMU_f.gx = (float)IMU_s.gx;
	    IMU_f.gy = (float)IMU_s.gy;
	    IMU_f.gz = (float)IMU_s.gz;
		}
		
		IMU_f.mx = 0.0f;  //此时不使用磁力计修正
		IMU_f.my = 0.0f;
		IMU_f.mz = 0.0f;
		
//	  if (MPU_Get_Magnetometer(&IMU_s.mx,&IMU_s.my,&IMU_s.mz))      
//		{
//			IMU_f.mx = 0.0f;  //此时不使用磁力计修正
//			IMU_f.my = 0.0f;
//			IMU_f.mz = 0.0f;			
//			status |= (1 << 3);
//		}
//		else
//		{
//			IMU_f.mx = (float)IMU_s.mx;
//			IMU_f.my = (float)IMU_s.my;
//			IMU_f.mz = (float)IMU_s.mz;			
//		}	  
	  return status;
}
/***********************************
名称：   零点确定函数
功能：   测出陀螺仪零点
输入：   无
输出：   无
************************************/
void GyrOffSet(void)
{
	u16 i;
	u32 dataNum = 0;
	float x_temp = 0.0f;
	float y_temp = 0.0f;
	float z_temp = 0.0f;

	for(i=0; i<1000; i++)
	{
		if (!(ReadIMU()&0x02)) ++dataNum;
	  {				
		  x_temp = x_temp + IMU_f.gx;
		  y_temp = y_temp + IMU_f.gy;
		  z_temp = z_temp + IMU_f.gz;
		}
		delay_ms(1);
	}			
		 
	x_temp = x_temp/dataNum;
	y_temp = y_temp/dataNum;	
	z_temp = z_temp/dataNum;
  
	gx_offset = x_temp;
	gy_offset = y_temp;
	gz_offset = z_temp;
 
}

/***********************************
名称：   零点确定函数
功能：   测出加速度计零点
输入：   无
输出：   无
************************************/
void AccOffSet(void)
{
	u16 i;
	u32 dataNum = 0;
	float x_temp = 0.0f;
	float y_temp = 0.0f;
	for(i=0; i<1000; i++)
	{
		if (!(ReadIMU()&0x01)) ++dataNum;
	  {				
		  x_temp = x_temp + IMU_f.ax;
		  y_temp = y_temp + IMU_f.ay;

		}
		delay_ms(1);
	}			
		 
	x_temp = x_temp/dataNum;
	y_temp = y_temp/dataNum;	

	ax_offset = x_temp;
	ay_offset = y_temp;
 
}

/***********************************
名称：   滤波函数
功能：   对陀螺仪,加速度计和磁力计的原始数据进行滤波
输入：   无
输出：   无
************************************/
void IMU_Filter(void){
	u8 status;
	
	status = ReadIMU();
	/*
			status可选值如下： (1表示该传感器数据读取失败)
	    可选值 | 磁力计 | 陀螺仪 | 加速度计
 	      0    |   0   |   0    |    0
	      1    |   0   |   0    |    1
	      2    |   0   |   1    |    0
	      3    |   0   |   1    |    1
	      4    |   1   |   0    |    0
	      5    |   1   |   0    |    1
	      6    |   1   |   1    |    0
	      7    |   1   |   1    |    1
	*/

	switch (status)
	{
			case 0:
			  GyroFilter();
				AccFilter();
				MagFilter();
				break;
			case 1:
			  GyroFilter();
			  MagFilter();			
				break;
			case 2:
				AccFilter();
				MagFilter();
				break;
			case 3:
				MagFilter();
				break;
			case 4:
			  GyroFilter();
				AccFilter();				
				break;
			case 5:
				GyroFilter();
				break;
			case 6:
				AccFilter();
				break;
			case 7:
				
				break;
	}
}

/***********************************
名称：   陀螺仪滤波函数
功能：   对陀螺仪原始数据进行滤波
输入：   无
输出：   无
************************************/
void GyroFilter(void){
      u8 i = 0;
	    float Sum[3] = {0.0f};
		 //陀螺仪去零点
			IMU_f.gx -= gx_offset;
			IMU_f.gy -= gy_offset;
			IMU_f.gz -= gz_offset;
			
#if GYR_FILTER_MODE == AVE
	   //陀螺仪滤波(滑动均值滤波)
      for (i = 1;i < aveNum_Gyr;i++)
		  {
				GYR_FIFO[0][i-1] = GYR_FIFO[0][i];
				GYR_FIFO[1][i-1] = GYR_FIFO[1][i];
				GYR_FIFO[2][i-1] = GYR_FIFO[2][i];
			}
			GYR_FIFO[0][aveNum_Gyr-1] = IMU_f.gx;
			GYR_FIFO[1][aveNum_Gyr-1] = IMU_f.gy;
			GYR_FIFO[2][aveNum_Gyr-1] = IMU_f.gz;
	   
			for (i = 0;i < aveNum_Gyr;i++)
			{
				Sum[0] += GYR_FIFO[0][i];
				Sum[1] += GYR_FIFO[1][i];
				Sum[2] += GYR_FIFO[2][i];
			}
			
			IMU_f.gx = Sum[0] / aveNum_Gyr;
			IMU_f.gy = Sum[1] / aveNum_Gyr;
			IMU_f.gz = Sum[2] / aveNum_Gyr;
#endif
			
#if GYR_FILTER_MODE == LPF2
			//二阶低通滤波
			IMU_f.gx = LPF2pApply_1(IMU_f.gx);
			IMU_f.gy = LPF2pApply_2(IMU_f.gy);
			IMU_f.gz = LPF2pApply_3(IMU_f.gz);
#endif
}

/***********************************
名称：   加速度计滤波函数
功能：   对加速度计原始数据进行滤波
输入：   无
输出：   无
************************************/
void AccFilter(void){
      u8 i = 0;
	    float Sum[3] = {0.0f};  	   
		  static float Preax = 0.0f;
	    static float Preay = 0.0f;
	    static float Preaz = 0.0f;

		 //加速度计去零点
			IMU_f.ax -= ax_offset;
			IMU_f.ay -= ay_offset;	
			
#if ACC_FILTER_MODE == AVE
     //加速度计滤波(滑动均值滤波)
	    for (i = 1;i < aveNum_Acc;i++)
		  {
				ACC_FIFO[0][i-1] = ACC_FIFO[0][i];
				ACC_FIFO[1][i-1] = ACC_FIFO[1][i];
				ACC_FIFO[2][i-1] = ACC_FIFO[2][i];
			}
			ACC_FIFO[0][aveNum_Acc-1] = IMU_f.ax;
			ACC_FIFO[1][aveNum_Acc-1] = IMU_f.ay;
			ACC_FIFO[2][aveNum_Acc-1] = IMU_f.az;
	   
			for (i = 0;i < aveNum_Acc;i++)
			{
				Sum[0] += ACC_FIFO[0][i];
				Sum[1] += ACC_FIFO[1][i];
				Sum[2] += ACC_FIFO[2][i];
			}
			
			IMU_f.ax = Sum[0] / aveNum_Acc;
			IMU_f.ay = Sum[1] / aveNum_Acc;
			IMU_f.az = Sum[2] / aveNum_Acc;
		
      //加速度计滤波(低通滤波器)
			IMU_f.ax = Lpf(Preax,IMU_f.ax);Preax = IMU_f.ax;
			IMU_f.ay = Lpf(Preay,IMU_f.ay);Preay = IMU_f.ay;
			IMU_f.az = Lpf(Preaz,IMU_f.az);Preaz = IMU_f.az;
#endif
			
#if ACC_FILTER_MODE == LPF2
			//二阶低通滤波
			IMU_f.ax = LPF2pApply_4(IMU_f.ax);
			IMU_f.ay = LPF2pApply_5(IMU_f.ay);
			IMU_f.az = LPF2pApply_6(IMU_f.az);
#endif
}

/***********************************
名称：   磁力计滤波函数
功能：   对磁力计原始数据进行滤波
输入：   无
输出：   无
************************************/
void MagFilter(void){
      u8 i = 0;
	    float Sum[3] = {0.0f};

#if MAG_FILTER_MODE == AVE
     //磁力计滤波(滑动均值滤波)
      for (i = 1;i < aveNum_Mag;i++)
		  {
				MAG_FIFO[0][i-1] = MAG_FIFO[0][i];
				MAG_FIFO[1][i-1] = MAG_FIFO[1][i];
				MAG_FIFO[2][i-1] = MAG_FIFO[2][i];
			}
			MAG_FIFO[0][aveNum_Mag-1] = IMU_f.mx;
			MAG_FIFO[1][aveNum_Mag-1] = IMU_f.my;
			MAG_FIFO[2][aveNum_Mag-1] = IMU_f.mz;
	   
			for (i = 0;i < aveNum_Mag;i++)
			{
				Sum[0] += MAG_FIFO[0][i];
				Sum[1] += MAG_FIFO[1][i];
				Sum[2] += MAG_FIFO[2][i];
			}
			
			IMU_f.mx = Sum[0] / aveNum_Mag;
			IMU_f.my = Sum[1] / aveNum_Mag;
			IMU_f.mz = Sum[2] / aveNum_Mag;
#endif
			
#if MAG_FILTER_MODE == LPF2
			//二阶低通滤波
			IMU_f.mx = LPF2pApply_7(IMU_f.mx);
			IMU_f.my = LPF2pApply_8(IMU_f.my);
			IMU_f.mz = LPF2pApply_9(IMU_f.mz);
#endif
	
}

/***********************************
名称：   一阶低通滤波器函数
功能：   一阶低通滤波
输入：   无
输出：   无
************************************/
float Lpf(float PreData,float NewData)
{
	  return (1.0f-lpf_factor)*NewData+lpf_factor*PreData;
}


/***********************************
名称：   二阶低通滤波器初始化函数
功能：   初始化低通滤波器
输入：   无
输出：   无
************************************/
void LPF2pInit(void){
		  LPF2pSetCutoffFreq_1(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_2(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_3(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_4(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_5(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_6(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_7(SAMPLE_FREQ,CUTOFF_FREQ);
	    LPF2pSetCutoffFreq_8(SAMPLE_FREQ,CUTOFF_FREQ);	
			LPF2pSetCutoffFreq_9(SAMPLE_FREQ,CUTOFF_FREQ);
}



 
static float           _cutoff_freq1; 
static float           _a11;
static float           _a21;
static float           _b01;
static float           _b11;
static float           _b21;
static float           _delay_element_11;        // buffered sample -1
static float           _delay_element_21;        // buffered sample -2
void LPF2pSetCutoffFreq_1(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq1 = cutoff_freq;
    if (_cutoff_freq1 > 0.0f) 
		{
				_b01 = ohm*ohm/c;
				_b11 = 2.0f*_b01;
				_b21 = _b01;
				_a11 = 2.0f*(ohm*ohm-1.0f)/c;
				_a21 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_1(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq1 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_11 * _a11 - _delay_element_21 * _a21;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b01 + _delay_element_11 * _b11 + _delay_element_21 * _b21;
				
				_delay_element_21 = _delay_element_11;
				_delay_element_11 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}

static float           _cutoff_freq2; 
static float           _a12;
static float           _a22;
static float           _b02;
static float           _b12;
static float           _b22;
static float           _delay_element_12;        // buffered sample -1
static float           _delay_element_22;        // buffered sample -2
void LPF2pSetCutoffFreq_2(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq2 = cutoff_freq;
    if (_cutoff_freq2 > 0.0f) 
		{
				_b02 = ohm*ohm/c;
				_b12 = 2.0f*_b02;
				_b22 = _b02;
				_a12 = 2.0f*(ohm*ohm-1.0f)/c;
				_a22 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_2(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq2 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_12 * _a12 - _delay_element_22 * _a22;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b02 + _delay_element_12 * _b12 + _delay_element_22 * _b22;
				
				_delay_element_22 = _delay_element_12;
				_delay_element_12 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}

static float           _cutoff_freq3; 
static float           _a13;
static float           _a23;
static float           _b03;
static float           _b13;
static float           _b23;
static float           _delay_element_13;        // buffered sample -1
static float           _delay_element_23;        // buffered sample -2
void LPF2pSetCutoffFreq_3(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq3 = cutoff_freq;
    if (_cutoff_freq3 > 0.0f) 
		{
				_b03 = ohm*ohm/c;
				_b13 = 2.0f*_b03;
				_b23 = _b03;
				_a13 = 2.0f*(ohm*ohm-1.0f)/c;
				_a23 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_3(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq3 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_13 * _a13 - _delay_element_23 * _a23;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b03 + _delay_element_13 * _b13 + _delay_element_23 * _b23;
				
				_delay_element_23 = _delay_element_13;
				_delay_element_13 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}
  
 
static float           _cutoff_freq4; 
static float           _a14;
static float           _a24;
static float           _b04;
static float           _b14;
static float           _b24;
static float           _delay_element_14;        // buffered sample -1
static float           _delay_element_24;        // buffered sample -2
void LPF2pSetCutoffFreq_4(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq4 = cutoff_freq;
    if (_cutoff_freq4 > 0.0f) 
		{
				_b04 = ohm*ohm/c;
				_b14 = 2.0f*_b04;
				_b24 = _b04;
				_a14 = 2.0f*(ohm*ohm-1.0f)/c;
				_a24 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_4(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq4 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_14 * _a14 - _delay_element_24 * _a24;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b04 + _delay_element_14 * _b14 + _delay_element_24 * _b24;
				
				_delay_element_24 = _delay_element_14;
				_delay_element_14 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}
 
 
 
static float           _cutoff_freq5; 
static float           _a15;
static float           _a25;
static float           _b05;
static float           _b15;
static float           _b25;
static float           _delay_element_15;        // buffered sample -1
static float           _delay_element_25;        // buffered sample -2
void LPF2pSetCutoffFreq_5(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq5 = cutoff_freq;
    if (_cutoff_freq5 > 0.0f) 
		{
				_b05 = ohm*ohm/c;
				_b15 = 2.0f*_b05;
				_b25 = _b05;
				_a15 = 2.0f*(ohm*ohm-1.0f)/c;
				_a25 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_5(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq5 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_15 * _a15 - _delay_element_25 * _a25;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b05 + _delay_element_15 * _b15 + _delay_element_25 * _b25;
				
				_delay_element_25 = _delay_element_15;
				_delay_element_15 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}
  
 
 
static float           _cutoff_freq6; 
static float           _a16;
static float           _a26;
static float           _b06;
static float           _b16;
static float           _b26;
static float           _delay_element_16;        // buffered sample -1
static float           _delay_element_26;        // buffered sample -2
void LPF2pSetCutoffFreq_6(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq6 = cutoff_freq;
    if (_cutoff_freq6 > 0.0f) 
		{
				_b06 = ohm*ohm/c;
				_b16 = 2.0f*_b06;
				_b26 = _b06;
				_a16 = 2.0f*(ohm*ohm-1.0f)/c;
				_a26 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_6(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq6 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_16 * _a16 - _delay_element_26 * _a26;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b06 + _delay_element_16 * _b16 + _delay_element_26 * _b26;
				
				_delay_element_26 = _delay_element_16;
				_delay_element_16 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}


static float           _cutoff_freq7; 
static float           _a17;
static float           _a27;
static float           _b07;
static float           _b17;
static float           _b27;
static float           _delay_element_17;        // buffered sample -1
static float           _delay_element_27;        // buffered sample -2
void LPF2pSetCutoffFreq_7(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq7 = cutoff_freq;
    if (_cutoff_freq7 > 0.0f) 
		{
				_b07 = ohm*ohm/c;
				_b17 = 2.0f*_b07;
				_b27 = _b07;
				_a17 = 2.0f*(ohm*ohm-1.0f)/c;
				_a27 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_7(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq7 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_17 * _a17 - _delay_element_27 * _a27;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b07 + _delay_element_17 * _b17 + _delay_element_27 * _b27;
				
				_delay_element_27 = _delay_element_17;
				_delay_element_17 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}


static float           _cutoff_freq8; 
static float           _a18;
static float           _a28;
static float           _b08;
static float           _b18;
static float           _b28;
static float           _delay_element_18;        // buffered sample -1
static float           _delay_element_28;        // buffered sample -2
void LPF2pSetCutoffFreq_8(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq8 = cutoff_freq;
    if (_cutoff_freq8 > 0.0f) 
		{
				_b08 = ohm*ohm/c;
				_b18 = 2.0f*_b08;
				_b28 = _b08;
				_a18 = 2.0f*(ohm*ohm-1.0f)/c;
				_a28 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_8(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq8 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_18 * _a18 - _delay_element_28 * _a28;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b08 + _delay_element_18 * _b18 + _delay_element_28 * _b28;
				
				_delay_element_28 = _delay_element_18;
				_delay_element_18 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}


static float           _cutoff_freq9; 
static float           _a19;
static float           _a29;
static float           _b09;
static float           _b19;
static float           _b29;
static float           _delay_element_19;        // buffered sample -1
static float           _delay_element_29;        // buffered sample -2
void LPF2pSetCutoffFreq_9(float sample_freq, float cutoff_freq)
{
		float fr =0;  
    float ohm =0;
    float c =0;
	
		fr= sample_freq/cutoff_freq;
		ohm=tanf(M_PI_F/fr);
		c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;
	
    _cutoff_freq9 = cutoff_freq;
    if (_cutoff_freq9 > 0.0f) 
		{
				_b09 = ohm*ohm/c;
				_b19 = 2.0f*_b09;
				_b29 = _b09;
				_a19 = 2.0f*(ohm*ohm-1.0f)/c;
				_a29 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
		}
}

float LPF2pApply_9(float sample)
{
	
		float delay_element_0 = 0, output=0;
    if (_cutoff_freq9 <= 0.0f) {
        // no filtering
        return sample;
    }
		else
		{
				delay_element_0 = sample - _delay_element_19 * _a19 - _delay_element_29 * _a29;
				// do the filtering
				if (isnan(delay_element_0) || isinf(delay_element_0)) {
						// don't allow bad values to propogate via the filter
						delay_element_0 = sample;
				}
				output = delay_element_0 * _b09 + _delay_element_19 * _b19 + _delay_element_29 * _b29;
				
				_delay_element_29 = _delay_element_19;
				_delay_element_19 = delay_element_0;

				// return the value.  Should be no need to check limits
				return output;
		}
}



