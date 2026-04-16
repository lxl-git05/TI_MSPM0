#include "Mode_G.h"
#include "AllHeader.h"

Mode_Typedef curr_mode = Mode_Null  ;     // 当前模式
Mode_Typedef next_mode = Mode_Null ;     // 下一个模式

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
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
    // OLED更新
    if (curr_mode == Mode_Null) { OLED_Printf(0, 0, OLED_6X8, "=====Mode_Null=====") ; }
    
    OLED_Update();
    
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

// 20ms定时器
void Timer_1_Callback(void)
{
    // 全局
    Motor_Update_Tick() ;   // AB电机状态更新

    // MPU6050更新参数
    MPU6050_Angle_Update_Tick() ;   // 耗时1.45ms

    // 模式选择
    if (curr_mode == Mode_PID)   { Mode_1_Tick() ;} // 打印AB的PID参数
    if (curr_mode == Mode_Angle) { Mode_2_Tick() ;} // 陀螺仪控制角度
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
