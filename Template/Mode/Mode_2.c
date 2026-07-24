#include "Mode_2.h"
#include "AllHeader.h"

// ==================== ICM42688 Mahony AHRS 测试 ====================
// 算法: 四元数 + PI 重力修正，无万向节死锁，yaw 相对准确
// OLED:  Roll / Pitch / Yaw  实时显示
// 串口:  roll,pitch,yaw CSV 输出 (Serial1)
//
// 测试流程:
//   1. 上电后进入 Mode_2 → Init 自动标定零偏（保持设备静置！）
//   2. 水平旋转 90° → Yaw 应显示 ~90°（不再减半）
//   3. 倾斜 30° 绕世界Z轴旋转 → Yaw 准确跟踪，回正后归零
//   4. 随意抖动后静置 → Roll/Pitch <0.5s 收敛到正确值

void Mode_2_Setup(void)
{
    OLED_Clear();
    OLED_Printf(0, 20,  OLED_6X8, "IMU");
    OLED_Update();
    
    IMU_Mahony_Init(1) ;
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0,  OLED_6X8, "R:%.1f", IMU_Mahony_Real.roll);
    OLED_Printf(0, 12, OLED_6X8, "P:%.1f", IMU_Mahony_Real.pitch);
    OLED_Printf(0, 24, OLED_6X8, "Y:%.1f", IMU_Mahony_Real.yaw);
#ifdef I2C_DEBUG_RESET_COUNT
    OLED_Printf(0, 1,  OLED_6X8, "I2C_Rst:%lu", IIC_Reset_Count);
#endif
}

void Mode_2_Tick(void)
{
    // 串口CSV输出（调试用，可注释掉）
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f\r\n",
                  IMU_Mahony_Real.roll, IMU_Mahony_Real.pitch, IMU_Mahony_Real.yaw,IMU_Yaw_Abs_Get());
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
