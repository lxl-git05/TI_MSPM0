#ifndef __CONTROL_H
#define __CONTROL_H

#include "AllHeader.h"

// ==================== 全局共享任务表 ====================
// ★ 所有 Con_Mode 统一引用此表，Con_Task_Init(Control_TaskTable, TASK_COUNT)
extern const Task_Descriptor_Typedef Control_TaskTable[TASK_COUNT];

// 1. 任务1：等待xs，然后Exit（伴随蜂鸣器）
// TASK_WAIT_TIME: p[0]=等待时间(ms)
void Task_Wait_Time_Setup(float p[4]) ;
bool Task_Wait_Time_IsExit(float p[4]) ;

// 2. 任务2：电机A旋转特定角度，旋转完成之后停止，Exit
// TASK_MOTOR_A_ANGLE: p[0]=旋转角度°, p[1]=容差°
void Task_Motor_A_Angle_Setup(float p[4]) ;
bool Task_Motor_A_Angle_IsExit(float p[4]) ;
void Task_Motor_A_Angle_Tick(float p[4]) ;

// 3. 任务3：电机B旋转特定角度，旋转完成之后停止，Exit
// TASK_MOTOR_B_ANGLE: p[0]=旋转角度°, p[1]=容差°
void Task_Motor_B_Angle_Setup(float p[4]) ;
bool Task_Motor_B_Angle_IsExit(float p[4]) ;
void Task_Motor_B_Angle_Tick(float p[4]) ;

// 4. 任务4：步进电机1旋转特定角度，旋转完成之后停止，Exit
// TASK_STEPPER1_ANGLE: p[0]=目标角度°, p[1]=max_speed(0=默认200), p[3]=acc(0=默认200)
void Task_Stepper1_Angle_Setup(float p[4]) ;
bool Task_Stepper1_Angle_IsExit(float p[4]) ;

// 5. 任务5：步进电机2旋转特定角度，旋转完成之后停止，Exit
// TASK_STEPPER2_ANGLE: p[0]=目标角度°, p[1]=max_speed(0=默认200), p[3]=acc(0=默认200)
void Task_Stepper2_Angle_Setup(float p[4]) ;
bool Task_Stepper2_Angle_IsExit(float p[4]) ;

// 6. 任务6：小车顺时针/逆时针旋转一定角度然后Exit（相对运动，不归零yaw）
// TASK_CAR_YAW: p[0]=相对增量角度°(+顺时针/-逆时针), p[1]=角度容差°(0=默认5°), p[2]=角速度容差°/s(0=默认7°/s)
void Task_Car_Yaw_Setup(float p[4]) ;
void Task_Car_Yaw_Tick(float p[4]) ;
bool Task_Car_Yaw_IsExit(float p[4]) ;

// 7. 任务7：香橙派视觉寻迹追踪
// TASK_ORAN_TRACK: p[0]=goal_x, p[1]=goal_y, p[2]=容差(默认10), p[3]=超时ms(0=不限)
void Task_Oran_Track_Setup(float p[4]) ;
void Task_Oran_Track_Tick(float p[4]) ;
bool Task_Oran_Track_IsExit(float p[4]) ;

#endif
