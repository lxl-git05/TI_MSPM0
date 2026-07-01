#include "MySystem.h"

// 0. PWM定义
// MyPWM_Typedef MyPWM_Servo1 = {...};
MyPWM_Typedef MyPWM_Motor_A_IN1 = {PWM_MOTOR_PWM_INST , GPIO_PWM_MOTOR_PWM_C0_IDX , 1000.0f};
MyPWM_Typedef MyPWM_Motor_B_IN1 = {PWM_MOTOR_PWM_INST , GPIO_PWM_MOTOR_PWM_C1_IDX , 1000.0f};

// 1. PWM初始化 - TI工程中由 SysConfig 统一初始化，此处保留兼容入口
void MyPWM_Init(MyPWM_Typedef* MyPWM)
{
    if (MyPWM == 0)
    {
        return;
    }

    if (MyPWM->PWM_MAX <= 0.0f)
    {
        MyPWM->PWM_MAX = 1.0f;
    }
}

// 2. 设置PWM值 - 添加限幅功能
void MyPWM_SetCompare(MyPWM_Typedef* MyPWM, uint16_t Compare)
{
    uint32_t limited_compare;

    if ((MyPWM == 0) || (MyPWM->gptimer == 0))
    {
        return;
    }

    limited_compare = (uint32_t)Compare;
    if (limited_compare > (uint32_t)MyPWM->PWM_MAX)
    {
        limited_compare = (uint32_t)MyPWM->PWM_MAX;
    }

    // 设置比较值
    DL_TimerG_setCaptureCompareValue(MyPWM->gptimer, limited_compare, MyPWM->ccIndex);
}

// 3. 得到PWM的频率(近似值，按PWM_MAX对应周期计算)
int MyPWM_GetFre(MyPWM_Typedef* MyPWM)
{
    if ((MyPWM == 0) || (MyPWM->PWM_MAX <= 0.0f))
    {
        return 0;
    }
    return (int)(MySystem_Fre / MyPWM->PWM_MAX);
}
