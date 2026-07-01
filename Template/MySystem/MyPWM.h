#ifndef __MyPWM_H
#define __MyPWM_H

#include "ti_msp_dl_config.h"

typedef struct
{
	GPTIMER_Regs* gptimer ;
	DL_TIMER_CC_INDEX ccIndex ;
	float PWM_MAX ;
}MyPWM_Typedef ;

extern MyPWM_Typedef MyPWM_Motor_A_IN1;	// Fre:20kHz(消音) , ARR:1000
extern MyPWM_Typedef MyPWM_Motor_B_IN1;

// PWM初始化
void MyPWM_Init(MyPWM_Typedef* MyPWM) ;
// 设置PWM值
void MyPWM_SetCompare(MyPWM_Typedef* MyPWM, uint16_t Compare) ;
// 得到PWM的频率
int MyPWM_GetFre(MyPWM_Typedef* MyPWM) ;
#endif
