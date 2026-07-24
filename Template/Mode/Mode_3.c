// ==================== Mode_3 多功能合并 ====================
// 通过 Mode_3.h 中的 MODE3_SELECT 宏选择当前功能:
//   1 = 电机PID调参    (原Mode_3)
//   2 = 陀螺仪角度环    (原Mode_5)
//   3 = 步进电机驱动    (原Mode_6)
#include "Mode_3.h"
#include "AllHeader.h"

// ==================== 功能1: 电机PID调参 ====================
#if MODE3_SELECT == 1

// 当前选择的电机：0 = Motor A, 1 = Motor B
static uint8_t Motor_Select = 0;
static uint8_t Motor_Pos_Enable = 0;  // 0=速度环, 1=位置环(角度)

// 串口通用目标值（速度环=目标速度, 位置环=目标角度）
float Motor_PID_Goal_Check = 0;

// 获取当前选中电机的指针
static Motor_Typedef* Get_Selected_Motor(void)
{
    return (Motor_Select == 0) ? &Motor_A : &Motor_B;
}

// 获取当前选中电机的名称
static const char* Get_Motor_Label(void)
{
    return (Motor_Select == 0) ? "A" : "B";
}

static void Mode_3_Setup_Impl(void)
{
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_PID=====");
}

static void Mode_3_Loop_Impl(void)
{
    Motor_Typedef *pMotor = Get_Selected_Motor();

    // ---- 标题 + 电机 + 环类型 ----
    OLED_Printf(0, 0, OLED_6X8, "Motor_%s [%s]", Get_Motor_Label(), Motor_Pos_Enable ? "Angle" : "Speed");

    // ---- KEY_1 单击：切换电机 ----
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Motor_Select = !Motor_Select;
    }

    // ---- KEY_2 单击：切换速度环/位置环 ----
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Motor_Pos_Enable = !Motor_Pos_Enable;
    }

    // ---- 串口参数更改 ----
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        if (Motor_Pos_Enable)
        {
            // 位置环：调 PID_Angle
            Serial_SetFloatData(&Serial1, "Kp",    "Kp=%f",    &pMotor->PID_Angle.Kp);
            Serial_SetFloatData(&Serial1, "Ki",    "Ki=%f",    &pMotor->PID_Angle.Ki);
            Serial_SetFloatData(&Serial1, "Kd",    "Kd=%f",    &pMotor->PID_Angle.Kd);
            Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Motor_PID_Goal_Check);

            Motor_SetAngle(pMotor, Motor_PID_Goal_Check);
        }
        else
        {
            // 速度环：调 PID_s
            Serial_SetFloatData(&Serial1, "Kp",    "Kp=%f",    &pMotor->PID_s.Kp);
            Serial_SetFloatData(&Serial1, "Ki",    "Ki=%f",    &pMotor->PID_s.Ki);
            Serial_SetFloatData(&Serial1, "Kd",    "Kd=%f",    &pMotor->PID_s.Kd);
            Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Motor_PID_Goal_Check);

            Motor_SetSpeed(pMotor, Motor_PID_Goal_Check);
        }

        // OLED 展示当前 PID 参数
        if (Motor_Pos_Enable)
            OLED_Printf(0, 10, OLED_6X8, "K:%.2f,%.2f,%.2f", pMotor->PID_Angle.Kp, pMotor->PID_Angle.Ki, pMotor->PID_Angle.Kd);
        else
            OLED_Printf(0, 10, OLED_6X8, "K:%.2f,%.2f,%.2f", pMotor->PID_s.Kp, pMotor->PID_s.Ki, pMotor->PID_s.Kd);
    }
}

static void Mode_3_Tick_Impl(void)
{
    Motor_Typedef *pMotor = Get_Selected_Motor();

    if (Motor_Pos_Enable)
    {
        // 位置环更新（选中电机）+ 速度内环（两电机）
				if (pMotor == &Motor_A)
				{
					Motorx_Angle_Update_Tick(pMotor, 1);
				}
				else
				{
					Motorx_Angle_Update_Tick(pMotor, -1);
				}
    }

    // 每20ms通过串口打印：目标, 当前值, PID输出
    if (Motor_Pos_Enable)
        Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
                      pMotor->PID_Angle.goalPoint,
                      pMotor->PID_Angle.realPoint_Now,
                      pMotor->PID_Angle.setPoint);
    else
        Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
                      pMotor->PID_s.goalPoint,
                      pMotor->PID_s.realPoint_Now,
                      pMotor->PID_s.setPoint);
}

static void Mode_3_Exit_Impl(void)
{
    Motor_Stop(Get_Selected_Motor());
    OLED_Clear();
}

// ==================== 功能2: 陀螺仪角度环 ====================
#elif MODE3_SELECT == 2

extern uint32_t IIC_Reset_Count ;

float Angle_Car ;

static int cnt ;

static void Mode_3_Setup_Impl(void)
{
    OLED_Clear();
    PID_Angle_Reset();
}

static void Mode_3_Loop_Impl(void)
{
    OLED_Printf(0, 0, OLED_8X16, "===Mode_3_Gyro===") ;
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Angle.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Angle.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Angle.Kd) ;
        Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Angle_Car) ;
    }
    PID_Angle.goalPoint = Angle_Car ;

    OLED_Printf(0, 20, OLED_6X8, "Angle_Car:%.2f",Angle_Car) ;
    OLED_Printf(0, 50, OLED_6X8, "cnt:%d",cnt++) ;
    OLED_Printf(0, 40, OLED_6X8, "IIC_Reset_Count:%d",IIC_Reset_Count) ;
    OLED_Printf(0, 30, OLED_6X8, "Yaw:%.2f",IMU_Yaw_Abs_Get()) ;
}

static void Mode_3_Tick_Impl(void)
{
    PID_Angle_Tick();
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Angle.goalPoint ,PID_Angle.realPoint_Now ,PID_Angle.setPoint );
}

static void Mode_3_Exit_Impl(void)
{
    OLED_Clear();
}

// ==================== 功能3: 步进电机驱动 ====================
#elif MODE3_SELECT == 3

float Angle_S ;
bool Ste1_Choice = 1 ;

static void Mode_3_Setup_Impl(void)
{
    OLED_Clear();
}

static void Mode_3_Loop_Impl(void)
{
    OLED_Printf(0, 0, OLED_6X8, "===Mode3_Stepper===") ;

    Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Angle_S);

    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        if (Ste1_Choice)
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper1, (int)Angle_S, 400, 200) ;
        }
        else
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper2, (int)Angle_S, 400, 200) ;
        }
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        if (Ste1_Choice)
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper1, 0, 400, 200) ;
        }
        else
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper2, 0, 400, 200) ;
        }
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Ste1_Choice = !Ste1_Choice ;
        Stepper_PWM_Stop(&Stepper1) ;
        Stepper_PWM_Stop(&Stepper2) ;
    }
    OLED_Printf(0, 20, OLED_6X8, "S1:%f",Stepper1.Pos_Now) ;
    OLED_Printf(0, 30, OLED_6X8, "S2:%f",Stepper2.Pos_Now) ;
    OLED_Printf(0, 40, OLED_6X8, "Angle_S%d:%.2f",Ste1_Choice == 1? 1 : 2,Angle_S ) ;
    Angle_S += Encoder_Get() ;
}

static void Mode_3_Tick_Impl(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Stepper1.Pos_Tar, Stepper1.Pos_Now , Stepper1.Speed_Now);
}

static void Mode_3_Exit_Impl(void)
{
    OLED_Clear();
}

#else
#error "MODE3_SELECT 值无效! 请设置为 1(电机PID) / 2(陀螺仪角度环) / 3(步进电机)"
#endif

// ==================== 公共接口（委托到选中的功能） ====================

void Mode_3_Setup(void)
{
    Mode_3_Setup_Impl();
}

void Mode_3_Loop(void)
{
    Mode_3_Loop_Impl();
}

void Mode_3_Tick(void)
{
    Mode_3_Tick_Impl();
}

void Mode_3_Exit(void)
{
    Mode_3_Exit_Impl();
}
