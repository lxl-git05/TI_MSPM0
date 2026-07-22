#include "MyEncoder.h"

// ====================================================================
// 1. 编码器初始化 — 清除并使能NVIC中断
// ====================================================================
void MyEncoder_Init(MyEncoder_Typedef *encoder)
{
    if (encoder == 0 || encoder->pins.port == 0 ||
        encoder->pins.pin_A == 0U || encoder->pins.pin_B == 0U)
    {
        return;
    }

    NVIC_ClearPendingIRQ(encoder->IRQN);
    NVIC_EnableIRQ(encoder->IRQN);
}

// ====================================================================
// 2. 编码器中断服务 — 双边沿触发方向判断与计数
//    Pin_A 双边沿触发中断, 读取 Pin_B 电平判断方向
//    A==B → 正转(+1), A!=B → 反转(-1)
// ====================================================================
void MyEncoder_ISR(MyEncoder_Typedef *encoder)
{
    uint32_t pinA_level, pinB_level;

    if (encoder == 0 || encoder->pins.port == 0)
    {
        return;
    }

    // 确认是否为本编码器的Pin_A触发的中断
    if (DL_GPIO_getEnabledInterruptStatus(encoder->pins.port, encoder->pins.pin_A)
        != encoder->pins.pin_A)
    {
        return;
    }

    // 读取A/B两相电平
    pinA_level = (DL_GPIO_readPins(encoder->pins.port, encoder->pins.pin_A) != 0U) ? 1U : 0U;
    pinB_level = (DL_GPIO_readPins(encoder->pins.port, encoder->pins.pin_B) != 0U) ? 1U : 0U;

    // 方向判断
    if (pinA_level == pinB_level)
    {
        encoder->cnt++;
    }
    else
    {
        encoder->cnt--;
    }

    // 清除Pin_A中断标志
    DL_GPIO_clearInterruptStatus(encoder->pins.port, encoder->pins.pin_A);
}

// ====================================================================
// 3. 获取并清零本次周期脉冲增量（自动累加到total_cnt）
//    调用频率: 20ms 周期（Timer_20ms_Callback → Mode_X_Tick）
// ====================================================================
int MyEncoder_Get_CNT(MyEncoder_Typedef *encoder)
{
    int32_t cnt;

    if (encoder == 0)
    {
        return 0;
    }

    cnt = encoder->cnt;
    encoder->cnt = 0;
    encoder->total_cnt += cnt;

    return (int)cnt;
}

// ====================================================================
// 4. 获取累计脉冲数
// ====================================================================
int MyEncoder_Get_Total_CNT(MyEncoder_Typedef *encoder)
{
    if (encoder == 0)
    {
        return 0;
    }
    return (int)encoder->total_cnt;
}

// ====================================================================
// 5. 清除累计脉冲数
// ====================================================================
void MyEncoder_Total_Cnt_Clear(MyEncoder_Typedef *encoder)
{
    if (encoder == 0)
    {
        return;
    }
    encoder->cnt = 0;
    encoder->total_cnt = 0;
}
