#ifndef __TIMER_COUNTER_H
#define __TIMER_COUNTER_H

#include "MySystem.h"

// 开始计时
void Timer_Counter_Begin(void) ;

// 结束计时
void Timer_Counter_End(void) ;

// 每次状态变化指示被调用一次
void Timer_Counter_Func(void) ;

#endif
