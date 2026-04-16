#include "Con_Motor.h"

// 初始化
void Motor_Init(void)
{
    Motor_A_Init() ;
    Motor_SetSpeed(&Motor_A, 0) ;   // 使能运动

    Motor_B_Init() ;
    Motor_SetSpeed(&Motor_B, 0) ;
}

// 20ms更新
void Motor_Update_Tick(void)
{
    Motorx_Update_Tick(&Motor_A) ;
    Motorx_Update_Tick(&Motor_B) ;
}

// 设置电机goal速度
void Motor_SetSpeed(Motor_Typedef *Motor, float speed)
{
    speed = speed * Motor->Goal_Speed_Dir;
    if (speed >= Motor_MAX_Speed)
    {
        speed = Motor_MAX_Speed ;
    }
    else if (speed < -Motor_MAX_Speed)
    {
        speed = -Motor_MAX_Speed ;
    }
    Motor->PID_s.goalPoint = speed ;
    Motor->State = MOTOR_RUN;
}

// 得到电机goal速度
int Motor_Get_GoalSpeed(Motor_Typedef *Motor)
{
    return Motor->PID_s.goalPoint ;
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

