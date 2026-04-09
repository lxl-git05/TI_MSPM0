#include "AllHeader.h"

void Mode_1_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_1=====") ;
}

// 功能:检查电机A的PID
void Mode_1_Loop(void)
{
    // OLED检查
    OLED_ShowSignedNum(0,  10, Motor_A.PID_s.realPoint_Now, 3, OLED_8X16);
    // Serial参数更改
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &Motor_A.PID_s.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &Motor_A.PID_s.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &Motor_A.PID_s.Kd) ;
        Serial_SetFloatData(&Serial1, "goalPoint_A", "goalPoint_A=%f", &Motor_A.PID_s.goalPoint) ;
    
        Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint) ;
    }
    // OLED展示
    OLED_Printf(0, 30, OLED_6X8, "%.2f,%.2f,%.2f" , Motor_A.PID_s.Kp , Motor_A.PID_s.Ki , Motor_A.PID_s.Kd) ;
}

void Mode_1_Exit(void)
{

}

// 打印电机A参数
void Mode_1_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Motor_A.PID_s.goalPoint ,Motor_A.PID_s.realPoint_Now ,Motor_A.PID_s.setPoint );
}
