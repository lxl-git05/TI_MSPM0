#include "MyTimer.h"

// 定时器初始化
void Timer_Initial(void)
{
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
}

/*弱函数声明,主函数调用*/
__attribute__((weak)) void Timer_1ms_Callback(void)
{
    // 默认空实现
}

/*弱函数声明,主函数调用*/
__attribute__((weak)) void Timer_20ms_Callback(void)
{
    // 默认空实现
}

// TIMER_0: 1ms 定时中断
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            Timer_1ms_Callback();
            break;

        default:
            break;
    }
}

// ========== 全局中断开关 ==========
// 标定/Flash写入等场景需要独占I2C等资源，防止ISR抢占
void Timer_DisableIRQ(void)
{
    __disable_irq();
}

void Timer_EnableIRQ(void)
{
    __enable_irq();
}

// TIMER_1: 20ms 定时中断
void TIMER_1_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_1_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            Timer_20ms_Callback();
            break;

        default:
            break;
    }
}
