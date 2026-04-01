#ifndef __CON_TRACK_H
#define __CON_TRACK_H

#include "MyI2C.h"
#include "MyPID.h"
#include "Y8_Track.h"
#include "math.h"

// 巡线核心控制函数
void Y8_Line_Control(void) ;
// Y8巡线采样,放入中断1ms计次
void Y8_Error_Update_Tick(void) ;

#endif
