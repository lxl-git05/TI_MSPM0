#include "Mode_G.h"
#include "AllHeader.h"

Mode_Typedef curr_mode = Mode_Null   ;       // 当前模式
Mode_Typedef next_mode = Mode_2      ;        // 下一个模式

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
    // 定时器必须最后初始化!!!
    Timer_Initial() ;
}

// 循环loop — 全局按键 + 模式分发
void Mode_G_Loop(void)
{
    // ===== 全局按键（所有模式共享）=====
    // Key2 单击: 切换 LED 闪烁模式
    if (Key_Check(KEY_0, KEY_SINGLE))
    {
        Flash_Mode_Set(Flash_Mode_Fast) ;
    }
    // Key2 双击: 进入下一个模式
    if (Key_Check(KEY_0, KEY_DOUBLE))
    {
        Mode_To_Next() ;
    }

    // ===== 模式分发 =====
    if (curr_mode == next_mode)
    {
        // 运行当前模式的 Loop
        switch (curr_mode)
        {
            case Mode_1: Mode_1_Loop(); break;
            case Mode_2: Mode_2_Loop(); break;
            case Mode_3: Mode_3_Loop(); break;
            default: break;
        }
    }
    else
    {
        // 退出旧模式
        switch (curr_mode)
        {
            case Mode_1: Mode_1_Exit(); break;
            case Mode_2: Mode_2_Exit(); break;
            case Mode_3: Mode_3_Exit(); break;
            default: break;
        }
        // 进入新模式
        switch (next_mode)
        {
            case Mode_1: Mode_1_Setup(); break;
            case Mode_2: Mode_2_Setup(); break;
            case Mode_3: Mode_3_Setup(); break;
            default: break;
        }
        curr_mode = next_mode ;
    }
}

// ========================== 系统定时器配置 ==========================

// 1ms定时器
void Timer_1ms_Callback(void)
{
    // 功能1:按键
    Key_Tick() ;
    // 功能2:LED闪烁监控
    Flash_Mode_Tick() ;
}

// 20ms定时器
void Timer_20ms_Callback(void)
{
    // 当前模式的 Tick
    switch (curr_mode)
    {
        case Mode_1: Mode_1_Tick(); break;
        case Mode_2: Mode_2_Tick(); break;
        case Mode_3: Mode_3_Tick(); break;
        default: break;
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

// ====================================================================
// GROUP1 中断总入口 — 所有GPIO端口(GPIOA/GPIOB等)共享此中断向量
// 通过 IIDX 区分具体是哪个GPIO端口触发的中断
// ====================================================================
void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        // GPIOB端口中断 — 编码器A/B电机测速
        case GPIO_MULTIPLE_GPIOB_INT_IIDX:
            MyEncoder_ISR(&Motor_A_Encoder);
            MyEncoder_ISR(&Motor_B_Encoder);
            break;

        // 其他GROUP1中断源在此扩展...

        default:
            break;
    }
}
