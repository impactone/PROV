#include "manifold.h"
#include "math.h"
#include "can.h"

/**
  * @brief  ���ĳһ�����Ƿ���ĳһ������
  * @param  data ����   
	*         a    ������
	*         b    ������
  * @retval data�Ƿ�����[a,b]
  */
u8 isInAInterval(float data,float a,float b)
{
	if ((data >= a) && (data <= b)) return 1;
	else                            return 0;
}

/**
  * @brief  �������ݽ���
  * @param  �������ݹ�����ָ������ݳ���
  * @retval 0 = ����ʧ��
  *         1 = ����ɹ�
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
  * @brief  �Ƿ��ег�
  * @param  �������ݹ�����ָ������ݳ���
  * @retval 1 = �ег�
  *         0 = �޵г�     
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

//�ܺ��������ڴ�����������
u8 ProcessMANIFOLD(RecStru *data,
										 float *angExpect,
                     float xReal,s32 *xExpect,
                     float yReal,s32 *yExpect,
                     u16 SystemCounter)
{
	if (MANIFOLD_Decode(data,6))  //����ɹ�
	{
		hasEnemy(data,xReal,yReal);
	}
	else
	{
		DebugMsgSend(sprintf(TxDebugMsg,
			"Decode failed!\r\n"));
	}
}










