#ifndef __CON_CAR_H
#define __CON_CAR_H

#include "ti_msp_dl_config.h"
#include "AllHeader.h"

// 小车初始化
void Car_Init(void) ;

// 回城状态翻转逻辑
Car_Status_Typedef Car_Status_Fan_1(Car_Status_Typedef Before) ;

// 小车状态转换和记录
void Car_Status_Change(Car_Status_Typedef next , bool Store_Enable) ;

// 判断路口的类型
Track_Status_Typedef Car_Inter_Check(void) ;

#endif
