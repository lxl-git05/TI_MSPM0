#include "Tim.h"

// ========================== 定时器0 ==========================
/*初始化定时器0*/
void Timer_0_Init(void)
{
    // 1. 清除定时器中断标志
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    // 2. 使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

/*弱函数声明,主函数调用*/
__attribute__((weak)) void Timer_0_Callback(void)
{
    // 默认空实现
}

// ========================== 定时器1 ==========================
/*初始化定时器1*/
void Timer_1_Init(void)
{
    // 1. 清除定时器中断标志
    NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
    // 2. 使能定时器中断
    NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
}

/*弱函数声明,主函数调用*/
__attribute__((weak)) void Timer_1_Callback(void)
{
    // 默认空实现
}

// ========================== 定时器中断中枢 ==========================
void TIMER_0_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            Timer_0_Callback() ;
            break;

        default://其他的定时器中断
            break;
    }
}

void TIMER_1_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_Timer_getPendingInterrupt(TIMER_1_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            Timer_1_Callback() ;
            break;

        default://其他的定时器中断
            break;
    }
}

