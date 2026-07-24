#ifndef __MODE_3_H
#define __MODE_3_H

#include "MySystem.h"

// ==================== Mode_3 功能选择 ====================
// 修改 MODE3_SELECT 值切换当前功能:
//   1 = 电机PID调参    (原Mode_3)
//   2 = 陀螺仪角度环    (原Mode_5)
//   3 = 步进电机驱动    (原Mode_6)
#define MODE3_SELECT 1

void Mode_3_Setup(void);

void Mode_3_Loop(void);

void Mode_3_Exit(void);

void Mode_3_Tick(void);

#endif

