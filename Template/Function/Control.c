#include "Control.h"

// 实现各大逻辑的动作存储
// 1. 任务1: 等待(x)ms，然后Exit
// TASK_WAIT_TIME: p[0]=等待时间(ms)
void Task_Wait_Time_Setup(float p[4])
{
	p[2] = Timer_Get_Ms() ;	// 开始计时
	Buzzer_ON() ;
}
	
bool Task_Wait_Time_IsExit(float p[4])
{
	if (Timer_Get_Ms() - p[2] > p[0])
	{
		Buzzer_OFF() ;
		return true ;
	}
	return false ;
}

// 2. 任务2:电机A旋转特定角度,旋转完成之后停止,Exit 
// TASK_Motor_A_Angle: p[0]为旋转角度 p[1]为容忍角度误差
void Task_Motor_A_Angle_Setup(float p[4])
{
	Motor_SetAngle(&Motor_A , p[0]) ;
}

bool Task_Motor_A_Angle_IsExit(float p[4])
{
	// 判断静止条件
	if (Motor_Is_Angle(&Motor_A , p[0] , p[1] , 5.0f))	// Speed_Tol=5rpm
	{
		Motor_SetSpeed(&Motor_A , 0) ;
		return true ;
	}
	return false ;
}

void Task_Motor_A_Angle_Tick(float p[4])
{
	Motorx_Angle_Update_Tick(&Motor_A , 1) ;	// A是正的
} 

// 3. 任务3:电机B旋转特定角度,旋转完成之后停止,Exit 
// TASK_Motor_B_Angle: p[0]为旋转角度 p[1]为容忍角度误差
void Task_Motor_B_Angle_Setup(float p[4])
{
	Motor_SetAngle(&Motor_B , p[0]) ;
}

bool Task_Motor_B_Angle_IsExit(float p[4])
{
	// 判断静止条件
	if (Motor_Is_Angle(&Motor_B , p[0] , p[1] , 5.0f))	// Speed_Tol=5rpm
	{
		Motor_SetSpeed(&Motor_B , 0) ;
		return true ;
	}
	return false ;
}

void Task_Motor_B_Angle_Tick(float p[4])
{
	Motorx_Angle_Update_Tick(&Motor_B , -1) ;	// B要反一下
} 

// 4. 任务4:步进电机1、2旋转特定角度,旋转完成之后停止,Exit
// Task_Stepper_Angle: p[0]为电机1旋转角度 p[1]为电机2旋转角度 
void Task_Stepper_Angle_Setup(float p[4])
{
	// 先计时
	p[2] = Timer_Get_Ms() ;
	// 配置角度
	Stepper_PWM_Pos_Set_Abs(&Stepper1 , p[0] , 400 , 200) ;
	Stepper_PWM_Pos_Set_Abs(&Stepper2 , p[1] , 400 , 200) ;
}

bool Task_Stepper_Angle_IsExit(float p[4])
{
	// 正式代码
	if (Stepper_PWM_Is_Angle() && Timer_Get_Ms() - p[2] > 500)
	{
		// 到达目标位置之后停止，进入下个模式
		Stepper_PWM_Stop(&Stepper1) ;
		Stepper_PWM_Stop(&Stepper2) ;
		return true ;
	}
	return false ;
}


