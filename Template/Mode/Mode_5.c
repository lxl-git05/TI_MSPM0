#include "Mode_5.h"
#include "AllHeader.h"

// ==================== 舵机循环演示 ====================
// 两个舵机同时从0°扫到180°再返回，无限循环
// 步进速率：1°/tick × 50tick/s = 50°/s，180°扫程≈3.6s单程

static float  Servo_Angle = 0.0f;     // 当前目标角度(°)
static int8_t Servo_Dir   = 1;        // 方向: +1=递增, -1=递减
static float  Servo_Step  = 1.0f;     // 步长(°/tick)

void Mode_5_Setup(void)
{
    // 从0°开始，方向递增
    Servo_Angle = 0.0f;
    Servo_Dir   = 1;
    Servo_SetAngle(&Servo1, Servo_Angle);
    Servo_SetAngle(&Servo2, Servo_Angle);
}

void Mode_5_Loop(void)
{
    // 主循环为空 — 角度更新放在Tick中保证20ms稳定步进
}

void Mode_5_Tick(void)
{
    // 更新角度
    Servo_Angle += Servo_Dir * Servo_Step;

    // 到达边界时反转方向
    if (Servo_Angle >= 180.0f) {
        Servo_Angle = 180.0f;
        Servo_Dir = -1;
    } else if (Servo_Angle <= 0.0f) {
        Servo_Angle = 0.0f;
        Servo_Dir = 1;
    }

    // 两路舵机同步输出
    Servo_SetAngle(&Servo1, Servo_Angle);
    Servo_SetAngle(&Servo2, Servo_Angle);
}

void Mode_5_Exit(void)
{
    // 退出时舵机归中
    Servo_SetAngle(&Servo1, 90.0f);
    Servo_SetAngle(&Servo2, 90.0f);
}
