#ifndef __CON_MOTOR_H
#define __CON_MOTOR_H

#include "MySystem.h"
#include "Motor.h"

extern Motor_Typedef Motor_A ;
extern Motor_Typedef Motor_B ;

// 1. 电机初始化
void Con_Motor_Init(void) ;

// 2. 设置电机goal速度
void Motor_SetSpeed(Motor_Typedef *Motor, float speed) ;

// 3. 得到电机goal速度
int Motor_Get_GoalSpeed(Motor_Typedef *Motor) ;

// 4. 电机停止
void Motor_Stop(Motor_Typedef *Motor) ;

// 5. 电机急刹
void Motor_Brake(Motor_Typedef *Motor) ;

// 6.1 电机速度更新与PID控制
void Motor_Speed_Update_Tick(uint32_t Gap_Time_ms) ;

// 6.2 电机角度环PID,并不需要知道周期,但是仍然需要放在需要周期定时器内
void Motorx_Angle_Update_Tick(Motor_Typedef *Motor , int Dir);	// Dir: 纠正PID控制方向

// 8. 设置电机旋转角度
void Motor_SetAngle(Motor_Typedef *Motor , int Angle); 

// 9. 得到电机当前位置
float Motor_Get_Angle(Motor_Typedef *Motor) ;

// 10. 检查电机位置（三重检查：状态+速度+角度容差，Speed_Tol 单位 rpm）
bool Motor_Is_Angle(Motor_Typedef *Motor , float Angle , float Tolerance , float Speed_Tol) ;

//// Pos函数
//// 1. 设置电机目标位移
//void Motor_SetPos(Motor_Typedef *Motor , float Pos) ;

//// 2. 得到电机当前位移
//float Motor_Get_Pos(Motor_Typedef *Motor) ;

//// 3. 检查电机位置
//bool Motor_Is_Pos(Motor_Typedef *Motor , int Pos , int Tolerance) ;

//// 4. 清除累计位移
//void Motor_Pos_Clear(void) ;

// =================== MPU6050角度环 ===================

extern Pid_Typedef PID_Angle ;	// 小车的yaw角度环

void PID_Angle_Init(void) ;         // 初始化角度环PID（Kp=6, Kd=20, Out±100）
void PID_Angle_Reset(void) ;        // 记录当前yaw为基准 + 清空PID历史（不归零，纯相对运动）
void PID_Angle_Tar_Yaw(float delta);// 配置相对增量角度（+顺时针/-逆时针，基于Reset时刻的yaw基准）
float PID_Angle_Get_Yaw(void) ;     // 获取相对yaw角度（当前值 - 起始基准）
void PID_Angle_Tick(void) ;         // 20ms Tick: MPU更新→PID→差速输出(A-/B+)

//// 位置环
//// ================= 电机编码器双轮外环 =================

//// 初始化
//void PID_ALL_Pos_Init(void) ;
//// 配置目标位移
//void PID_ALL_Pos_Set_Goal(int Goal_Pos) ;
//// 角度环配置
//float PID_ALL_Pos_Tick(void) ;

//void PID_ALL_Pos_Reset(void) ;

#endif
