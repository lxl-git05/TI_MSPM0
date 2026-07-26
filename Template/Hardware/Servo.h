#ifndef __SERVO_H
#define __SERVO_H

#include "MySystem.h"

// 舵机默认参数
#define SERVO_PULSE_MIN     500     // 最小脉宽(us)，对应0度
#define SERVO_PULSE_MAX     2500    // 最大脉宽(us)，对应180度
#define SERVO_ANGLE_MIN     0.0f    // 最小角度(度)
#define SERVO_ANGLE_MAX     180.0f  // 最大角度(度)

typedef struct
{
    MyPWM_Typedef* PWM;         // PWM实例指针
    float Angle_Now;            // 当前角度(°)
    float Angle_Min;            // 最小角度限制(°)
    float Angle_Max;            // 最大角度限制(°)
    uint16_t Pulse_Min;         // 最小脉宽(us)
    uint16_t Pulse_Max;         // 最大脉宽(us)
} Servo_Typedef;

extern Servo_Typedef Servo1;
extern Servo_Typedef Servo2;

// 初始化舵机（绑定PWM，设置角度/脉宽范围，归中）
void Servo_Init(Servo_Typedef* pServo, MyPWM_Typedef* PWM,
                float angle_min, float angle_max,
                uint16_t pulse_min_us, uint16_t pulse_max_us);

// 设置角度(°)，自动限幅+线性映射为脉宽
void Servo_SetAngle(Servo_Typedef* pServo, float angle);

// 直接设置脉宽(us)
void Servo_SetPulse_us(Servo_Typedef* pServo, uint16_t pulse_us);

// 获取当前角度(°)
float Servo_GetAngle(const Servo_Typedef* pServo);

#endif
