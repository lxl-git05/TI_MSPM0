#include "Mode_3.h"
#include "AllHeader.h"

void Mode_3_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_3=====") ;

    Motor_SetSpeed(&Motor_A, 0) ;
    Motor_SetSpeed(&Motor_B, 0) ;
}

void Mode_3_Loop(void)
{
    // 长按开启小车电机
    if (Key_Check(KEY_0, KEY_LONG))
    {
        Motor_SetSpeed(&Motor_A, 80) ;
        Motor_SetSpeed(&Motor_B, 80) ;
    }
    OLED_Printf(0, 20, OLED_6X8, "%d" , Serial2.Hex_Data.Serial_New_Package[1]) ;
    OLED_Printf(0, 40, OLED_6X8, "%.2f,%.2f" , Motor_A.Distance , -Motor_B.Distance) ;
    OLED_Update();

    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "goalPoint_A", "goalPoint_A=%f", &Motor_A.PID_s.goalPoint) ;
        Motor_B.PID_s.goalPoint = Motor_A.PID_s.goalPoint ;
        Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint) ;
        Motor_SetSpeed(&Motor_B, Motor_B.PID_s.goalPoint) ;
    }

    if (Motor_A.Distance > 0.6f || Motor_B.Distance >= 0.6f)
    {
        Motor_SetSpeed(&Motor_A, 0 ) ;
        Motor_SetSpeed(&Motor_B, 0 ) ;
    }

    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Motor_SetSpeed(&Motor_A, 120 ) ;
        Motor_SetSpeed(&Motor_B, 120 ) ;
    }
}

void Mode_3_Exit(void)
{
    OLED_Clear() ;
}

void Mode_3_Tick(void)
{
    
}
