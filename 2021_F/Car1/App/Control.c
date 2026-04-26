#include "AllHeader.h"

Car_Status_Typedef curr_Status = Car_Stop;
Car_Status_Typedef next_Status = Car_Stop;

// 电机外环控制台
void Car_Control(void)
{
    if (curr_Status == next_Status)
    {
        switch (curr_Status) 
        {
            case Car_Forward  : Oran_Track_Tick() ;       break;    // 寻迹环
            case Car_Turn_L   : Con_MPU_Motor_Tick() ;    break;    // 角度环左转
            case Car_Turn_R   : Con_MPU_Motor_Tick() ;    break;    // 角度环右转
            case Car_Turn_F   : Motor_SetSpeed(&Motor_A , 120) ;Motor_SetSpeed(&Motor_B , 120) ; break; // 路口直行
            case Car_Turn_H   : Con_MPU_Motor_Tick() ;    break;    // half圈,也就是180度翻转
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    if (curr_Status != next_Status) // 这里一定要用if,因为条件改变是在==的条件下完成的,否则下一刻curr会与next相等,导致永远到不了!=     
    {
        // switch (curr_Status) // exit
        // {
        //     case Car_Forward  : break;  // 停车
        //     case Car_Turn_L   : break;  // 左转
        //     case Car_Turn_R   : break;  // 右转
        //     case Car_Turn_H   : break;  // 180度旋转
        //     case Car_Stop     : break;  // 停车
        // }

        switch (next_Status) // setup
        {
            case Car_Forward  : break;
            case Car_Turn_L   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw( 100) ; break;  // 左转
            case Car_Turn_R   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw(-100) ; break;  // 右转
            case Car_Turn_F   : break; // 路口直行
            case Car_Turn_H   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw( 200) ; break;  // 180度旋转
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    // 状态切换
    curr_Status = next_Status ;
}

// 小车状态转换台
void Car_Control_Change(void)
{
    // 当前状态和下次状态相同才能进入切换状态
    if (curr_Status != next_Status) {return;}
    switch (curr_Status) 
    {
        case Car_Forward: 
        {
            if (Road_y > 230)   // 检测到路口
            {
                if      (Target_Num == 1)           {next_Status = Car_Turn_L ;}
                else if (Target_Num == 2)           {next_Status = Car_Turn_R ;}
                else if (Target_Num == Oran_Num[0]) {next_Status = Car_Turn_L ;}
                else if (Target_Num == Oran_Num[1]) {next_Status = Car_Turn_R ;}
                // 路口直行
                else                                {next_Status = Car_Turn_F ;}
            }
            if (isRoad_T == true)    // 检测到丁字路口
            {
                isRoad_T = false ;
                if      (Target_Num == Oran_Num[0]) {next_Status = Car_Turn_L ;}
                else if (Target_Num == Oran_Num[1]) {next_Status = Car_Turn_R ;}
                else if (Target_Num == Oran_Num[2]) {next_Status = Car_Turn_L ;}
                else if (Target_Num == Oran_Num[3]) {next_Status = Car_Turn_R ;}
            }
            if (Over_y > 230)        // 检测到停车点,具体值待修改
            {
                next_Status = Car_Stop ;
            }
            break;
        }
        case Car_Turn_L : 
        {
            if (Con_MPU_Get_Yaw() > 90)
            {
                next_Status = Car_Forward ;
            }
            break;
        }
        case Car_Turn_R : 
        {
            if (Con_MPU_Get_Yaw() < -90)
            {
                next_Status = Car_Forward ;
            }
            break;
        }
        case Car_Turn_F : 
        {
            if (Road_y < 20)    // 走过路口,值待修改
            {
                next_Status = Car_Forward ;
            }
            break;
        }
        case Car_Turn_H : 
        {
            if (Con_MPU_Get_Yaw() > 180)
            {
                next_Status = Car_Forward ;
            }
            break;
        }
        case Car_Stop:
        {
            break;
        }
    }
}

