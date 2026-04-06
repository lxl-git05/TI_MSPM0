#include "Motor.h"
#include "PWM.h"

Motor_Typedef Motor_A ;

// ===================== 设定Motor_A相关参数 =====================
void Motor_A_Init(void)
{
    // ============ 参数配置 ============
    // Encoder参数
	Motor_A.Encoder_IRQN = GPIO_MULTIPLE_GPIOB_INT_IRQN ;
    Motor_A.Encoder_GPIO_Port = GPIO_ENCODER_A_PORT ;
    Motor_A.Encoder_Pin_1 = GPIO_ENCODER_A_A_Encoder_1_PIN ;
    Motor_A.Encoder_Pin_2 = GPIO_ENCODER_A_A_Encoder_2_PIN ;
    Motor_A.EncoderCount = 0 ;

	// PWM参数
	Motor_A.PWM_INST   	  = PWM_MOTOR_PWM_INST ;		
	Motor_A.PWM_Channel_1 = GPIO_PWM_MOTOR_PWM_C0_IDX  ;    // 默认是0对应A的PWM,1对应B的
	
	// Motor_A.IN1_Port = Motor_A_IN1_GPIO_Por;
	// Motor_A.IN1_Pin	 = Motor_A_IN1_Pin ;
	
	Motor_A.IN2_Port = GPIO_MOTOR_IN2_PORT	;
	Motor_A.IN2_Pin	 = GPIO_MOTOR_IN2_AIN2_PIN ;
	
    // 电机参数
	Motor_A.PPR = 13.0f ;
	Motor_A.ReductionRatio = 28.0f ;
	
	Motor_A.DIR = DIR_N ;	 // 方向判断
	Motor_A.Encoder_Dir = 1; // 编码器正方向
	
    // PID参数
    PID_Init(&Motor_A.PID_s , 8.0f , 0.80f , 0.0f , 1000 , -1000 , 1000) ;

    // 状态参数
    Motor_A.State = MOTOR_STOP;

    // ============ 函数初始化 ============
    // Encoder初始化
	Encoder_Init(Motor_A.Encoder_IRQN) ;

	// 额外功能
	// Motor_A.PID_s.deadspace = 3.0f ;	// 输出死区
}

// ===================== 功能代码 =====================

// 1. 计算真实速度: 更新Motor的真实速度,得到的值直接写入Motor
void Motor_Speed_Update(Motor_Typedef *Motor)
{
	// 得到总脉冲数
	int Motor_CNT = Encoder_Get_CNT(&Motor->EncoderCount) * Motor->Encoder_Dir;
	
	// 转速n = 总脉冲数/2倍频/单圈脉冲数(13)/减速比(28)/采样时间 , Encoder_PID_Gap_Time暂时为20ms
	// Motor->Motor_RealSpeed = (float)Motor_CNT / 2 / 13 / 28 / Encoder_Gap_Time * 1000 ; ,直接算出来:4*13*28/1000=1.456
	// 特别关注倍频参数! 2 or 4
    Motor->PID_s.realPoint_Now = (float)Motor_CNT * 60 * 1000 / (2.0f * Motor->PPR * Motor->ReductionRatio) / Encoder_PID_Gap_Time  ;
}

// 1. 计算真实速度: 编码器速度更新
void GROUP1_IRQHandler(void)
{
    // === 必须先判断是不是 GPIOB 的中断 ===
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case GPIO_MULTIPLE_GPIOB_INT_IIDX:
            Encoder_Counter_Tick(Motor_A.Encoder_GPIO_Port,Motor_A.Encoder_Pin_1,Motor_A.Encoder_Pin_2,&Motor_A.EncoderCount);
            break;

        // 如果以后还有其他 GPIOB 的中断，可以继续加 case
        default:
            break;
    }
}

// 4. 输出setpoint: 设置PWM,幕后执行的速度逻辑(setPoint)
void Motor_SetPWM(Motor_Typedef *Motor , int PWM)
{
	// 限制最值
	if (PWM >= Motor_MAX_PWM)
	{
		PWM = Motor_MAX_PWM ;
	}
	else if (PWM <= -Motor_MAX_PWM)
	{
		PWM = -Motor_MAX_PWM ;
	}
	// 判断方向,设置速度
	if (PWM >= 0)
	{
        DL_GPIO_clearPins(Motor->IN2_Port, Motor->IN2_Pin);    // 低电平
        PWM_SetCompare(Motor->PWM_INST , Motor->PWM_Channel_1 , PWM ) ; // 变大	 ,差值变大
	}
	else
	{
        DL_GPIO_setPins(Motor->IN2_Port, Motor->IN2_Pin);   // 高电平
        PWM_SetCompare(Motor->PWM_INST , Motor->PWM_Channel_1 , Motor_MAX_PWM + PWM ) ; // 变小	 ,差值变大
	}
}

/*
核心逻辑:Motor速度更新(放在20ms定时器)
    1. 计算真实速度
    2. 状态机控制模式
    3. PID计算setpoint
    4. 输出setpoint(定时器需要最后声明,否则PWM还没初始化就调用PWM会出现bug)
*/
void Motor_Update_Tick(Motor_Typedef *Motor)
{
    // 1. 计算真实速度（编码器）
    Motor_Speed_Update(Motor) ;

    // 2. 状态机控制
    switch (Motor->State)
    {
        case MOTOR_STOP:
            Motor->PID_s.goalPoint = 0;
            break;

        case MOTOR_RUN:
            break;

        case MOTOR_BRAKE:
            Motor->PID_s.goalPoint = 0;
            Motor->PID_s.setPoint = 0;
            Motor_SetPWM(Motor, 0);
            return;
    }

    // 3. PID计算
    PID_Update(&Motor->PID_s , Motor->PID_s.realPoint_Now) ;

    // 4. 输出PWM
    Motor_SetPWM(Motor, Motor->PID_s.setPoint);
}
