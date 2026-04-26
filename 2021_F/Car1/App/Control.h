#ifndef __CONTROL_H
#define __CONTROL_H

#include "AllHeader.h"

typedef enum
{
    Car_Stop    ,   // 停车
    Car_Forward ,   // 寻迹环直行
    Car_Turn_L  ,   // 角度环左转
    Car_Turn_R  ,   // 角度环右转
    Car_Turn_F  ,   // 角度环直行
    Car_Turn_H  ,   // 角度环自转180度
}Car_Status_Typedef ;

extern Car_Status_Typedef curr_Status ;
extern Car_Status_Typedef next_Status ;

// 电机外环控制台
void Car_Control(void) ;

// 小车状态转换台
void Car_Control_Change(void) ;

#endif
