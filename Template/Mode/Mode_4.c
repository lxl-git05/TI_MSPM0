#include "Mode_4.h"
#include "AllHeader.h"

// ★ 任务表已迁移到 Control.c 作为全局共享表，直接引用即可
// 新任务只需在 Control.c 的 Control_TaskTable 中注册

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

    // 注册全局共享任务表
    Con_Task_Init(Control_TaskTable, TASK_COUNT);
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
            case TASK_ORAN_TRACK:     OLED_Printf(0, 30, OLED_6X8, ">>> Oran Track");  break;
            case TASK_CAR_STRAIGHT:  OLED_Printf(0, 30, OLED_6X8, ">>> Car Straight");break;
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
        Con_Task_Enqueue(TASK_CAR_YAW, 90, 2, 2, 0);            // 顺时针旋转90度
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        // 小车向前走1m(100cm)
        Con_Task_Enqueue(TASK_CAR_STRAIGHT, 50, 10, 0, 0);
        Con_Task_Enqueue(TASK_CAR_YAW, 90, 2, 2, 0);            // 顺时针旋转90度
        Con_Task_Enqueue(TASK_CAR_STRAIGHT, 50, 10, 0, 0);
        Con_Task_Enqueue(TASK_CAR_YAW, 90, 2, 2, 0);            // 顺时针旋转90度
        Con_Task_Enqueue(TASK_CAR_STRAIGHT, 50, 10, 0, 0);
        Con_Task_Enqueue(TASK_CAR_YAW, 90, 2, 2, 0);            // 顺时针旋转90度
        Con_Task_Enqueue(TASK_CAR_STRAIGHT, 50, 10, 0, 0);
        Con_Task_Enqueue(TASK_CAR_YAW, 90, 2, 2, 0);            // 顺时针旋转90度
    }

    OLED_Printf(0, 30, OLED_6X8, "S1:%s",Serial1.ABC_Data.Serial_New_Package_ABC) ;
    OLED_Printf(0, 40, OLED_6X8, "S2:%s",Serial2.ABC_Data.Serial_New_Package_ABC) ;

    OLED_Printf(0 , 50, OLED_6X8, "S1:%x",Serial1.rxLen) ;
    OLED_Printf(40 , 50, OLED_6X8, "S2:%x",Serial2.rxLen) ;
    OLED_Printf(70, 50, OLED_6X8, "Yaw:%.3f",IMU_Yaw_Abs_Get()) ;
}

void Mode_4_Tick(void)
{
	Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f\n",PID_Angle.goalPoint , PID_Angle.realPoint_Now , PID_Angle.setPoint,MPU_Yaw_Abs_Get()) ;
}

void Mode_4_Exit(void)
{
    
}
