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

// 获取PWM频率 — 使用 Tim_Clock 计算
int MyPWM_GetFre(MyPWM_Typedef *pwm)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return 0;
    }

    uint32_t load = DL_TimerG_getLoadValue(pwm->gptimer);
    if (load == 0) {
        return 0;
    }

    uint32_t clock = (pwm->Tim_Clock > 0) ? pwm->Tim_Clock : MySystem_Fre;
    return (int)(clock / (load + 1));
}

// 设置定时器周期值（AR/Load），用于步进电机动态调速
void MyPWM_SetLoadValue(MyPWM_Typedef *pwm, uint32_t load)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return;
    }
    if (load < 1)  load = 1;
    if (load > 65535) load = 65535;
    DL_TimerG_setLoadValue(pwm->gptimer, load);
}

// 获取定时器输入时钟频率(Hz)
uint32_t MyPWM_GetTimClock(MyPWM_Typedef *pwm)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return 0;
    }
    if (pwm->Tim_Clock > 0) {
        return pwm->Tim_Clock;
    }
    return MySystem_Fre;  // 兜底：未配置时用主频
}

// 使能定时器更新中断（用于脉冲计数，步进电机专用）
void MyPWM_EnableIT(MyPWM_Typedef *pwm)
{
    if (pwm == 0 || pwm->gptimer == 0) {
        return;
    }
    // 优先级=1（低于 1ms Tick 的优先级 0，保证 Tick 不掉）
    NVIC_SetPriority(pwm->Tim_IRQn, 1);
    NVIC_ClearPendingIRQ(pwm->Tim_IRQn);
    NVIC_EnableIRQ(pwm->Tim_IRQn);
    // 使能计数器归零中断（每个 PWM 周期触发一次 → 脉冲计数）
    DL_TimerG_enableInterrupt(pwm->gptimer, DL_TIMER_INTERRUPT_ZERO_EVENT);
}
