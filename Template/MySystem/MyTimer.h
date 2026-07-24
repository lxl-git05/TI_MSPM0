#ifndef __MYTIMER_H
#define __MYTIMER_H

#include "MySystem.h"

void Timer_Initial(void) ;
void Timer_1ms_Callback(void) ;
void Timer_20ms_Callback(void) ;

// 全局中断开关（用于I2C等不能被ISR抢占的关键时序）
void Timer_DisableIRQ(void);   // 关闭所有中断
void Timer_EnableIRQ(void);    // 恢复所有中断

#endif
