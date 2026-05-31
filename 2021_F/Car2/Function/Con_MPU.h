#ifndef __CON_MPU_H
#define __CON_MPU_H

#include "ti_msp_dl_config.h"
#include "MyPID.h"

extern Pid_Typedef PID_Angle ; 

// 初始化MPU控制参数
void Con_MPU_Init(void) ;

// 20ms让小车旋转固定角度
void Con_MPU_Motor_Tick(void) ;

// 重置MPU的yaw
void Con_MPU_Yaw_Reset(void) ;

// 配置yaw的方位
void Con_MPU_Tar_Yaw(int Traget_Yaw) ;

// 得到当前yaw角度
int Con_MPU_Get_Yaw(void) ;

// 接力旋转180度
void Con_MPU_Motor_Turn180_Tick(void) ;

#endif
