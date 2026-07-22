#include "Encoder_Key.h"

static int16_t Encoder_Count = 0;   // 编码器累计脉冲数

// ========== 初始化 ==========
void Encoder_Init(void)
{
    // GPIO引脚和中断配置已在 empty.syscfg 中完成：
    //   EC11_S1 (PA12): INPUT + PULL_UP + FALL_INT
    //   EC11_S2 (PA14): INPUT + PULL_UP + FALL_INT
    // 使能 GPIOA GROUP1 中断（参照 MyEncoder_Init 模式）
    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

    Encoder_Count = 0;
}

// ========== 获取并清零计数 ==========
int16_t Encoder_Get(void)
{
    int16_t temp = Encoder_Count;
    Encoder_Count = 0;
    return temp;
}

// ========== EC11 中断服务（GROUP1 GPIOA）==========
// EC11 原理：A/B两相正交，每 detent 产生 4 个边沿（A↓→B↓→A↑→B↑ 或反向）
// 方向判断：某相下降沿时，看另一相电平即可确定旋转方向
//   正转(CW):  S1↓ 时 S2=HIGH  → ++
//   正转(CW):  S2↓ 时 S1=LOW   → ++
//   反转(CCW): S1↓ 时 S2=LOW   → --
//   反转(CCW): S2↓ 时 S1=HIGH  → --
void EC11_Encoder_ISR(void)
{
    // 检查 S1 (PA12) 下降沿
    if (DL_GPIO_getEnabledInterruptStatus(MyGPIO_EC11_S1.GPIO_Port, MyGPIO_EC11_S1.GPIO_Pin)) {
        DL_GPIO_clearInterruptStatus(MyGPIO_EC11_S1.GPIO_Port, MyGPIO_EC11_S1.GPIO_Pin);
        if (MyGPIO_ReadPin(&MyGPIO_EC11_S1) == 0)       // 确认仍为低
        {
            if (MyGPIO_ReadPin(&MyGPIO_EC11_S2))        // S2=HIGH → 正转
                Encoder_Count++;
            else                                         // S2=LOW  → 反转
                Encoder_Count--;
        }
    }

    // 检查 S2 (PA14) 下降沿
    if (DL_GPIO_getEnabledInterruptStatus(MyGPIO_EC11_S2.GPIO_Port, MyGPIO_EC11_S2.GPIO_Pin)) {
        DL_GPIO_clearInterruptStatus(MyGPIO_EC11_S2.GPIO_Port, MyGPIO_EC11_S2.GPIO_Pin);
        if (MyGPIO_ReadPin(&MyGPIO_EC11_S2) == 0)       // 确认仍为低
        {
            if (MyGPIO_ReadPin(&MyGPIO_EC11_S1))        // S1=HIGH → 反转
                Encoder_Count--;
            else                                         // S1=LOW  → 正转
                Encoder_Count++;
        }
    }
}
