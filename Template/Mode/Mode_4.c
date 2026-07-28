#include "Mode_4.h"
#include "AllHeader.h"

// ======================== 小车找寻终点 ========================
/*
    setup: 激活香橙派
    单击按键2 -> 开启任务:
    1. 相对旋转45度, 发送一次 "@Find_Home:1$#" 给香橙派
    2. 随后跟随香橙派巡线数据进行巡线
    3. 巡线直到 x,y 都对齐, 然后小车停车
*/

// ======================== 方案A风格：3函数（Start/Run/Tick），内部不依赖Con_Task ========================
// 后续可直接注册为单个 TASK_FIND_HOME

typedef enum {
    HOME_Init     ,   // 等待按键
    HOME_Rotate45 ,   // 旋转到相对最初角度顺时针45°的角度
    HOME_Tracking ,   // 跟随香橙派巡线
    HOME_Over     ,   // 结束
} Home_Status_Typedef;

static Home_Status_Typedef home_status = HOME_Init;
static uint32_t home_settle_ms = 0;      // 到位稳定计时（旋转和追踪共用）

// ======================== 3个对外函数（预备封装为 Con_Task） ========================

// 启动/重置状态机
void Find_Home_Start(void)
{
    // 激活香橙派:告知大模式为寻找物体
    Serial_printf(&Serial2, "@Find_Item:1$#\r\n");

    home_status    = HOME_Rotate45;
    home_settle_ms = 0;

    // ★ 就近旋转到初始方向+45°（Real.yaw会跳变，yaw_abs可能很大，用就近解绕）
    // 45°在物理空间唯一，但在yaw_abs解绕空间有多种表示(45, 405, -315...)
    // 找到最接近当前yaw_abs的那个表示 → 最短旋转路径
    float current = IMU_Yaw_Abs_Get();
    float target_world = 45.0f;
    float diff = current - target_world;
    int   N    = (diff >= 0) ? (int)(diff / 360.0f + 0.5f)
                             : (int)(diff / 360.0f - 0.5f);
    float target_abs = target_world + N * 360.0f;
    float delta = target_abs - current;

    PID_Angle_Reset();
    PID_Angle_Tar_Yaw(delta);
    Serial_printf(&Serial1, "[Mode2] -> Rotate45 (abs:%.1f->%.1f d:%.1f)\r\n", current, target_abs, delta);
}

// Loop 每帧调用，return true = 流程结束
bool Find_Home_Run(void)
{
    switch (home_status)
    {
    // ---------- 等待按键 ----------
    case HOME_Init:
        return false;   // 未开始

    // ---------- 相对旋转45° ----------
    case HOME_Rotate45:
    {
        float angle_tol = 5.0f;
        float gyro_tol  = 7.0f;

        if (IMU_Turn_Yaw_Is_Ok_Ex(PID_Angle.goalPoint, angle_tol)
            && IMU_Yaw_Gyro_Get() <= gyro_tol)
        {
            if (home_settle_ms == 0)
                home_settle_ms = Timer_Get_Ms();
            else if (Timer_Get_Ms() - home_settle_ms >= 100)   // 稳定100ms
            {
                // 到位 → 停车 + 发命令 + 进入追踪
                Motor_SetSpeed(&Motor_A, 0);
                Motor_SetSpeed(&Motor_B, 0);
                Serial_printf(&Serial2, "@Find_Home:1$#\r\n");
                Serial_printf(&Serial1, "[Mode2] -> Tracking\r\n");

                PID_Oran_X.goalPoint = 0;
                PID_Oran_Y.goalPoint = 0;
                home_settle_ms = 0;
                home_status    = HOME_Tracking;
            }
        }
        else
        {
            home_settle_ms = 0;
        }
        return false;
    }

    // ---------- 跟随香橙派巡线 ----------
    case HOME_Tracking:
    {
        float tol = 10.0f;

        if (fabsf(x_real) < tol && fabsf(y_real) < tol)
        {
            if (home_settle_ms == 0)
                home_settle_ms = Timer_Get_Ms();
            else if (Timer_Get_Ms() - home_settle_ms >= 200)  // 稳定200ms
            {
                Motor_SetSpeed(&Motor_A, 0);
                Motor_SetSpeed(&Motor_B, 0);
                Serial_printf(&Serial1, "[Mode2] -> Over\r\n");
                home_status = HOME_Over;
            }
        }
        else
        {
            home_settle_ms = 0;
        }
        return false;
    }

    // ---------- 结束 ----------
    case HOME_Over:
        return true;

    default:
        return false;
    }
}

// 20ms Tick 调用，驱动 PID
void Find_Home_Tick(void)
{
    switch (home_status)
    {
    case HOME_Rotate45:
        PID_Angle_Tick();        // 角度环 PID → 差速输出
        // 调试:
        Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Angle.goalPoint , PID_Angle.realPoint_Now , PID_Angle.setPoint) ;
        break;

    case HOME_Tracking:
        Oran_XY_PID_Update();    // 视觉巡线 PID → 差速输出
        // 调试
	    Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",PID_Oran_X.goalPoint , PID_Oran_X.realPoint_Now , PID_Oran_X.setPoint,PID_Oran_Y.goalPoint , PID_Oran_Y.realPoint_Now , PID_Oran_Y.setPoint) ; 
        break;

    default:
        break;
    }
}

// ======================== Mode_4 生命周期 ========================

void Mode_4_Setup(void)
{
    OLED_Clear();
    Serial_printf(&Serial1, "[Mode2] Init\r\n");
}

void Mode_4_Loop(void)
{
    // ★ 推进查找终点状态机
    Find_Home_Run();

    // ===== OLED 显示 =====
    OLED_Printf(0, 0, OLED_6X8, "===Mode2===");
    OLED_Printf(0, 10, OLED_6X8, "home_status:%d", home_status);
    OLED_Printf(0, 20, OLED_6X8, "X:%.1f Y:%.1f", x_real, y_real);
    OLED_Printf(0, 40, OLED_6X8, "Yaw:%.1f", IMU_Yaw_Abs_Get());

    if (home_status == HOME_Init)
        OLED_Printf(0, 50, OLED_6X8, "KEY2 to Start");
    else if (home_status == HOME_Rotate45)
        OLED_Printf(0, 50, OLED_6X8, "Rotating...");
    else if (home_status == HOME_Tracking)
        OLED_Printf(0, 50, OLED_6X8, "Tracking...");
    else if (home_status == HOME_Over)
        OLED_Printf(0, 50, OLED_6X8, "=== DONE ===");

    // ===== 按键：启动 =====
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Find_Home_Start();
    }
}

void Mode_4_Tick(void)
{
    Find_Home_Tick();
}

void Mode_4_Exit(void)
{
    OLED_Clear();
    Motor_SetSpeed(&Motor_A, 0);
    Motor_SetSpeed(&Motor_B, 0);
}
