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

// 2. 任务2:电机旋转特定角度,旋转完成之后停止,Exit 
// TASK_Motor_Angle:p[0]为电机选择:0x3->两个 0x1->电机A 0x2->电机B p[1]为旋转角度 p[2]为容忍角度误差
// void Task_Motor_Angle_Setup(float p[4])
// {
// 	Motor_SetAngle(&Motor_A , p[1]) ;
// }

// bool Task_Motor_Angle_IsExit(float p[4])
// {
// 	// 判断静止条件
// 	if (Motor_Is_Angle(&Motor_A , p[1] , p[2] , 5.0f))	// Speed_Tol=5rpm
// 	{
// 		Motor_SetSpeed(&Motor_A , 0) ;
// 		return true ;
// 	}
// 	return false ;
// }

// void Task_Motor_Angle_Tick(float p[4])
// {
// 	Motorx_Angle_Update_Tick(&Motor_A , 1) ;
// } 

// 任务3: 


