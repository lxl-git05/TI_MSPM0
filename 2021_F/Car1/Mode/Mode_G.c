#include "Mode_G.h"
#include "AllHeader.h"

Mode_Typedef curr_mode = Mode_Null  ;     // 当前模式
Mode_Typedef next_mode = Mode_Null  ;      // 下一个模式

float stop_y = 230;

typedef enum
{
    Car_Forward ,
    Car_Turn ,
}Car_Status_Typedef ;

Car_Status_Typedef curr_Status = Car_Forward;
Car_Status_Typedef next_Status = Car_Forward;

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
    Oran_Init();
    // 定时器必须最后初始化!!!
    Timer_Init() ;
}

// 循环loop
void Mode_G_Loop(void)
{
    // 检测程序是否可行
    if (Key_Check(KEY_0, KEY_SINGLE))// 单击
    {
        Flash_Mode_Set(Flash_Mode_Fast) ;   
    }
    // 进入下一个模式
    if (Key_Check(KEY_0, KEY_DOUBLE))// 双击
    {   
        Mode_To_Next() ;
    }
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Oran_Go();
    }
    // OLED更新
    if (curr_mode == Mode_Null) { OLED_Printf(0, 0, OLED_6X8, "=====Mode_Null=====") ; }
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 40, OLED_6X8, "roa=%d  ", Road_y) ;
}

// ========================== 系统定时器配置 ==========================

// 1ms定时器
void Timer_0_Callback(void)
{
    // 功能1:按键
    Key_Tick() ;

    // 功能2:LED闪烁监控
    Flash_Mode_Tick() ;
}

// 小车控制台
void Car_Control(void)
{
    if (curr_Status == next_Status)
    {
        if (curr_Status == Car_Forward)
        {
            Oran_Track_Tick() ;
            // 寻迹切换到转向
            if (Road_y > 230)   // 检测到路口
            {
                next_Status = Car_Turn ;
            }
        }
        else if (curr_Status == Car_Turn)
        {
            Con_MPU_Motor_Tick() ;
            // 转向切换到寻迹
            if (Con_MPU_Get_Yaw() > 90)
            {
                next_Status = Car_Forward ;
            }
        }
    }
    // 这里一定要用if,因为条件改变是在==的条件下完成的,否则下一刻curr会与next相等,导致永远到不了!=
    if (curr_Status != next_Status)
    {
        if (next_Status == Car_Turn)
        {
            Con_MPU_Yaw_Reset() ;   // 先将当前角度置零
            Con_MPU_Tar_Yaw(100) ;  // 目标为向左旋转100度
        }
    }

    curr_Status = next_Status ;
}

// 20ms定时器
void Timer_1_Callback(void)
{
    // 全局
    Motor_Update_Tick() ;   // AB电机状态更新

    Car_Control() ;

    // MPU6050更新参数
    MPU6050_Angle_Update_Tick() ;   // 耗时1.45ms

    // 模式选择
    if (curr_mode == Mode_PID)   { Mode_1_Tick() ;} // 打印AB的PID参数
    if (curr_mode == Mode_Angle) { Mode_2_Tick() ;} // 陀螺仪控制角度
    if (curr_mode == Mode_Track) { Mode_3_Tick() ;} // 小车自行计算路程寻迹
    
}












// ========================== 系统状态配置 ==========================

// 进入下一状态
void Mode_To_Next(void)
{
    // Mode_End纯属标记模式尽头防止越界
    next_mode = (next_mode + 1) == Mode_End ? Mode_Null : next_mode + 1 ;
}

// 将当前状态转换为:
void Mode_ChangeTo(Mode_Typedef nextmode)
{
    if (nextmode >= Mode_End) { return;}  

    next_mode = nextmode ;
}
