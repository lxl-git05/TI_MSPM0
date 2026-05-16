#ifndef __PWM_H
#define __PWM_H

#include "ti_msp_dl_config.h"

// PWM初始化
// void PWM_Init(TIM_HandleTypeDef htimx , uint32_t Channel) ;

// 设置PWM值
void PWM_SetCompare(GPTIMER_Regs *gptimer, DL_TIMER_CC_INDEX ccIndex, uint32_t value) ;

#endif
