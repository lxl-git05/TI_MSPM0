#include "Servo.h"

// 全局舵机实例
Servo_Typedef Servo1;
Servo_Typedef Servo2;

// 初始化舵机
void Servo_Init(Servo_Typedef* pServo, MyPWM_Typedef* PWM,
                float angle_min, float angle_max,
                uint16_t pulse_min_us, uint16_t pulse_max_us)
{
    // 绑定PWM通道
    pServo->PWM = PWM;

    // 角度范围
    pServo->Angle_Min = angle_min;
    pServo->Angle_Max = angle_max;

    // 脉宽范围
    pServo->Pulse_Min = pulse_min_us;
    pServo->Pulse_Max = pulse_max_us;

    // 设置PWM周期为20000（50Hz @ 1MHz时钟）
    // SysConfig生成的默认周期=1000，必须改为20000
    MyPWM_SetLoadValue(pServo->PWM, 20000);

    // 初始化PWM — 校验Load值+补全Compare默认限幅
    // 注意：MyPWM_Init在SetLoadValue之后调用，以正确读取Load值
    MyPWM_Init(pServo->PWM);

    // 归中
    pServo->Angle_Now = (pServo->Angle_Min + pServo->Angle_Max) / 2.0f;
    Servo_SetAngle(pServo, pServo->Angle_Now);
}

// 设置角度 — 线性映射: angle → pulse_us → Compare值
// 公式: pulse = Pulse_Min + (angle - Angle_Min) * (Pulse_Max - Pulse_Min) / (Angle_Max - Angle_Min)
void Servo_SetAngle(Servo_Typedef* pServo, float angle)
{
    // 角度限幅
    if (angle < pServo->Angle_Min) angle = pServo->Angle_Min;
    if (angle > pServo->Angle_Max) angle = pServo->Angle_Max;

    // 记录当前角度
    pServo->Angle_Now = angle;

    // 线性映射
    float range_angle = pServo->Angle_Max - pServo->Angle_Min;
    float range_pulse = (float)(pServo->Pulse_Max - pServo->Pulse_Min);
    float pulse = (float)pServo->Pulse_Min +
                  (angle - pServo->Angle_Min) * range_pulse / range_angle;

    // 写入PWM比较值（1计数=1us，pulse值直接等于Compare值）
    MyPWM_SetCompare(pServo->PWM, pulse);
}

// 直接设置脉宽
void Servo_SetPulse_us(Servo_Typedef* pServo, uint16_t pulse_us)
{
    float pulse_f = (float)pulse_us;

    // 脉宽限幅
    if (pulse_f > (float)pServo->Pulse_Max) pulse_f = (float)pServo->Pulse_Max;
    if (pulse_f < (float)pServo->Pulse_Min) pulse_f = (float)pServo->Pulse_Min;

    // 反算角度
    float range_pulse = (float)(pServo->Pulse_Max - pServo->Pulse_Min);
    float range_angle = pServo->Angle_Max - pServo->Angle_Min;
    pServo->Angle_Now = pServo->Angle_Min +
                        (pulse_f - (float)pServo->Pulse_Min) * range_angle / range_pulse;

    MyPWM_SetCompare(pServo->PWM, pulse_f);
}

// 获取当前角度
float Servo_GetAngle(const Servo_Typedef* pServo)
{
    return pServo->Angle_Now;
}
