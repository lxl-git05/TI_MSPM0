#ifndef __ORANGE_H
#define __ORANGE_H

#include "MySystem.h"

// 香橙派/OpenMV 视觉数据（外部定义，通信模块写入）
extern float x_real;    // 目标 X 坐标实际值（视觉测量）
extern float y_real;    // 目标 Y 坐标实际值（视觉测量）
extern float x_tar;     // 目标 X 坐标期望值
extern float y_tar;     // 目标 Y 坐标期望值

#endif
