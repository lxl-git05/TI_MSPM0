#include "Con_MPU.h"
#include "MPU6050_Angle.h"

Pid_Typedef PID_Angle ; 
#define Angle_MAX_Speed 60  // 小车转向环最大偏移速度

// 初始化MPU控制参数
void Con_MPU_Init(void)
{
    PID_Init(&PID_Angle, 0.66f, 0.0f, 0.0f, 60, -60, 1000) ;
}

// 20ms让小车旋转固定角度
void Con_MPU_Motor_Tick(void)
{
    // 1. 得到真实角度(yaw)
    PID_Angle.realPoint_Now = MPU_Real.yaw ;

    // 3. PID计算
    PID_Update(&PID_Angle, PID_Angle.realPoint_Now ) ;

    // 4. 输出小车转速, 差速
    Motor_SetSpeed(&Motor_A, - PID_Angle.setPoint) ;    // A B 差速不要搞岔
    Motor_SetSpeed(&Motor_B,   PID_Angle.setPoint) ;

    // 5. 展示效果
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Angle.goalPoint ,PID_Angle.realPoint_Now ,PID_Angle.setPoint );
}

// 重置MPU的yaw
void Con_MPU_Yaw_Reset(void)
{
    MPU_Real.yaw = 0 ;
}

// 配置yaw的方位
void Con_MPU_Tar_Yaw(int Traget_Yaw)
{
    PID_Angle.goalPoint = Traget_Yaw ;
}

// 得到当前yaw角度
int Con_MPU_Get_Yaw(void)
{
    return MPU_Real.yaw ;
}
