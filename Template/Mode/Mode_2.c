#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "Mode2") ;
    OLED_Printf(0, 40, OLED_6X8, "%f",MPU_Yaw_Abs_Get()) ;
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n" , MPU_Mahony_Real.yaw , MPU_Mahony_Real.roll , MPU_Mahony_Real.pitch) ;
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
