#include "Mode_2.h"
#include "AllHeader.h"

Pid_Typedef PID_Angle ; 

void Mode_2_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Angle=====") ;
    PID_Init(&PID_Angle, 0.66f, 0.0f, 0.0f, 20, -20, 1000) ;
    PID_Angle.goalPoint = 0 ;
}

void Mode_2_Loop(void)
{
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Angle.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Angle.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Angle.Kd) ;
    }
}

// 20ms让小车旋转固定角度
void Mode_2_Tick(void)
{
    // 1. 得到真实角度(yaw)
    // 将tick写在MPU更新之后即可
    PID_Angle.realPoint_Now = MPU_Real.yaw ;

    // 2. 状态机控制,略

    // 3. PID计算
    PID_Update(&PID_Angle, PID_Angle.realPoint_Now ) ;

    // 4. 输出小车转速, 差速
    Motor_SetSpeed(&Motor_A, - PID_Angle.setPoint) ;
    Motor_SetSpeed(&Motor_B,   PID_Angle.setPoint) ;

    // 5. 展示效果
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Angle.goalPoint ,PID_Angle.realPoint_Now ,PID_Angle.setPoint );
}

void Mode_2_Exit(void)
{
    
}
