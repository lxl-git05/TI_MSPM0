#include "Mode_5.h"
#include "AllHeader.h"

// ========================== 陀螺仪角度环 ==========================

float Angle_Car ;

void Mode_5_Setup(void)
{
    OLED_Clear();
    PID_Angle_Reset();
}

void Mode_5_Loop(void)
{
    OLED_Printf(0, 0, OLED_8X16, "===Mode_5===") ;
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Angle.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Angle.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Angle.Kd) ;
        Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Angle_Car) ;
    }
    PID_Angle.goalPoint = Angle_Car ;

    OLED_Printf(0, 20, OLED_6X8, "Angle_Car:%.2f",Angle_Car) ;
    OLED_Printf(0, 30, OLED_6X8, "Yaw:%.2f",MPU_Real.yaw) ;
}

void Mode_5_Tick(void)
{
    PID_Angle_Tick();
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Angle.goalPoint ,PID_Angle.realPoint_Now ,PID_Angle.setPoint );
}

void Mode_5_Exit(void)
{
    OLED_Clear();
}
