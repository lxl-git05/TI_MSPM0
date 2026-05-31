#ifndef __ORANGE_H
#define __ORANGE_H

#include "Serial.h"
#include "Con_Motor.h"
#include "Timer_Counter.h"
#include "ti_msp_dl_config.h"

#define Track_Speed 60 // 寻迹的基础速度

extern Pid_Typedef PID_Track ;

extern bool isRoad_T ;  // 判断是否到达T字路口
extern int line_x ;     // 2.
extern int Road_y ;     // 4.
extern int Oran_Num[4]; // 5.6.7.8. 
extern int Target_Num;  // 目标数字
extern int Over_y ;     // 10.

extern bool Angle_Track_Check ;

// 初始化
void Oran_Init(void) ;

// 数据更新
void Oran_Data_Update(void) ;

// 寻迹PID代码 20ms
void Oran_Track_Tick(int BaseSpeed) ;

// 得到目标数字
void Oran_Get_Target(void) ;

// 使能寻迹
void Oran_Go(void) ;

// 寻迹停止
void Oran_Stop(void) ;

#endif
