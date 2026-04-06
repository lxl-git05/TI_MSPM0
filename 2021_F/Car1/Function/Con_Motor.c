#include "Con_Motor.h"

// 初始化
void Motor_Init(void)
{
    Motor_A_Init() ;
    Motor_SetSpeed(&Motor_A, 0) ;

    // Motor_A_Init() ;
    // Motor_SetSpeed(&Motor_A, 0) ;
}

// 设置电机速度
void Motor_SetSpeed(Motor_Typedef *Motor, float speed)
{
    if (speed >= Motor_MAX_Speed)
    {
        speed = Motor_MAX_Speed ;
    }
    else if (speed < -Motor_MAX_Speed)
    {
        speed = -Motor_MAX_Speed ;
    }
    Motor->PID_s.goalPoint = speed;
    Motor->State = MOTOR_RUN;
}

// 电机停止
void Motor_Stop(Motor_Typedef *Motor)
{
    Motor->State = MOTOR_STOP;
}

// 电机急刹
void Motor_Brake(Motor_Typedef *Motor)
{
    Motor->State = MOTOR_BRAKE;
}

