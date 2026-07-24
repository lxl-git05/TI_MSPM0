#include "IMU.h"
#include "math.h"

// ==================== 【陀螺零偏 (deg/s) —— 两大陀螺仪共用】 ====================
// 标定时自动更新，也可由 AT24C02 恢复 / 用户手动赋值
float IMU_Mahony_GyroBiasX = -9.26840305f;
float IMU_Mahony_GyroBiasY = 0.428176761f;
float IMU_Mahony_GyroBiasZ = -1.27142811f;
// ==================== 偏航角到位检测 ====================

// target: 目标 yaw_abs 角度 (°)
// deadband: 死区宽度 (°)，内算到位
uint8_t IMU_Turn_Yaw_Is_Ok_Ex(float target, float deadband)
{
    float yaw_now = IMU_Yaw_Abs_Get();
    float err     = yaw_now - target;

    if (err < 0.0f) err = -err;     // |error|

    return (err <= deadband) ? 1 : 0;
}

// 使用默认死区
uint8_t IMU_Turn_Yaw_Is_Ok(float target)
{
    return IMU_Turn_Yaw_Is_Ok_Ex(target, IMU_TURN_YAW_DEFAULT_DEADBAND);
}
