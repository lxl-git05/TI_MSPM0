#include "Timer_Counter.h"

// ========== 内部变量 ==========
static uint32_t Timer_Counter_Start_count;
static uint32_t Timer_Counter_End_count;

// ========== 全局计时结果 ==========
float time_us;
float time_ms;
float time_Func_us;
float time_Func_ms;

// ========== 读定时器当前值 ==========
// TIMG12 @ 4MHz: 1 tick = 0.25µs
// 32位最大值: 0xFFFFFFFF → @4MHz 约 1074 秒（~18分钟）
static inline uint32_t TICKS_Get(void)
{
    return DL_Timer_getTimerCount(TIMER_2_INST);
}

// ========== 单位转换 ==========
// 4MHz → 1 tick = 0.25µs → us = ticks / 4
#define TICKS_TO_US(ticks)  ((float)(ticks) * 0.25f)

// ========== 初始化 ==========
void Timer_Counter_Init(void)
{
    // SysConfig 已初始化 TIMG12 为 4MHz PERIODIC 模式
    // 重配为 向上计数 + 最大周期 = 自由运行计数器
    DL_TimerG_TimerConfig freeRunCfg = {
        .timerMode    = DL_TIMER_TIMER_MODE_PERIODIC_UP,
        .period       = 0xFFFFFFFF,     // 32位最大周期
        .startTimer   = DL_TIMER_START,
        .genIntermInt = DL_TIMER_INTERM_INT_DISABLED,
        .counterVal   = 0,
    };
    DL_TimerG_initTimerMode(TIMER_2_INST,
        (DL_TimerG_TimerConfig *)&freeRunCfg);

    // 清零全局结果
    time_us      = 0.0f;
    time_ms      = 0.0f;
    time_Func_us = 0.0f;
    time_Func_ms = 0.0f;
}

// ========== 代码块计时 ==========
void Timer_Counter_Begin(void)
{
    Timer_Counter_Start_count = TICKS_Get();
}

void Timer_Counter_End(void)
{
    Timer_Counter_End_count = TICKS_Get();
    // uint32 减法自动处理计数器溢出（C 标准环绕语义）
    uint32_t ticks = Timer_Counter_End_count - Timer_Counter_Start_count;
    time_us = TICKS_TO_US(ticks);
    time_ms = time_us / 1000.0f;
}

// ========== 函数间隔计时 ==========
void Timer_Counter_Func(void)
{
    static uint32_t Timer_Counter_Before = 0;
    uint32_t Timer_Counter_Now = TICKS_Get();
    uint32_t ticks = Timer_Counter_Now - Timer_Counter_Before;
    time_Func_us = TICKS_TO_US(ticks);
    time_Func_ms = time_Func_us / 1000.0f;
    Timer_Counter_Before = Timer_Counter_Now;
}
