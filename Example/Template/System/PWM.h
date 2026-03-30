#ifndef __PWM_H
#define __PWM_H

#include "ti_msp_dl_config.h"

// PWM初始化
// void PWM_Init(TIM_HandleTypeDef htimx , uint32_t Channel) ;

// 设置PWM值
void PWM_SetCompare1(GPTIMER_Regs *gptimer, uint32_t value, DL_TIMER_CC_INDEX ccIndex) ;

#endif
