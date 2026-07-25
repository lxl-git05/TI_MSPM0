#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "Mode2") ;
    OLED_Printf(0, 40, OLED_6X8, "%f",IMU_Yaw_Abs_Get()) ;
    OLED_Printf(0, 20, OLED_6X8, "Reset:%d",IIC_Reset_Count) ;
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f\n",IMU_Mahony_Real.yaw,IMU_Mahony_Real.pitch , IMU_Mahony_Real.roll , IMU_Yaw_Abs_Get()) ;
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
