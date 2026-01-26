#ifndef __MYDELAY_H
#define __MYDELAY_H

#include "ti_msp_dl_config.h"

/* 务必注意:SYS需要 1us 周期!!! */

// 全局系统时间
extern uint32_t total_ms ;
// 函数运行时间
extern uint32_t time_fuc_us   ;        // 一个函数两次调用间隔的时间
extern uint32_t time_line_us  ;        // 两行之间运行时间

// 初始化:不需要
// void Timer_Cnt_init(void) ;

// 不精确延时
void Delay_ms_cyc(uint32_t ms) ;
void Delay_us_cyc(uint32_t us) ;
// 精确延时
void Delay_ms_tim(uint32_t ms);
// 计时函数
void Timer_Cnt_Begin(int Sample_times); // 计算一个函数两次调用间隔的时间
void Timer_Cnt_End(void);

void Timer_Cnt_Fuc(int Sample_Times);   // 计算一个函数两次调用间隔的时间

// 系统总时间累加
void Timer_Update_1ms(void) ;

#endif
