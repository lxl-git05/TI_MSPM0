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
// 原理：EC11旋转编码器A/B相正交信号
//   正转: S1下降沿时S2为低 → ++
//   反转: S2下降沿时S1为低 → --
void EC11_Encoder_ISR(void)
{
    // 检查 S1 (PA12) 中断标志
    if (DL_GPIO_getEnabledInterruptStatus(MyGPIO_EC11_S1.GPIO_Port, MyGPIO_EC11_S1.GPIO_Pin)) {
        DL_GPIO_clearInterruptStatus(MyGPIO_EC11_S1.GPIO_Port, MyGPIO_EC11_S1.GPIO_Pin);
        // 去抖动：再次确认 S1 仍为低电平
        if (MyGPIO_ReadPin(&MyGPIO_EC11_S1) == 0) {
            // 读取 S2 电平判断方向（下降沿时S2为低→正转）
            if (MyGPIO_ReadPin(&MyGPIO_EC11_S2) == 0) {
                Encoder_Count++;
            }
        }
    }

    // 检查 S2 (PA14) 中断标志
    if (DL_GPIO_getEnabledInterruptStatus(MyGPIO_EC11_S2.GPIO_Port, MyGPIO_EC11_S2.GPIO_Pin)) {
        DL_GPIO_clearInterruptStatus(MyGPIO_EC11_S2.GPIO_Port, MyGPIO_EC11_S2.GPIO_Pin);
        if (MyGPIO_ReadPin(&MyGPIO_EC11_S2) == 0) {
            // 下降沿时S1为低→反转
            if (MyGPIO_ReadPin(&MyGPIO_EC11_S1) == 0) {
                Encoder_Count--;
            }
        }
    }
}
