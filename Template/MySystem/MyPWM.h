#ifndef __MyPWM_H
#define __MyPWM_H

#include "MySystem.h"

typedef struct {
    GPTIMER_Regs* gptimer;      // 定时器外设寄存器
    DL_TIMER_CC_INDEX ccIndex;  // 比较捕获通道号
    float Compare_Max;          // SetCompare上限
    float Compare_Min;          // SetCompare下限
    uint32_t Tim_Clock;         // 定时器输入时钟频率(Hz)
    IRQn_Type Tim_IRQn;         // 定时器中断号（用于脉冲中断，电机PWM可不填）
} MyPWM_Typedef;

// PWM外部实例（在MySystem.c中定义）
extern MyPWM_Typedef MyPWM_Motor_A_IN1;  // Fre:20kHz(消音), ARR:1000
extern MyPWM_Typedef MyPWM_Motor_B_IN1;
extern MyPWM_Typedef MyPWM_Stepper1;     // 步进电机1 (TIMG6, PA21)
extern MyPWM_Typedef MyPWM_Stepper2;     // 步进电机2 (TIMG7, PA23)
extern MyPWM_Typedef MyPWM_Servo1;      // 舵机1 (TIMG8 CCP0, PA29, 50Hz)
extern MyPWM_Typedef MyPWM_Servo2;      // 舵机2 (TIMG8 CCP1, PA2,  50Hz)

// PWM初始化 — 校验配置是否合理
void MyPWM_Init(MyPWM_Typedef *pwm);
// 设置PWM比较值（自动限幅到[Compare_Min, Compare_Max]）
void MyPWM_SetCompare(MyPWM_Typedef *pwm, float compare);
// 获取PWM频率
int MyPWM_GetFre(MyPWM_Typedef *pwm);
// 设置定时器周期值（Load/ARR），用于步进电机动态调速
void MyPWM_SetLoadValue(MyPWM_Typedef *pwm, uint32_t load);
// 获取定时器输入时钟频率(Hz)
uint32_t MyPWM_GetTimClock(MyPWM_Typedef *pwm);
// 使能定时器更新中断（用于脉冲计数）
void MyPWM_EnableIT(MyPWM_Typedef *pwm);

#endif
