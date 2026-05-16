#include "Mode_G.h"
#include "AllHeader.h"

Mode_Typedef curr_mode = Mode_Null  ;      // 当前模式
Mode_Typedef next_mode = Mode_Null  ;      // 下一个模式

bool Car_Enable  = false ;
bool Car_is_Load = false ;  // 小车装药状态
extern int next_cnt ;
extern int load_cnt ;
extern int Car_Status_Load[10] ;
extern int Car_Back_Status_Load[20] ;
extern bool isBack ;

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
    Oran_Init();
    // 定时器必须最后初始化!!!
    Timer_Init() ;
    // 日志打印
    Serial_printf(&Serial1, "\n\n\n================= Begin==================\n");
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
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num += 1 ;
    }
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num = (Target_Num == 0 ? 1 : Target_Num) ;    // 模拟目标数字
        Car_is_Load = true; // 模拟装载成功
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        isBack = true ;
        next_Status = Car_Back_Status_Load[next_cnt++] ;
    }
    if (Car_Enable == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (Car_is_Load == true && Target_Num != 0)
        {
            Car_Enable = true ;
            next_Status = Car_Forward ; // 开始行进
        }
    }
    // OLED更新
    if (curr_mode == Mode_Null) { OLED_Printf(0, 0, OLED_6X8, "=====Mode_Null=====") ; }
    OLED_Clear() ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "roa=%d,tar=%d,cnt=%d,n%d", Road_y,Target_Num,load_cnt,next_cnt) ;
    
    OLED_Printf(0, 40, OLED_6X8, "%d%d%d%d%d%d%d%d", Car_Status_Load[0],Car_Status_Load[1],Car_Status_Load[2],Car_Status_Load[3], Car_Status_Load[4],
    Car_Status_Load[5],Car_Status_Load[6],Car_Status_Load[7]) ;
    
    OLED_Printf(0, 50, OLED_6X8, "%d%d%d%d%d%d%d%d", Car_Back_Status_Load[0],Car_Back_Status_Load[1],Car_Back_Status_Load[2],Car_Back_Status_Load[3],
    Car_Back_Status_Load[4],Car_Back_Status_Load[5],Car_Back_Status_Load[6],Car_Back_Status_Load[7]) ;
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
    // 电机控制台
    Oran_Data_Update();
    Car_Control_Change() ;
    Car_Control() ;
    // 全局
    Motor_Update_Tick() ;                           // AB电机状态更新
    // MPU6050更新参数
    MPU6050_Angle_Update_Tick() ;                   // 耗时1.45ms
    // 模式选择
    if (curr_mode == Mode_PID)  { Mode_1_Tick() ;}  // 打印AB的PID参数
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
