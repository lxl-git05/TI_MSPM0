#ifndef __CONTROL_H
#define __CONTROL_H

#include "AllHeader.h"

// 5种状态: 停车 寻迹直行 路口直行 左转 右转 掉头
typedef enum
{
    Car_Stop    ,   // 停车         0
    Car_Forward ,   // 寻迹环直行   1
    Car_Turn_L  ,   // 角度环左转   2
    Car_Turn_R  ,   // 角度环右转   3
    Car_Turn_F  ,   // 角度环直行   4
    Car_Turn_H  ,   // 角度环自转180度 5
}Car_Status_Typedef ;

extern Car_Status_Typedef curr_Status ;
extern Car_Status_Typedef next_Status ;

typedef enum 
{
    Track_Null ,        // 没检测到
    Track_Inter ,       // 交叉路口
    Track_T_Inter ,     // T字路口
    Track_Over ,        // 终点
}Track_Status_Typedef;
extern Track_Status_Typedef Track_Status     ;
extern Track_Status_Typedef Track_Status_Back;

// 电机外环控制台
void Car_Control(void) ;

#endif
