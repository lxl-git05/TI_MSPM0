// Menu_Param.c — 调参任务菜单（参照 Control.c: 枚举→回调→任务表→Con_Task）
// 按键: KEY_1单击=下一项, KEY_1长按=入队/再次长按=Skip
//  Tick: 所有任务20ms输出调试数据到Serial1（PID:goal/real/set, GyroCal:biasX/Y/Z, Stepper:Tar/Now/Speed）
// Serial1: Kp/Ki/Kd/Goal 修改PID, Speed/Pos/Stop 控制步进
#include "AllHeader.h"
#include "Menu_Param.h"

// ==================== 菜单显示名（用于OLED）====================
typedef struct {
    const char *cat;   // 分类 "Motor_A"
    const char *name;  // 任务 "Speed"
} TuneLabel;

static const TuneLabel s_labels[TUNE_COUNT] = {
    { "Motor_A", "Speed"    },  // TUNE_MOTOR_A_SPEED
    { "Motor_A", "Angle"    },  // TUNE_MOTOR_A_ANGLE
    { "Motor_A", "Pos"      },  // TUNE_MOTOR_A_POS
    { "Motor_B", "Speed"    },  // TUNE_MOTOR_B_SPEED
    { "Motor_B", "Angle"    },  // TUNE_MOTOR_B_ANGLE
    { "Motor_B", "Pos"      },  // TUNE_MOTOR_B_POS
    { "Car",     "Straight" },  // TUNE_CAR_STRAIGHT
    { "Gyro",    "YawPID"   },  // TUNE_GYRO_YAW
    { "Gyro",    "Cal"      },  // TUNE_GYRO_CAL
    { "Stepper", "S1"       },  // TUNE_STEPPER_S1
    { "Stepper", "S2"       },  // TUNE_STEPPER_S2
};

// ==================== 菜单状态 ====================
static int8_t s_cursor = 0;  // 当前浏览位置

// Gyro_Cal 状态机
static int      s_cal_state = 0;    // 0=等待按键, 1=等待稳定, 2=完成
static uint32_t s_cal_timer = 0;    // 计时器

// ==================== 辅助：导航 ====================
#define NEXT_CURSOR(c)  (((c) + 1) % TUNE_COUNT)

// ==================== 通用 OLED 渲染：PID 值 ====================
static void OLED_ShowPID(const char *cat, const char *name, Pid_Typedef *pid)
{
    OLED_Printf(0, 0,  OLED_6X8, "%-7s %-8s", cat, name);
    OLED_Printf(0, 10, OLED_6X8, "Kp:%.2f Ki:%.2f", pid->Kp, pid->Ki);
    OLED_Printf(0, 20, OLED_6X8, "Kd:%.2f", pid->Kd);
    OLED_Printf(0, 30, OLED_6X8, "Goal:%.1f", pid->goalPoint);
    OLED_Printf(0, 40, OLED_6X8, "Real:%.1f", pid->realPoint_Now);
    OLED_Printf(0, 50, OLED_6X8, "Set:%.1f L:Back", pid->setPoint);
}

// ==================== 通用 Serial1 路由：PID（返回 Goal 是否变化）====================
static bool Serial_RoutePID(Pid_Typedef *pid)
{
    if (!Serial_GetNewPackageFlag_ABC(&Serial1)) return false;
    float old = pid->goalPoint;
    Serial_SetFloatData(&Serial1, "Kp",   "Kp=%f",   &pid->Kp);
    Serial_SetFloatData(&Serial1, "Ki",   "Ki=%f",   &pid->Ki);
    Serial_SetFloatData(&Serial1, "Kd",   "Kd=%f",   &pid->Kd);
    Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &pid->goalPoint);
    return (pid->goalPoint != old);
}

// ==================== TUNE_MOTOR_A_SPEED ====================
void Tune_MotorA_Speed_Setup(float p[4]) { Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint); }
void Tune_MotorA_Speed_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_A.PID_s))
        Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint);
    OLED_ShowPID("Motor_A", "Speed", &Motor_A.PID_s);
}
void Tune_MotorA_Speed_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Motor_A.PID_s.goalPoint , Motor_A.PID_s.realPoint_Now , Motor_A.PID_s.setPoint) ;
}

// ==================== TUNE_MOTOR_A_ANGLE ====================
void Tune_MotorA_Angle_Setup(float p[4]) { Motor_SetAngle(&Motor_A, Motor_A.PID_Angle.goalPoint); }
void Tune_MotorA_Angle_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_A.PID_Angle))
        Motor_SetAngle(&Motor_A, Motor_A.PID_Angle.goalPoint);
    OLED_ShowPID("Motor_A", "Angle", &Motor_A.PID_Angle);
}
void Tune_MotorA_Angle_Tick(float p[4])
{
    Motorx_Angle_Update_Tick(&Motor_A, 1);
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_A.PID_Angle.goalPoint, Motor_A.PID_Angle.realPoint_Now, Motor_A.PID_Angle.setPoint);
}

// ==================== TUNE_MOTOR_A_POS ====================
void Tune_MotorA_Pos_Setup(float p[4]) { Motor_SetPos(&Motor_A, Motor_A.PID_Pos.goalPoint); }
void Tune_MotorA_Pos_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_A.PID_Pos))
        Motor_SetPos(&Motor_A, Motor_A.PID_Pos.goalPoint);
    OLED_ShowPID("Motor_A", "Pos", &Motor_A.PID_Pos);
}
void Tune_MotorA_Pos_Tick(float p[4])
{
    Motorx_Pos_Update_Tick(&Motor_A, 1);
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_A.PID_Pos.goalPoint, Motor_A.PID_Pos.realPoint_Now, Motor_A.PID_Pos.setPoint);
}

// ==================== TUNE_MOTOR_B_SPEED ====================
void Tune_MotorB_Speed_Setup(float p[4]) { Motor_SetSpeed(&Motor_B, Motor_B.PID_s.goalPoint); }
void Tune_MotorB_Speed_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_B.PID_s))
        Motor_SetSpeed(&Motor_B, Motor_B.PID_s.goalPoint);
    OLED_ShowPID("Motor_B", "Speed", &Motor_B.PID_s);
}
void Tune_MotorB_Speed_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_B.PID_s.goalPoint, Motor_B.PID_s.realPoint_Now, Motor_B.PID_s.setPoint);
}

// ==================== TUNE_MOTOR_B_ANGLE ====================
void Tune_MotorB_Angle_Setup(float p[4]) { Motor_SetAngle(&Motor_B, Motor_B.PID_Angle.goalPoint); }
void Tune_MotorB_Angle_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_B.PID_Angle))
        Motor_SetAngle(&Motor_B, Motor_B.PID_Angle.goalPoint);
    OLED_ShowPID("Motor_B", "Angle", &Motor_B.PID_Angle);
}
void Tune_MotorB_Angle_Tick(float p[4])
{
    Motorx_Angle_Update_Tick(&Motor_B, -1);
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_B.PID_Angle.goalPoint, Motor_B.PID_Angle.realPoint_Now, Motor_B.PID_Angle.setPoint);
}

// ==================== TUNE_MOTOR_B_POS ====================
void Tune_MotorB_Pos_Setup(float p[4]) { Motor_SetPos(&Motor_B, Motor_B.PID_Pos.goalPoint); }
void Tune_MotorB_Pos_Run(float p[4])
{
    if (Serial_RoutePID(&Motor_B.PID_Pos))
        Motor_SetPos(&Motor_B, Motor_B.PID_Pos.goalPoint);
    OLED_ShowPID("Motor_B", "Pos", &Motor_B.PID_Pos);
}
void Tune_MotorB_Pos_Tick(float p[4])
{
    Motorx_Pos_Update_Tick(&Motor_B, -1);
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_B.PID_Pos.goalPoint, Motor_B.PID_Pos.realPoint_Now, Motor_B.PID_Pos.setPoint);
}

// ==================== TUNE_CAR_STRAIGHT ====================
void Tune_Car_Straight_Setup(float p[4])
{
    PID_Car_Straight_Reset();
}
void Tune_Car_Straight_Run(float p[4])
{
    if (Serial_RoutePID(&PID_Car_Straight))
    {
        PID_Car_Straight_Reset();
        // goalPoint 已被 Serial_RoutePID 更新，Reset 后会保持
    }
    OLED_ShowPID("Car", "Straight", &PID_Car_Straight);
}
void Tune_Car_Straight_Tick(float p[4])
{
    PID_Car_Straight_Tick();
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        PID_Car_Straight.goalPoint, PID_Car_Straight.realPoint_Now, PID_Car_Straight.setPoint);
}

// ==================== TUNE_GYRO_YAW ====================
void Tune_Gyro_Yaw_Setup(float p[4]) { PID_Angle_Reset(); }
void Tune_Gyro_Yaw_Run(float p[4])
{
    Serial_RoutePID(&PID_Angle);  // Goal变化时只需更新goalPoint，Tick自动处理
    OLED_ShowPID("Gyro", "YawPID", &PID_Angle);
    Serial_printf(&Serial1, "Yaw:%.1f Tar:%.1f Out:%.1f\n",
        IMU_Mahony_Real.yaw, PID_Angle.goalPoint, PID_Angle.setPoint);
}
void Tune_Gyro_Yaw_Tick(float p[4])
{
    PID_Angle_Tick();
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        PID_Angle.goalPoint, PID_Angle.realPoint_Now, PID_Angle.setPoint);
}

// ==================== TUNE_GYRO_CAL ====================
void Tune_Gyro_Cal_Setup(float p[4])
{
    s_cal_state = 0;
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "Gyro Bias:");
}
void Tune_Gyro_Cal_Run(float p[4])
{
    // 展示当前bias三大参数
    OLED_Printf(0, 10, OLED_6X8, "Bx:%.4f        ", IMU_Mahony_GyroBiasX);
    OLED_Printf(0, 20, OLED_6X8, "By:%.4f        ", IMU_Mahony_GyroBiasY);
    OLED_Printf(0, 30, OLED_6X8, "Bz:%.4f        ", IMU_Mahony_GyroBiasZ);

    if (s_cal_state == 0)
    {
        OLED_Printf(0, 50, OLED_6X8, "K2:Cal K1:Back");
        if (Key_Check(KEY_2, KEY_SINGLE))
        {
            s_cal_state = 1;
            s_cal_timer = Timer_Get_Ms();
        }
    }
    else if (s_cal_state == 1)
    {
        OLED_Printf(0, 50, OLED_6X8, "Wait 1s stable.");
        if (Timer_Get_Ms() - s_cal_timer >= 1000)
        {
            OLED_Printf(0, 40, OLED_6X8, "Calibrating... ");
            OLED_Update();

            Timer_DisableIRQ();
            IMU_Mahony_Calibrate(1000);
            Timer_EnableIRQ();

            Timer_DisableIRQ();
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasX);
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasY);
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasZ);
            Timer_EnableIRQ();

            OLED_Printf(0, 40, OLED_6X8, "IMU_OK!        ");
            s_cal_state = 0;
        }
    }
}
bool Tune_Gyro_Cal_IsExit(float p[4]) { return (s_cal_state == 2); }
void Tune_Gyro_Cal_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.4f,%.4f,%.4f\n",
        IMU_Mahony_GyroBiasX, IMU_Mahony_GyroBiasY, IMU_Mahony_GyroBiasZ);
}

// ==================== TUNE_STEPPER_S1 / S2 ====================
static void Stepper_Run(Stepper_PWM_Typedef *stp, const char *name)
{
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        float v = 0;
        if (Serial_SetFloatData(&Serial1, "Speed", "Speed=%f", &v))
            Stepper_PWM_Speed_Set(stp, v, 200);
        if (Serial_SetFloatData(&Serial1, "Pos",   "Pos=%f",   &v))
            Stepper_PWM_Pos_Set_Abs(stp, v, 400, 200);
        if (Serial_CheckCmd(&Serial1, "Stop"))
            Stepper_PWM_Stop(stp);
    }

    OLED_Printf(0, 0,  OLED_6X8, "Stepper %s", name);
    OLED_Printf(0, 10, OLED_6X8, "Pos:%.1f", stp->Pos_Now);
    OLED_Printf(0, 20, OLED_6X8, "Tar:%.1f", stp->Pos_Tar);
    OLED_Printf(0, 30, OLED_6X8, "Speed:%.0f", stp->Speed_Now);
    OLED_Printf(0, 50, OLED_6X8, "L:Back             ");
}

void Tune_Stepper_S1_Run(float p[4]) { Stepper_Run(&Stepper1, "S1"); }
void Tune_Stepper_S2_Run(float p[4]) { Stepper_Run(&Stepper2, "S2"); }
void Tune_Stepper_S1_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.1f,%.1f,%.0f\n",
        Stepper1.Pos_Tar, Stepper1.Pos_Now, Stepper1.Speed_Now);
}
void Tune_Stepper_S2_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.1f,%.1f,%.0f\n",
        Stepper2.Pos_Tar, Stepper2.Pos_Now, Stepper2.Speed_Now);
}

// ==================== 通用：永不自动退出 ====================
bool Tune_AlwaysFalse(float p[4]) { return false; }

// ==================== 任务描述表（同 Control_TaskTable）====================
Task_Descriptor_Typedef Menu_Tune_Table[TUNE_COUNT] = {
    // TUNE_MOTOR_A_SPEED
    { Tune_MotorA_Speed_Setup, Tune_MotorA_Speed_Run, Tune_AlwaysFalse, Tune_MotorA_Speed_Tick },
    // TUNE_MOTOR_A_ANGLE
    { Tune_MotorA_Angle_Setup, Tune_MotorA_Angle_Run, Tune_AlwaysFalse, Tune_MotorA_Angle_Tick },
    // TUNE_MOTOR_A_POS
    { Tune_MotorA_Pos_Setup,   Tune_MotorA_Pos_Run,   Tune_AlwaysFalse, Tune_MotorA_Pos_Tick },
    // TUNE_MOTOR_B_SPEED
    { Tune_MotorB_Speed_Setup, Tune_MotorB_Speed_Run, Tune_AlwaysFalse, Tune_MotorB_Speed_Tick },
    // TUNE_MOTOR_B_ANGLE
    { Tune_MotorB_Angle_Setup, Tune_MotorB_Angle_Run, Tune_AlwaysFalse, Tune_MotorB_Angle_Tick },
    // TUNE_MOTOR_B_POS
    { Tune_MotorB_Pos_Setup,   Tune_MotorB_Pos_Run,   Tune_AlwaysFalse, Tune_MotorB_Pos_Tick },
    // TUNE_CAR_STRAIGHT
    { Tune_Car_Straight_Setup, Tune_Car_Straight_Run, Tune_AlwaysFalse, Tune_Car_Straight_Tick },
    // TUNE_GYRO_YAW
    { Tune_Gyro_Yaw_Setup,     Tune_Gyro_Yaw_Run,     Tune_AlwaysFalse, Tune_Gyro_Yaw_Tick },
    // TUNE_GYRO_CAL
    { Tune_Gyro_Cal_Setup,     Tune_Gyro_Cal_Run,     Tune_Gyro_Cal_IsExit, Tune_Gyro_Cal_Tick },
    // TUNE_STEPPER_S1
    { NULL,                    Tune_Stepper_S1_Run,   Tune_AlwaysFalse, Tune_Stepper_S1_Tick },
    // TUNE_STEPPER_S2
    { NULL,                    Tune_Stepper_S2_Run,   Tune_AlwaysFalse, Tune_Stepper_S2_Tick },
};

// ==================== 菜单浏览 OLED ====================
static void Menu_Render(void)
{
    OLED_Printf(0, 0, OLED_6X8, "===Tune_Menu======");

    // 显示当前光标附近的4个任务
    int8_t start = s_cursor;
    // 确保能看到前后
    if (start > 0) start--;
    if (start > TUNE_COUNT - 4) start = TUNE_COUNT - 4;
    if (start < 0) start = 0;

    for (uint8_t i = 0; i < 4; i++) {
        int8_t idx = start + i;
        if (idx >= TUNE_COUNT) break;
        char c = (idx == s_cursor) ? '>' : ' ';
        OLED_Printf(0, 10 + i * 10, OLED_6X8, "%c%-7s %-8s",
                    c, s_labels[idx].cat, s_labels[idx].name);
    }

    OLED_Printf(0, 50, OLED_6X8, "LONG:Active CLK:Next");
}

// ==================== API ====================

void Menu_Tune_Init(void)
{
    s_cursor = 0;
    Con_Task_Init(Menu_Tune_Table, TUNE_COUNT);
}

int Menu_Tune_Cursor(void) { return s_cursor; }

void Menu_Tune_Loop(void)
{
    // Con_Task_Loop 必须每帧调用（负责出队→Setup→Run→IsExit）
    Con_Task_Loop();

    if (Con_Task_IsBusy())
    {
        // ---- 任务运行中：Run 回调已处理 OLED + Serial1 ----
        if (Key_Check(KEY_1, KEY_LONG))
            Con_Task_Skip();
    }
    else
    {
        // ---- 浏览模式：KEY_1单击下一项, KEY_1长按入队 ----
        if (Key_Check(KEY_1, KEY_SINGLE))
            s_cursor = NEXT_CURSOR(s_cursor);

        if (Key_Check(KEY_1, KEY_LONG))
            Con_Task_Enqueue(s_cursor, 0, 0, 0, 0);

        Menu_Render();
    }
}
