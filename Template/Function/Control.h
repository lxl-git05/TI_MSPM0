#ifndef __CONTROL_H
#define __CONTROL_H

#include "AllHeader.h"

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

// 4. 任务4：步进电机1、2旋转特定角度，旋转完成之后停止，Exit
// TASK_STEPPER_ANGLE: p[0]=Stepper1角度°, p[1]=Stepper2角度°
void Task_Stepper_Angle_Setup(float p[4]) ;
bool Task_Stepper_Angle_IsExit(float p[4]) ;

#endif
