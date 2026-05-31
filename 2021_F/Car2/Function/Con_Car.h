#ifndef __CON_CAR_H
#define __CON_CAR_H

#include "ti_msp_dl_config.h"
#include "AllHeader.h"

// 阈值
#define Track_Inter_Th (225)
#define Track_Over_Th  (100)

// 小车初始化
void Car_Init(void) ;

// 回城状态翻转逻辑
Car_Status_Typedef Car_Status_Fan_1(Car_Status_Typedef Before) ;

// 小车状态转换和记录
void Car_Status_Change(Car_Status_Typedef next , bool Store_Enable) ;

// 判断路口的类型
Track_Status_Typedef Car_Inter_Check(void) ;

// 自动执行下一状态:状态pop->自动翻转->打印给上位机
void Car_To_Next_Status_From_Stack(void) ;

// 自动执行下一状态
void Car_To_Next_Status_From_Stack(void) ;

#endif
