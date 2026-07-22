#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    // 测量 OLED_Update() 耗时
    Timer_Counter_Begin();
    
    OLED_Printf(0, 16, OLED_6X8, "OLED:%5d us  ", (int)time_us);
    OLED_Printf(0, 32, OLED_6X8, "Loop:%5d us  ", (int)time_Func_us);
    OLED_Update();
    
    // 测量帧间隔
    Timer_Counter_End();
}

void Mode_2_Tick(void)
{
    // 20ms — 暂无需求
    Timer_Counter_Func();
}

void Mode_2_Exit(void)
{
    OLED_Clear();
    OLED_Update();
}
