#ifndef __CON_MOTOR_H
#define __CON_MOTOR_H

#include "ti_msp_dl_config.h"
#include "Motor.h"

// 初始化
void Motor_Init(void) ;

// 20ms更新
void Motor_Update_Tick(void) ;

// 设置电机速度
void Motor_SetSpeed(Motor_Typedef *Motor, float speed);

// 电机停止
void Motor_Stop(Motor_Typedef *Motor);

// 电机急刹
void Motor_Brake(Motor_Typedef *Motor);

// 得到电机目标速度
int Motor_Get_GoalSpeed(Motor_Typedef *Motor) ;

#endif
