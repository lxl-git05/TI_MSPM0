#ifndef __MyPWM_H
#define __MyPWM_H

#include "MySystem.h"

typedef struct {
    GPTIMER_Regs* gptimer;      // 定时器外设寄存器
    DL_TIMER_CC_INDEX ccIndex;  // 比较捕获通道号
    float Compare_Max;          // SetCompare上限
    float Compare_Min;          // SetCompare下限
} MyPWM_Typedef;

// PWM外部实例（在MySystem.c中定义）
extern MyPWM_Typedef MyPWM_Motor_A_IN1;  // Fre:20kHz(消音), ARR:1000
extern MyPWM_Typedef MyPWM_Motor_B_IN1;

// PWM初始化 — 校验配置是否合理
void MyPWM_Init(MyPWM_Typedef *pwm);
// 设置PWM比较值（自动限幅到[Compare_Min, Compare_Max]）
void MyPWM_SetCompare(MyPWM_Typedef *pwm, float compare);
// 获取PWM频率
int MyPWM_GetFre(MyPWM_Typedef *pwm);

#endif
