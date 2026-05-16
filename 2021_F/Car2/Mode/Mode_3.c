#include "Mode_3.h"
#include "AllHeader.h"

Pid_Typedef PID_D_A ;   // A 电机的距离环PID
Pid_Typedef PID_D_B ;   // B 电机的距离环PID

void Mode_3_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_3=====") ;

    Motor_SetSpeed(&Motor_A, 0) ;
    Motor_SetSpeed(&Motor_B, 0) ;

    PID_Init(&PID_D_A , 2000.0f , 0.0f , 0.0f , 120 , -120 , 1000 ) ;
    PID_Init(&PID_D_B , 2000.0f , 0.0f , 0.0f , 120 , -120 , 1000 ) ;
}

void Mode_3_Loop(void)
{
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        PID_D_A.goalPoint = 0.60f ;
        PID_D_B.goalPoint = 0.60f ;
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        PID_D_A.goalPoint = 0.0f ;
        PID_D_B.goalPoint = 0.0f ;
    }
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_D_A.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_D_A.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_D_A.Kd) ;
        Serial_SetFloatData(&Serial1, "goalPoint_A", "goalPoint_A=%f", &PID_D_A.goalPoint) ;
    }
    OLED_Printf(0, 40, OLED_6X8, "%.2f,%.2f" , Motor_A.Distance , -Motor_B.Distance) ;
}

void Mode_3_Exit(void)
{
    OLED_Clear() ;
}

void Mode_3_Tick(void)
{
    // 1. 得到当前路程
    PID_D_A.realPoint_Now = Motor_A.Distance ;
    PID_D_B.realPoint_Now = -Motor_B.Distance ;

    // 2. PID 计算
    PID_Update(&PID_D_A, PID_D_A.realPoint_Now) ;
    PID_Update(&PID_D_B, PID_D_B.realPoint_Now) ;

    // 3. 输出设定值
    Motor_SetSpeed(&Motor_A, PID_D_A.setPoint) ;
    Motor_SetSpeed(&Motor_B, PID_D_B.setPoint) ;

    Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f\n",PID_D_A.goalPoint ,PID_D_A.realPoint_Now ,PID_D_B.goalPoint ,PID_D_B.realPoint_Now );
}
