#include "Mode_G.h"
#include "AllHeader.h"
#include "BLE.h"

extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern int Car_1_Target_Num ; 

// #define MPU6050_Check 

Mode_Typedef curr_mode = Mode_Null  ;       // 当前模式
Mode_Typedef next_mode = Mode_Con_3  ;       // 下一个模式

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
    Oran_Init();
    // 定时器必须最后初始化!!!
    Timer_Init() ;
    // 小车控制系统初始化
    Car_Init() ;
    // 日志打印
    Serial_printf(&Serial1, "\n\n\n================= Begin ==================\n");
}

// 循环loop
void Mode_G_Loop(void)
{
    // 检测程序是否可行
    if (Key_Check(KEY_0, KEY_SINGLE))// 单击
    {
        Flash_Mode_Set(Flash_Mode_Fast) ;  
        #ifdef MPU6050_Check
            MPU6050_Data_Error_Check(1000) ;
        #endif
    }
    // 进入下一个模式
    if (Key_Check(KEY_0, KEY_DOUBLE))// 双击
    {
        Mode_To_Next() ;
    }
    if (curr_mode == Mode_Null) { OLED_Printf(0, 0, OLED_6X8, "=====Mode_Null=====") ; }
    // =========================== 测试代码 ===========================
    
    
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
    // 通信数据更新
    Oran_Data_Update();
    // 电机运动转换状态
    if (curr_mode == Mode_Con_2)
    {
        Car_Control_Change_1() ;
        Car_Control() ;
    }
    else if (curr_mode == Mode_Con_3)
    {
        Car_Control_Change_TiGao_1() ;
        Car_Control() ;
    }
    else if (curr_mode == Mode_Con_1)
    {
        Car_Control_Change_TiGao_2() ;
        Car_Control() ;
    }
    else if (curr_mode == Mode_Angle) 
    {
        Oran_Track_Tick(0) ;
    }
    // 全局
    Motor_Update_Tick() ;                           // AB电机状态更新
    // MPU6050更新参数
    #ifndef MPU6050_Check 
        MPU6050_Angle_Update_Tick() ;   // 耗时1.45ms
    #endif                  
    // 模式选择
    if (curr_mode == Mode_PID)  { Mode_1_Tick() ;}  // 打印AB的PID参数
    if (curr_mode == Mode_Angle)
    {
        OLED_Printf(0, 10, OLED_6X8,"r=%.2f,s=%.2f",PID_Track.realPoint_Now ,PID_Track.setPoint);
    }
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
