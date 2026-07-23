#include "Mode_4.h"
#include "AllHeader.h"

// ==================== 任务表（按 Con_Task.h 枚举顺序） ====================
static const Task_Descriptor_Typedef Mode4_TaskTable[TASK_COUNT] = {
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
};

// ==================== 预设演示序列 ====================
static void Mode4_Enqueue_Demo(void)
{
    // 1. 等待 1s（蜂鸣器响）
    Con_Task_Enqueue(TASK_WAIT_TIME, 200, 0, 0, 0);
    // // 2. 电机A 转360°
    // Con_Task_Enqueue(TASK_MOTOR_A_ANGLE, 360, 20, 0, 0);
    // // 3. 等待 0.5s
    // Con_Task_Enqueue(TASK_WAIT_TIME, 500, 0, 0, 0);
    // // 4. 电机B 转-180°（反向半圈）
    // Con_Task_Enqueue(TASK_MOTOR_B_ANGLE, -180, 20, 0, 0);
    // // 5. 等待 0.5s
    // Con_Task_Enqueue(TASK_WAIT_TIME, 500, 0, 0, 0);
    // // 6. 步进电机1转90°（自动完成后再执行下一个）
    // Con_Task_Enqueue(TASK_STEPPER1_ANGLE, 90, 0, 0, 0);
    // // 7. 步进电机2转90°
    // Con_Task_Enqueue(TASK_STEPPER2_ANGLE, 90, 0, 0, 0);
    // // 8. 小车顺时针旋转180°
    // Con_Task_Enqueue(TASK_CAR_YAW, 180, 0, 0, 0);
}

// ==================== Mode 4 生命周期 ====================

void Mode_4_Setup(void)
{
    OLED_Clear();

    // 注册任务表
    Con_Task_Init(Mode4_TaskTable, TASK_COUNT);
    Serial_printf(&Serial1, "Mode4\r\n") ;

    // 预设演示序列
    Mode4_Enqueue_Demo();
}

void Mode_4_Loop(void)
{
    // ★ 推进任务状态机（必须调用）
    Con_Task_Loop();

    // ===== OLED 状态显示 =====
    OLED_Printf(0, 0, OLED_8X16, "===Mode_4===");

    if (Con_Task_IsBusy())
    {
        OLED_Printf(0, 20, OLED_6X8, "Task:%d Q:%d",
            Con_Task_CurrType(), Con_Task_Remaining());

        // 显示当前任务类型名
        switch (Con_Task_CurrType())
        {
            case TASK_WAIT_TIME:      OLED_Printf(0, 30, OLED_6X8, ">>> Wait");      break;
            case TASK_MOTOR_A_ANGLE:  OLED_Printf(0, 30, OLED_6X8, ">>> MotorA Ang"); break;
            case TASK_MOTOR_B_ANGLE:  OLED_Printf(0, 30, OLED_6X8, ">>> MotorB Ang"); break;
            case TASK_STEPPER1_ANGLE: OLED_Printf(0, 30, OLED_6X8, ">>> Stepper1");   break;
            case TASK_STEPPER2_ANGLE: OLED_Printf(0, 30, OLED_6X8, ">>> Stepper2");   break;
            case TASK_CAR_YAW:        OLED_Printf(0, 30, OLED_6X8, ">>> Car Yaw");    break;
            default: break;
        }
    }
    else if (Con_Task_Remaining() == 0)
    {
        OLED_Printf(0, 20, OLED_6X8, "All Done!");
    }

    // ===== 按键控制 =====
    // Key1 单击: 重新入队演示序列
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        // 8. 小车顺时针旋转180°
        Con_Task_Enqueue(TASK_CAR_YAW, 180, 0, 0, 0);
        Con_Task_Enqueue(TASK_WAIT_TIME, 1000, 0, 0, 0);
    }
}

void Mode_4_Tick(void)
{
    Con_Task_Tick();
}

void Mode_4_Exit(void)
{
    
}
