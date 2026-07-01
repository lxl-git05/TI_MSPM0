#ifndef __MYENCODER_H
#define __MYENCODER_H

/* TI平台编码器使用GPIO外部中断(双边沿) */

#include "ti_msp_dl_config.h"

typedef struct
{
	GPIO_Regs *Encoder_GPIO_Port;
	uint32_t Encoder_Pin_1;
	uint32_t Encoder_Pin_2;
	IRQn_Type Encoder_IRQN;
	uint32_t time_Fre ;		// 倍频数(2倍频,4倍频等等)
	volatile int32_t cnt ;	// 本次cnt
	int32_t total_cnt ;		// 总cnt
}MyEncoder_Typedef;

extern MyEncoder_Typedef Motor_A_Encoder ;
extern MyEncoder_Typedef Motor_B_Encoder ;

// 1. 编码器初始化
void MyEncoder_Init(MyEncoder_Typedef* MyEncoder) ;

// 1.1 编码器中断计数更新(放在中断中调用)
void MyEncoder_Counter_Tick(MyEncoder_Typedef* MyEncoder) ;

// 2. 得到编码器的脉冲数
int MyEncoder_Get_CNT(MyEncoder_Typedef* MyEncoder) ;

// 3. 得到累计脉冲数
int MyEncoder_Get_Total_CNT(MyEncoder_Typedef* MyEncoder) ;

// 4. 清除累计脉冲数
void MyEncoder_Total_Cnt_Clear(MyEncoder_Typedef* MyEncoder) ;

#endif
