#ifndef __TIMER_COUNTER_H
#define __TIMER_COUNTER_H

#include "MySystem.h"

/* ====================================================================
 * Timer_Counter — 微秒级代码执行时间测量
 *
 * 使用 TIMG12 硬件定时器实现 DWT 等效功能（MSPM0 无 DWT CYCCNT）
 * 时钟: BUSCLK(32MHz) / 8 = 4MHz → 0.25µs/tick
 * 模式: 32位向上计数，周期 0xFFFFFFFF，自动回绕
 *
 * 用法示例:
 *   Timer_Counter_Begin();
 *   Func_To_Measure();
 *   Timer_Counter_End();
 *   // time_us 和 time_ms 包含测量结果
 *
 *   void Loop(void) {
 *       DoWork();
 *       Timer_Counter_Func();
 *       // time_Func_us = 本次 Loop 距上次的时间
 *   }
 * ==================================================================== */

// 全局计时变量
extern float time_us;         // Begin→End 间隔（微秒）
extern float time_ms;         // Begin→End 间隔（毫秒）
extern float time_Func_us;    // Func 两次调用间隔（微秒）
extern float time_Func_ms;    // Func 两次调用间隔（毫秒）

// 初始化 — 在 Initial_All() 中调用
void Timer_Counter_Init(void);

// 开始计时 — 记录当前计数值
void Timer_Counter_Begin(void);

// 结束计时 — 计算差值，结果存入 time_us / time_ms
void Timer_Counter_End(void);

// 函数间隔计时 — 计算距上次调用的时间，结果存入 time_Func_us / time_Func_ms
void Timer_Counter_Func(void);

#endif
