#include "MyEncoder.h"

// 1. 编码器初始化
void MyEncoder_Init(MyEncoder_Typedef* MyEncoder)
{
	if ((MyEncoder == 0) || (MyEncoder->Encoder_GPIO_Port == 0) ||
	    (MyEncoder->Encoder_Pin_1 == 0U) || (MyEncoder->Encoder_Pin_2 == 0U))
	{
		return;
	}

	NVIC_ClearPendingIRQ(MyEncoder->Encoder_IRQN);
	NVIC_EnableIRQ(MyEncoder->Encoder_IRQN);
}

// 1.1 更新编码器中断计数(双边沿触发)
void MyEncoder_Counter_Tick(MyEncoder_Typedef* MyEncoder)
{
	uint32_t pinA;
	uint32_t pinB;

	if ((MyEncoder == 0) || (MyEncoder->Encoder_GPIO_Port == 0))
	{
		return;
	}

	if (DL_GPIO_getEnabledInterruptStatus(MyEncoder->Encoder_GPIO_Port, MyEncoder->Encoder_Pin_1) != MyEncoder->Encoder_Pin_1)
	{
		return;
	}

	pinA = (DL_GPIO_readPins(MyEncoder->Encoder_GPIO_Port, MyEncoder->Encoder_Pin_1) != 0U) ? 1U : 0U;
	pinB = (DL_GPIO_readPins(MyEncoder->Encoder_GPIO_Port, MyEncoder->Encoder_Pin_2) != 0U) ? 1U : 0U;

	if (pinA == pinB)
	{
		MyEncoder->cnt++;
	}
	else
	{
		MyEncoder->cnt--;
	}

	DL_GPIO_clearInterruptStatus(MyEncoder->Encoder_GPIO_Port, MyEncoder->Encoder_Pin_1);
}

// 2. 得到编码器的脉冲数
int MyEncoder_Get_CNT(MyEncoder_Typedef* MyEncoder)
{
	int32_t cnt;

	if (MyEncoder == 0)
	{
		return 0;
	}

	cnt = MyEncoder->cnt;
	MyEncoder->cnt = 0;
	MyEncoder->total_cnt += cnt;

	// 返回本次周期的计数值
	return (int)cnt;
}

// 3. 得到累计脉冲数
int MyEncoder_Get_Total_CNT(MyEncoder_Typedef* MyEncoder)
{
	if (MyEncoder == 0)
	{
		return 0;
	}
	return (int)MyEncoder->total_cnt;
}

// 4. 清除累计脉冲数
void MyEncoder_Total_Cnt_Clear(MyEncoder_Typedef* MyEncoder)
{
	if (MyEncoder == 0)
	{
		return;
	}
	MyEncoder->cnt = 0;
	MyEncoder->total_cnt = 0 ;
}
