#include "Control.h"
#include "Orange.h"
#include "MPU6050_Angle.h"
#include "MyPID.h"

// 5种状态: 停车 寻迹直行 路口直行 左转 右转 掉头
Car_Status_Typedef curr_Status = Car_Stop;
Car_Status_Typedef next_Status = Car_Stop;

// 路口检测
Track_Status_Typedef Track_Status      = Track_Null;
Track_Status_Typedef Track_Status_Back = Track_Null;

// 电机外环控制台,切记控制台只进行控制，不实现状态转换，所以需要手动判断是否控制完成然后进行状态转换
void Car_Control(void)
{
    if (curr_Status == next_Status)
    {
        switch (curr_Status)
        {
            case Car_Forward  : Oran_Track_Tick(Track_Speed) ; break; // 寻迹环
            case Car_Turn_L   : Con_MPU_Motor_Tick() ;    break;      // 角度环左转,直到有指令修改Status
            case Car_Turn_R   : Con_MPU_Motor_Tick() ;    break;      // 角度环右转,直到有指令修改Status
            case Car_Turn_F   : Motor_SetSpeed(&Motor_A , Track_Speed) ;Motor_SetSpeed(&Motor_B , Track_Speed) ; break; // 路口直行
            case Car_Turn_H   : Con_MPU_Motor_Turn180_Tick() ;    break;    // half圈,也就是180度翻转
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    if (curr_Status != next_Status) // 这里一定要用if,因为条件改变是在==的条件下完成的,否则下一刻curr会与next相等,导致永远到不了!=     
    {
        switch (next_Status) // setup
        {
            case Car_Forward  : PID_Param_Reset(&PID_Track) ; break;
            case Car_Turn_L   : Angle_Track_Check = true ; PID_Param_Reset(&Motor_A.PID_s) ;PID_Param_Reset(&PID_Angle) ; Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw(90)  ; break;  // 左转
            case Car_Turn_R   : Angle_Track_Check = true ; PID_Param_Reset(&Motor_A.PID_s) ;PID_Param_Reset(&PID_Angle) ; Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw(-90) ; break;  // 右转
            case Car_Turn_F   : PID_Param_Reset(&PID_Track) ; break; // 路口直行
            case Car_Turn_H   : Angle_Track_Check = true ; PID_Param_Reset(&Motor_A.PID_s) ;PID_Param_Reset(&Motor_A.PID_s) ; PID_Param_Reset(&Motor_B.PID_s) ; Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw(95) ; break;  // 180度旋转,前面90没毛病，因为是半圈旋转接下
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    // 状态切换
    curr_Status = next_Status ;
}
