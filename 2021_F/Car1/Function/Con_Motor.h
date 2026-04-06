#ifndef __CON_MOTOR_H
#define __CON_MOTOR_H

#include "Motor.h"
#include "ti_msp_dl_config.h"

// 初始化
void Motor_Init(void) ;

// 设置电机速度
void Motor_SetSpeed(Motor_Typedef *Motor, float speed);

// 电机停止
void Motor_Stop(Motor_Typedef *Motor);

// 电机急刹
void Motor_Brake(Motor_Typedef *Motor);

#endif
