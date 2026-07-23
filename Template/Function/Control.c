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

// 4. 任务4:步进电机1旋转特定角度,旋转完成之后停止,Exit
// TASK_STEPPER1_ANGLE: p[0]=目标角度°, p[1]=max_speed(0=默认200), p[3]=acc(0=默认200)
void Task_Stepper1_Angle_Setup(float p[4])
{
	Buzzer_OFF();	// 减载
	Stepper_PWM_Stop(&Stepper1);
	// 提取参数（默认 max_speed=200, acc=200）
	float max_spd = (p[1] > 0.0f) ? p[1] : 200.0f;
	float acc_val = (p[3] > 0.0f) ? p[3] : 200.0f;
	// 计时
	p[2] = Timer_Get_Ms() ;
	// 启动
	Stepper_PWM_Pos_Set_Abs(&Stepper1 , p[0] , max_spd , acc_val) ;
}

bool Task_Stepper1_Angle_IsExit(float p[4])
{
	if (Stepper_PWM_Is_Angle_Stepper(&Stepper1) && Timer_Get_Ms() - p[2] > 500)
	{
		Stepper_PWM_Stop(&Stepper1) ;
		return true ;
	}
	return false ;
}

// 5. 任务5:步进电机2旋转特定角度,旋转完成之后停止,Exit
// TASK_STEPPER2_ANGLE: p[0]=目标角度°, p[1]=max_speed(0=默认200), p[3]=acc(0=默认200)
void Task_Stepper2_Angle_Setup(float p[4])
{
	Buzzer_OFF();	// 减载
	Stepper_PWM_Stop(&Stepper2);
	// 提取参数（默认 max_speed=200, acc=200）
	float max_spd = (p[1] > 0.0f) ? p[1] : 200.0f;
	float acc_val = (p[3] > 0.0f) ? p[3] : 200.0f;
	// 计时
	p[2] = Timer_Get_Ms() ;
	// 启动
	Stepper_PWM_Pos_Set_Abs(&Stepper2 , p[0] , max_spd , acc_val) ;
}

bool Task_Stepper2_Angle_IsExit(float p[4])
{
	if (Stepper_PWM_Is_Angle_Stepper(&Stepper2) && Timer_Get_Ms() - p[2] > 500)
	{
		Stepper_PWM_Stop(&Stepper2) ;
		return true ;
	}
	return false ;
}


