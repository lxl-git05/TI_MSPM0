#include "Mode_4.h"
#include "AllHeader.h"

void Mode_4_Setup(void)
{
    OLED_Clear();
    OLED_Printf(0, 20,  OLED_6X8, "IMU");
    OLED_Update();
}

void Mode_4_Loop(void)
{
    OLED_Printf(0, 0,  OLED_6X8, "R:%.1f", IMU_Mahony_Real.roll);
    OLED_Printf(0, 12, OLED_6X8, "P:%.1f", IMU_Mahony_Real.pitch);
    OLED_Printf(0, 24, OLED_6X8, "Y:%.1f", IMU_Mahony_Real.yaw);
#ifdef I2C_DEBUG_RESET_COUNT
    OLED_Printf(0, 36,  OLED_6X8, "I2C_Rst:%lu", IIC_Reset_Count);
#endif
}

void Mode_4_Tick(void)
{
    // 串口CSV输出（调试用，可注释掉）
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f\r\n",
                  IMU_Mahony_Real.roll, IMU_Mahony_Real.pitch, IMU_Mahony_Real.yaw,IMU_Yaw_Abs_Get());
}

void Mode_4_Exit(void)
{
    OLED_Clear();
}
