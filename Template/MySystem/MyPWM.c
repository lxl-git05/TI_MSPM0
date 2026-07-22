#include "MyPWM.h"

// PWM初始化 — 校验LOAD配置是否合理，补全默认参数
void MyPWM_Init(MyPWM_Typedef *pwm)
{
    if (pwm == 0) {
        return;
    }

    // 读取PWM周期值(LOAD)，校验范围
    uint32_t load = DL_TimerG_getLoadValue(pwm->gptimer);

    // LOAD异常则卡死（方便调试发现配置问题）
    if (load == 0 || load > 65535) {
        while (1) { }
    }

    // 参数合法性补全
    if (pwm->Compare_Max <= 0.0f) {
        pwm->Compare_Max = (float)load;  // 默认使用LOAD值作为上限
    }
    if (pwm->Compare_Min < 0.0f) {
        pwm->Compare_Min = 0.0f;
    }

    // MSPM0中PWM由SysConfig统一初始化并启动，此处仅做参数校验
}

// 设置PWM比较值 — 自动限幅到[Compare_Min, Compare_Max]
void MyPWM_SetCompare(MyPWM_Typedef *pwm, float compare)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return;
    }

    if (compare > pwm->Compare_Max) compare = pwm->Compare_Max;
    if (compare < pwm->Compare_Min) compare = pwm->Compare_Min;
    DL_TimerG_setCaptureCompareValue(pwm->gptimer, (uint32_t)compare, pwm->ccIndex);
}

// 获取PWM频率
int MyPWM_GetFre(MyPWM_Typedef *pwm)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return 0;
    }

    uint32_t load = DL_TimerG_getLoadValue(pwm->gptimer);
    if (load == 0) {
        return 0;
    }

    return (int)(MySystem_Fre / (load + 1));
}
