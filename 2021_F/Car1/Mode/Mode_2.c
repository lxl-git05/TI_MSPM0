#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_2=====") ;
}

// 功能:检查电机B的PID
void Mode_2_Loop(void)
{
    // OLED检查
    OLED_ShowSignedNum(0,  10, Motor_B.PID_s.realPoint_Now, 3, OLED_8X16);
    // Serial参数更改
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &Motor_B.PID_s.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &Motor_B.PID_s.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &Motor_B.PID_s.Kd) ;
        Serial_SetFloatData(&Serial1, "goalPoint_B", "goalPoint_B=%f", &Motor_B.PID_s.goalPoint) ;
    
        Motor_SetSpeed(&Motor_B, Motor_B.PID_s.goalPoint) ;
    }
    // OLED展示
    OLED_Printf(0, 30, OLED_6X8, "%.2f,%.2f,%.2f" , Motor_B.PID_s.Kp , Motor_B.PID_s.Ki , Motor_B.PID_s.Kd) ;
}

void Mode_2_Exit(void)
{
    
}

// 打印电机B参数
void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Motor_B.PID_s.goalPoint ,Motor_B.PID_s.realPoint_Now ,Motor_B.PID_s.setPoint );
}
