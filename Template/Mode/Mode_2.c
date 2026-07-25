#include "Mode_2.h"
#include "AllHeader.h"

int Begin = 0 ;

void Mode_2_Setup(void)
{
    // 任务初始化
    Con_Task_Init(Control_TaskTable, TASK_COUNT);
    Serial_printf(&Serial1, "Mode2_Check\r\n") ;
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8,"Mode2") ;
    // 推进任务状态机（必须调用）
    Con_Task_Loop();

    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Begin = 1 ;
    }
    if (Begin == 0)
    {
        return;
    }

    OLED_Printf(0, 10, OLED_6X8,"%d",isHomeIn) ;
    // 家没有入画并且没有任务
    if (Con_Task_Remaining() == 0 && isHomeIn == false)
    {
        PID_Angle.OutMax = 20 ;
        PID_Angle.OutMin =-20 ;
        Con_Task_Enqueue(TASK_CAR_YAW, 360, 2, 2, 0);            // 顺时针慢速旋转360度
    }
    else if (Con_Task_CurrType() == TASK_CAR_YAW && isHomeIn == true)
    {
        // 终止任务
        Con_Task_Skip() ;
        Motor_SetSpeed(&Motor_A, 0);
        Motor_SetSpeed(&Motor_B, 0);
    }
    else if (Con_Task_Remaining() == 0 && isHomeIn == true)
    {
        // TASK_ORAN_TRACK 退出条件: |x|<tol && |y|<tol 稳定200ms
        // 若已到达目标中心附近（±5容差），停止；否则继续追踪逼近
        if (x_real > -5.0f && x_real < 5.0f && y_real > -5.0f && y_real < 5.0f)
        {
            Motor_SetSpeed(&Motor_A, 0);
            Motor_SetSpeed(&Motor_B, 0);
            // 已到达，不再入队任务，小车静止
        }
        else
        {
            Con_Task_Enqueue(TASK_ORAN_TRACK , 0 , 0 , 10 , 0) ;
        }
    }
    else if (Con_Task_CurrType() == TASK_ORAN_TRACK && isHomeIn == false)
    {
        // 丢画: 取消追踪 → 重新自转寻找
        Con_Task_Skip() ;
        Motor_SetSpeed(&Motor_A, 0);
        Motor_SetSpeed(&Motor_B, 0);
        PID_Angle.OutMax = 20 ;
        PID_Angle.OutMin =-20 ;
        Con_Task_Enqueue(TASK_CAR_YAW, 360, 2, 2, 0);            // 顺时针慢速旋转360度
    }
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        // 终止任务
        Con_Task_Skip() ;
    }
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%d,%d\n",Con_Task_CurrType() , isHomeIn) ;
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
