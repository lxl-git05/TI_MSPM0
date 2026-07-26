#include "Mode_6.h"
#include "AllHeader.h"

// ======================== 回家对齐驱动 ========================

typedef enum 
{
    Init    ,   // 初始化,等待开始
    No_First,   // 第1次没找到
    No_Item ,   // 其他次没找到
    Find_Item,  // 物品入画
    Get_Item,   // 开始夹取
    Over ,      // 结束
}Item_status_Typedef;

Item_status_Typedef Item_Status = Init ;

// 搜索子阶段：No_First 状态内部分段旋转
typedef enum {
    Search_Rotate_90  ,   // 第一步：逆时针转90°
    Search_Rotate_360 ,   // 第二步：逆时针转360°
    Search_Done       ,   // 搜索完成（未找到）
} Search_Phase_Typedef;

static Search_Phase_Typedef Search_Phase = Search_Rotate_90;

void Mode_6_Setup(void)
{
    OLED_Clear();
    Oran_XY_Init() ;
    // 初始化任务队列（清空残留任务）
    Con_Task_Init(Control_TaskTable, TASK_COUNT);
    Serial_printf(&Serial1, "[Mode6] Init\r\n");
    PID_Angle_Speed_Low_On() ;
}

void Mode_6_Loop(void)
{
    // ★ 推进任务队列状态机（必须首行）
    Con_Task_Loop();

    // ===== OLED 显示 =====
    OLED_Printf(0, 0, OLED_6X8, "===Mode6===") ;
    OLED_Printf(0, 10, OLED_6X8, "St:%d Ph:%d It:%d",
        Item_Status, Search_Phase, Oran_Item[0]) ;
    OLED_Printf(0, 20, OLED_8X16, "X:%.1f Y:%.1f",x_real , y_real) ;
    OLED_Printf(0, 40, OLED_8X16, "Yaw:%.1f",IMU_Yaw_Abs_Get()) ;

    // ===== 按键：启动搜索 =====
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Item_Status = No_First ;
        Search_Phase = Search_Rotate_90;
        Serial_printf(&Serial1, "[Mode6] -> No_First\r\n");
    }
    if (Item_Status == Init)    // 未开始
    {
        OLED_Printf(0, 50, OLED_6X8, "KEY2 to Start") ;
        return;
    }

    // ======================== 状态机 ========================

    // ★ 所有搜索/旋转状态：一旦摄像头看到物品，立即中断 → PID逼近
    if (Item_Status == No_First)
    {
        if (Oran_Item[0] > 0)
        {
            Con_Task_Skip();                        // 终止当前旋转任务
            Motor_SetSpeed(&Motor_A, 0);            // 立即停车
            Motor_SetSpeed(&Motor_B, 0);
            Con_Task_Enqueue(TASK_ORAN_TRACK, 0, 0, 10, 0);  // goal=(0,0), tol=10
            Item_Status = Find_Item;
            Serial_printf(&Serial1, "[Mode6] -> Find_Item\r\n");
        }
    }

    switch (Item_Status)
    {
    // ---------- 第一次搜索：自转找物品 ----------
    case No_First:
        OLED_Printf(0, 50, OLED_6X8, "Searching...") ;

        // 任务空闲时入队下一个旋转动作
        if (!Con_Task_IsBusy())
        {
            switch (Search_Phase)
            {
            case Search_Rotate_90:
                Con_Task_Enqueue(TASK_CAR_YAW, -90, 5, 0, 0);   // 逆时针90°
                Search_Phase = Search_Rotate_360;
                break;

            case Search_Rotate_360:
                Con_Task_Enqueue(TASK_CAR_YAW, -360, 5, 0, 0);  // 逆时针360°
                Search_Phase = Search_Done;
                break;

            case Search_Done:
                // 全部转完仍未找到 → 停车等待
                Motor_SetSpeed(&Motor_A, 0);
                Motor_SetSpeed(&Motor_B, 0);
                Item_Status = No_Item;
                Serial_printf(&Serial1, "[Mode6] -> No_Item\r\n");
                break;
            }
        }
        break;

    // ---------- 未找到：停车等待 ----------
    case No_Item:
        OLED_Printf(0, 50, OLED_6X8, "No Item. Wait.") ;
        break;

    // ---------- 找到物品：PID 逼近 ----------
    case Find_Item:
        OLED_Printf(0, 50, OLED_6X8, "Approaching...") ;
        if (!Con_Task_IsBusy())     // TASK_ORAN_TRACK 已完成
        {
            Con_Task_Enqueue(TASK_WAIT_TIME, 500, 0, 0, 0);  // 电磁铁吸附等待
            Elec_ON();
            Item_Status = Get_Item;
            Serial_printf(&Serial1, "[Mode6] -> Get_Item\r\n");
        }
        break;

    // ---------- 夹取物品 ----------
    case Get_Item:
        OLED_Printf(0, 50, OLED_6X8, "Grabbing...") ;
        if (!Con_Task_IsBusy())     // 等待完成
        {
            Item_Status = Over;
            Serial_printf(&Serial1, "[Mode6] -> Over\r\n");
        }
        break;

    // ---------- 结束 ----------
    case Over:
        OLED_Printf(0, 50, OLED_6X8, "=== DONE ===") ;
        break;

    default:
        break;
    }
}

void Mode_6_Tick(void)
{
    // 调试窗口
	// Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",PID_Oran_X.goalPoint , PID_Oran_X.realPoint_Now , PID_Oran_X.setPoint,PID_Oran_Y.goalPoint , PID_Oran_Y.realPoint_Now , PID_Oran_Y.setPoint) ; 
}

void Mode_6_Exit(void)
{
    OLED_Clear();
    Motor_SetSpeed(&Motor_A, 0);
    Motor_SetSpeed(&Motor_B, 0);
    Elec_OFF();
}
