#include "Con_Car.h"

// 阈值
#define Track_Inter_Th (230)
#define Track_Over_Th  (180)

// 小车开始运动判断
bool Car_Start = false ;
// 小车回城判断
bool Car_Back_Enable = false ;
// 小车运动状态记录(栈)
StatusStack_Typedef stack_car ;
const char *car_str[] =  {"Stop" , "Forward" , "Turn_L" , "Turn_R" , "Car_Turn_F" , "Car_Turn_H" } ;
// 小车装载检测
bool isCarLoad = false ;

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

// =================== 初始化
void Car_Init(void)
{
    // 小车状态初始化
    StatusStack_Init(&stack_car) ;
    // 小车初始状态记录
    Car_Status_Change(curr_Status , !Car_Back_Enable) ; // 只有前进的时候才进行记录
}

// =================== 回城
// 回城状态翻转逻辑
Car_Status_Typedef Car_Status_Fan_1(Car_Status_Typedef Before)
{
    Car_Status_Typedef Car_Next_Status ;
    switch (Before) 
    {
        case Car_Stop    : Car_Next_Status = Car_Turn_H ; break; 
        case Car_Forward : Car_Next_Status = Car_Forward ; break; 
        case Car_Turn_L  : Car_Next_Status = Car_Turn_R ; break; 
        case Car_Turn_R  : Car_Next_Status = Car_Turn_L ; break; 
        case Car_Turn_F  : Car_Next_Status = Car_Turn_F ; break; 
        default: Car_Next_Status = Car_Stop ; Serial_printf(&Serial1, "UnKnown Status\n"); break;    // 其他状态就判定为停止
    }
    Serial_printf(&Serial1, "Back: %s\n" , car_str[Car_Next_Status]) ;
    return Car_Next_Status;
}

// 小车状态转换和记录
void Car_Status_Change(Car_Status_Typedef next , bool Store_Enable)
{
    next_Status = next ;
    if (Store_Enable)
    {
        StatusStack_Push(&stack_car, next) ;
    }
    // 打印当前输入的栈
    Car_Status_Typedef temp ;
    StatusStack_Peek(&stack_car , &temp) ;
    Serial_printf(&Serial1, "stack_cnt = %d , stack_status = %s\n" , StatusStack_Size(&stack_car) - 1, car_str[temp]) ;
}

// =================== 路口
// 判断路口的类型
Track_Status_Typedef Car_Inter_Check(void)
{
    if (Road_y > Track_Inter_Th)
    {
        Serial_printf(&Serial1 ,"Inter\n");     // 检测到路口 
        return Track_Inter ;
    }
    else if (isRoad_T == true)
    {
        Serial_printf(&Serial1 ,"T_Inter\n");   // 检测到丁字路口
        return Track_T_Inter;
    }
    else if (Over_y > Track_Over_Th)
    {
        Serial_printf(&Serial1 ,"Over\n");      // 到达终点
        return Track_Over ;
    }
    else 
    {
        return Track_Null;
    }
}

// =================== 