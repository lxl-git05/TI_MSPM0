#ifndef __ORANGE_H
#define __ORANGE_H

#include "Serial.h"
#include "Con_Motor.h"
#include "Timer_Counter.h"
#include "ti_msp_dl_config.h"

extern int Track_GoalSpeed ;
extern Pid_Typedef PID_Track ;
extern int Road_y  ;

// 初始化
void Oran_Init(void) ;

// 数据更新
void Oran_Data_Update(void) ;

// 寻迹PID代码 20ms
void Oran_Track_Tick(void) ;

// 使能寻迹
void Oran_Go(void) ;

// 寻迹停止
void Oran_Stop(void) ;

#endif
