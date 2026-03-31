#ifndef __TIM_H
#define __TIM_H

#include "ti_msp_dl_config.h"

// 定时器0函数
void Timer_0_Init(void);
void Timer_0_Callback(void) ;   // 1ms调用一次

#endif
