#include "manifold.h"
#include "math.h"
#include "can.h"

/**
  * @brief  检查某一个数是否在某一个区间
  * @param  data 数据   
	*         a    左区间
	*         b    右区间
  * @retval data是否属于[a,b]
  */
u8 isInAInterval(float data,float a,float b)
{
	if ((data >= a) && (data <= b)) return 1;
	else                            return 0;
}

/**
  * @brief  妙算数据解码
  * @param  妙算数据共用体指针和数据长度
  * @retval 0 = 解码失败
  *         1 = 解码成功
  */
u8 MANIFOLD_Decode(RecStru *data,u8 Lenth)
{
	u8 i;
	u16 j = 0;
	for (i = 0;i < Lenth;i++)
	{
		data[i].C[0] = RecBuf[j++];
		data[i].C[1] = RecBuf[j++];
		data[i].C[2] = RecBuf[j++];
		data[i].C[3] = RecBuf[j++];
		if (!isInAInterval(data[i].F,-MAX_MANIFOLD_DATA,MAX_MANIFOLD_DATA))
		{
			return 0;
		}
	}
	return 1;
}

/**
  * @brief  是否有敌车
  * @param  妙算数据共用体指针和数据长度
  * @retval 1 = 有敌车
  *         0 = 无敌车     
  */
u8 hasEnemy(RecStru *data,float xReal,float yReal)
{
	if (!(isInAInterval(data[0].F,0.05,0.15)|isInAInterval(data[1].F,0.05,0.15)))
	{
		if (isInAInterval(xReal+data[0].F,-240,240)&isInAInterval(data[1].F,0,480))
			return 1;
	}
  return 0;
}

//总函数，用于处理妙算数据
u8 ProcessMANIFOLD(RecStru *data,
										 float *angExpect,
                     float xReal,s32 *xExpect,
                     float yReal,s32 *yExpect,
                     u16 SystemCounter)
{
	if (MANIFOLD_Decode(data,6))  //解码成功
	{
		hasEnemy(data,xReal,yReal);
	}
	else
	{
		DebugMsgSend(sprintf(TxDebugMsg,
			"Decode failed!\r\n"));
	}
}










