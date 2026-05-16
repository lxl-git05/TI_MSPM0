#ifndef __MOTOR_H
#define __MOTOR_H

#include "ti_msp_dl_config.h"
#include "Encoder.h"
#include "MyPID.h"

#define Motor_MAX_Speed 320	// goal:最大转速(goal的最大值)
#define Motor_MAX_PWM   1000    // PWM:最大值
#define Encoder_PID_Gap_Time 20 // ms

// 电机正方向
#define DIR_P ( 1)
#define DIR_N (-1)

typedef enum
{
    MOTOR_STOP = 0, // 停车
    MOTOR_RUN,      // 允许发车
    MOTOR_BRAKE     // 急刹车
}Motor_State_e;

// 电机参数
typedef struct
{
    // Encoder参数
	IRQn_Type Encoder_IRQN ;        // 编码器外部中断句柄
    GPIO_Regs* Encoder_GPIO_Port;   // 编码器Port
    uint32_t Encoder_Pin_1 ;        // 编码器Pin1(双边沿)
    uint32_t Encoder_Pin_2 ;        // 编码器pin2(普通引脚)
    int32_t  EncoderCount ;         // 编码器计数,get后清零
    float    Distance ;             // 小车运行累积距离(单位:m)

    // PWM参数
	GPTIMER_Regs *PWM_INST ;            // Pin1->INST
    DL_TIMER_CC_INDEX PWM_Channel_1 ;   // Pin1->PWM引脚
	
    GPIO_Regs *IN1_Port ; 				// Pin1->电机IN1引脚类型,PWM引脚
	uint32_t IN1_Pin  ;					// Pin1->电机IN1引脚号

	GPIO_Regs *IN2_Port ; 				// Pin2->电机IN2引脚类型,普通引脚
	uint32_t IN2_Pin  ;					// Pin2->电机IN2引脚号

	// 电机参数
	float PPR;                          // 编码器线数
	float ReductionRatio;               // 减速比
	int8_t Goal_Speed_Dir;              // goalSpeed修正方向,因为A4950有In1和In2,所以两个正反接会有不同的结果,所以相同的goal会有正反不同的PWM方向,所以需要修正
	int8_t Encoder_Dir ;				// Encoder修正方向,也就是如果正转轮子发现是负的就将该参数调为负值即可
	
    // PID参数:含有GoalSpeed,RealSpeed,SetSpeed!
	Pid_Typedef PID_s ;					// PID参数

    // 状态参数
    Motor_State_e State ;
	
}Motor_Typedef ;

extern Motor_Typedef Motor_A ;
extern Motor_Typedef Motor_B ;

// MotorA初始化
void Motor_A_Init(void);
void Motor_B_Init(void);

// 20ms更新一次
void Motorx_Update_Tick(Motor_Typedef *Motor) ;

void Motor_SetPWM(Motor_Typedef *Motor , int PWM) ;

#endif
