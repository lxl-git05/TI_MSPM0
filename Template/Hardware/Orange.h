#ifndef __ORANGE_H
#define __ORANGE_H

#include "MySystem.h"
#include "MyPID.h"

// 香橙派/OpenMV 视觉数据（外部定义，通信模块写入）
extern float x_real;    // 目标 X 坐标实际值(偏差)
extern float y_real;    // 目标 Y 坐标实际值(偏差)
extern int Oran_Item[4];// 目标位置
extern bool isHomeIn   ;// 家是否入画
extern bool isOver     ;// 本题是否结束

// 香橙派数据更新,在Mode_G实现20ms固定更新
void Oran_Update(void) ;

// 香橙派寻迹PID
extern Pid_Typedef PID_Oran_X ;
extern Pid_Typedef PID_Oran_Y ;
void Oran_XY_Init(void) ;
void Oran_XY_PID_Update(void) ;

#endif
