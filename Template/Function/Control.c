#include "Control.h"

// 实现各大逻辑的动作存储
// 1. 任务1: 等待xs，然后Exit
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
// TASK_Motor_Angle:p[0]为旋转角度 p[1]为容忍角度误差
void Task_Motor_Angle_Setup(float p[4]) 
{

}
bool Task_Motor_Angle_IsExit(float p[4]) 
{
    
    return false;
}

void Task_Motor_Angle_Tick(float p[4]) 
{
    
}

// 任务3: 


