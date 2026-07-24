#include "Control.h"

// ==================== 全局共享任务表 ====================
// ★ 所有 Con_Mode 统一引用此表: Con_Task_Init(Control_TaskTable, TASK_COUNT)
// ★ 新任务只需在此表中注册即可被所有模式使用
const Task_Descriptor_Typedef Control_TaskTable[TASK_COUNT] = {
    [TASK_WAIT_TIME] = {
        .Setup  = Task_Wait_Time_Setup,
        .IsExit = Task_Wait_Time_IsExit,
    },
    [TASK_MOTOR_A_ANGLE] = {
        .Setup  = Task_Motor_A_Angle_Setup,
        .Tick   = Task_Motor_A_Angle_Tick,     // 20ms PID 更新
        .IsExit = Task_Motor_A_Angle_IsExit,
    },
    [TASK_MOTOR_B_ANGLE] = {
        .Setup  = Task_Motor_B_Angle_Setup,
        .Tick   = Task_Motor_B_Angle_Tick,     // 20ms PID 更新
        .IsExit = Task_Motor_B_Angle_IsExit,
    },
    [TASK_STEPPER1_ANGLE] = {
        .Setup  = Task_Stepper1_Angle_Setup,
        .IsExit = Task_Stepper1_Angle_IsExit,
    },
    [TASK_STEPPER2_ANGLE] = {
        .Setup  = Task_Stepper2_Angle_Setup,
        .IsExit = Task_Stepper2_Angle_IsExit,
    },
    [TASK_CAR_YAW] = {
        .Setup  = Task_Car_Yaw_Setup,
        .Tick   = Task_Car_Yaw_Tick,       // 20ms PID 更新
        .IsExit = Task_Car_Yaw_IsExit,
    },
    [TASK_ORAN_TRACK] = {
        .Setup  = Task_Oran_Track_Setup,
        .Tick   = Task_Oran_Track_Tick,    // 20ms 寻迹PID更新
        .IsExit = Task_Oran_Track_IsExit,
    },
};

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

// 6. 任务6: 小车顺时针/逆时针旋转一定角度然后Exit（相对运动，不归零yaw）
// TASK_CAR_YAW: p[0]=相对旋转角度°(+顺时针/-逆时针), p[1]=角度容差°(0=默认5°), p[2]=角速度容差°/s(0=默认7°/s)
void Task_Car_Yaw_Setup(float p[4])
{
	// 记录当前yaw为基准 + 清空PID历史（不归零MPU_Real.yaw）
	PID_Angle_Reset();
	// 设置相对增量目标（goalPoint = startYaw + delta）
	PID_Angle_Tar_Yaw(p[0]);
}

void Task_Car_Yaw_Tick(float p[4])
{
	// MPU更新→PID计算→差速输出
	PID_Angle_Tick();
}

bool Task_Car_Yaw_IsExit(float p[4])
{
	float angle_tol = (p[1] > 0.0f) ? p[1] : 5.0f;
	// 注意：检查绝对目标值 PID_Angle.goalPoint（= startYaw + delta），而非增量 p[0]
	if (IMU_Turn_Yaw_Is_Ok_Ex(PID_Angle.goalPoint, angle_tol))
	{
		Motor_SetSpeed(&Motor_A, 0);
		Motor_SetSpeed(&Motor_B, 0);
		return true;
	}
	return false;
}

// 7. 取放物资
// Task_Elec: p[0]=等待时间(ms)
void Task_Elec_Setup(float p[4])
{
	// 开始计时
	p[1] = Timer_Get_Ms() ;	
	// 直接开启蜂鸣器，指示正在取/放棋子
	Buzzer_ON() ;					
	// 开始取/放
	if (MyGPIO_ReadPin(&MyGPIO_Elec))	// 正在吸附->那就放下
	{
		MyGPIO_WritePin(&MyGPIO_Elec , 0) ;
	}
	else	// 为0，也就是没在吸附,那就开吸
	{
		MyGPIO_WritePin(&MyGPIO_Elec , 1) ;
	}
}

bool Task_Elec_IsExit(float p[4])
{
	if (Timer_Get_Ms() - p[1] > p[0])
	{
		Buzzer_OFF() ;
		return true ;
	}
	return false ;
}

// 7. 任务7: 香橙派视觉寻迹追踪
// TASK_ORAN_TRACK: p[0]=goal_x, p[1]=goal_y, p[2]=容差(默认10), p[3]=超时ms(0=不限)
static uint32_t OranTrack_StartMs  = 0;
static uint32_t OranTrack_ArriveMs = 0;

void Task_Oran_Track_Setup(float p[4])
{
    // 设置寻迹目标
    PID_Oran_X.goalPoint = p[0];
    PID_Oran_Y.goalPoint = p[1];

    // 记录开始时刻
    OranTrack_StartMs  = Timer_Get_Ms();
    OranTrack_ArriveMs = 0;
}

void Task_Oran_Track_Tick(float p[4])
{
    // 20ms: 读取视觉数据 → PID计算 → 电机差速驱动
    Oran_XY_PID_Update();
}

bool Task_Oran_Track_IsExit(float p[4])
{
    float tol = (p[2] > 0.0f) ? p[2] : 10.0f;

    // 超时检测
    if (p[3] > 0.0f && Timer_Get_Ms() - OranTrack_StartMs > (uint32_t)p[3])
    {
        Motor_SetSpeed(&Motor_A, 0);
        Motor_SetSpeed(&Motor_B, 0);
        return true;
    }

    // 位置到达检测（需持续200ms稳定在容差内）
    if (x_real > p[0] - tol && x_real < p[0] + tol &&
        y_real > p[1] - tol && y_real < p[1] + tol)
    {
        if (OranTrack_ArriveMs == 0)
            OranTrack_ArriveMs = Timer_Get_Ms();
        if (Timer_Get_Ms() - OranTrack_ArriveMs > 200)
        {
            Motor_SetSpeed(&Motor_A, 0);
            Motor_SetSpeed(&Motor_B, 0);
            return true;
        }
    }
    else
    {
        OranTrack_ArriveMs = 0;  // 离开容差区则重置计时
    }

    return false;
}


