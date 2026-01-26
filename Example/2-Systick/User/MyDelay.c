#include "MyDelay.h"

// 全局系统时间
uint32_t total_ms = 0 ;

// ========================= 简单延时 =========================
// 精度有限,谨慎使用
void Delay_ms_cyc(uint32_t ms)
{
    while (ms--) 
    {
        delay_cycles(CPUCLK_FREQ / 1000 ) ;
    }
    
}

void Delay_us_cyc(uint32_t us)
{
    while (us--) 
    {
        delay_cycles(CPUCLK_FREQ / 1000000) ;
    }
}
// 使用滴答计时器延时
void Delay_ms_tim(uint32_t ms)
{
    uint32_t time_goal = total_ms + ms ;
    while (total_ms <= time_goal) ;
}


// ========================= 函数运行计时(us) =========================
// 运行计时时间
uint32_t time_fuc_us  = 0 ;        // 一个函数两次调用间隔的时间
uint32_t time_line_us = 0 ;        // 两行之间运行时间
uint32_t time_line_ms_start = 0 ;   // 两行运行时间起始时间
// 取样配置
int Sample_Times = 100 ;
int Sample_Flag = 0 ;
int Sample_Cnt = 0 ;

void Timer_Cnt_Begin(int Sample_times)
{
    if (Sample_Flag == 0)
    {
        Sample_Flag = 1 ;
        time_line_ms_start = total_ms ;
        if (Sample_times <= 0 || Sample_times > 1000)
        {
            Sample_Times = 100;
            Sample_Cnt = Sample_Times ;
        }
        else 
        {
            Sample_Times = Sample_times ;
            Sample_Cnt = Sample_Times ;
        }
    }
}

void Timer_Cnt_End(void)
{
    if (Sample_Flag == 1)
    {
        Sample_Cnt -- ;
        if (Sample_Cnt == 0)
        {
            time_line_us = (total_ms - time_line_ms_start) * 1000 / Sample_Times ;
            Sample_Flag = 0 ;
            Sample_Times = 100 ;
            Sample_Cnt = 0 ;
        }
    }
}

void Timer_Cnt_Fuc(int Sample_Times)
{
    static int Sample_Fuc_Cnt = 0 ; 
    static uint32_t Sample_Fuc_Time_ms = 0 ;

    if (Sample_Times <= 0)
    {
        return;
    }

    // 初始时间
    if (Sample_Fuc_Cnt == 0)
    {
        Sample_Fuc_Time_ms = total_ms ;
    }

    Sample_Fuc_Cnt ++ ;

    // 结束时间
    if (Sample_Fuc_Cnt == Sample_Times)
    {
        Sample_Fuc_Cnt = 0 ;
        time_fuc_us = (total_ms -Sample_Fuc_Time_ms) * 1000 / (Sample_Times - 1)  ;
    }
}

// ========================= 系统总时间累加,放在systick的1ms定时器里面 =========================
void Timer_Update_1ms(void)
{
    total_ms ++ ;
}
