#include "AllHeader.h"

// 阈值
#define Track_Inter_Th (230)
#define Track_Over_Th  (180)

// 小车开始运动判断
bool Car_Start = false ;
// 小车回城判断
bool Car_Back_Enable = false ;
// 小车运动状态记录(栈)
StatusStack_Typedef stack_car ;
// 小车装载检测
bool isCarLoad = false ;

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

// 回城状态翻转逻辑
Car_Status_Typedef Car_Status_Fan_1(Car_Status_Typedef Before)
{
    switch (Before) 
    {
        case Car_Stop    : return Car_Turn_H ;
        case Car_Forward : return Car_Forward ;
        case Car_Turn_L  : return Car_Turn_R ;
        case Car_Turn_R  : return Car_Turn_L ;
        case Car_Turn_F  : return Car_Turn_F ;
        default: return 6;
    }
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
    Serial_printf(&Serial1, "stack_cnt = %d , stack_status = %d\n" , StatusStack_Size(&stack_car) - 1, temp) ;
}

void Mode_Con_2_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_2=====") ;
    // 小车状态初始化
    StatusStack_Init(&stack_car) ;
    // 小车初始状态记录
    Car_Status_Change(curr_Status , !Car_Back_Enable) ; // 只有前进的时候才进行记录
}

void Mode_Con_2_Loop(void)
{
    // 模拟目标数字
    // 模拟目标数字
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num += 1 ;
    }
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num = (Target_Num == 0 ? 1 : Target_Num) ;    // 模拟目标数字
        isCarLoad = true ;
    }
    // 起跑判断
    if (Car_Start == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (isCarLoad == true && Target_Num != 0)
        {
            Car_Start = true ;
            Car_Status_Change(Car_Forward , !Car_Back_Enable) ;
        }
    }
    // 回城判断
    if (Key_Check(KEY_1, KEY_LONG))
    {
        Car_Back_Enable = true ;
        
        Car_Status_Typedef temp;
        if (StatusStack_Pop(&stack_car, &temp))
        {
            next_Status = Car_Status_Fan_1(temp);
        }
    }
    // OLED打印栈元素
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_2=====") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "roa=%d,tar=%d", Road_y,Target_Num) ;
}

void Mode_Con_2_Exit(void)
{

}

void Mode_Con_2_Tick(void)
{

}

// 判断路口的类型
Track_Status_Typedef Car_Inter_Check(void)
{
    if (Road_y > Track_Inter_Th)
    {
        return Track_Inter ;
        Serial_printf(&Serial1 ,"Inter\n");     // 检测到路口 
    }
    else if (isRoad_T == true)
    {
        return Track_T_Inter;
        Serial_printf(&Serial1 ,"T_Inter\n");   // 检测到丁字路口
    }
    else if (Over_y > Track_Over_Th)
    {
        return Track_Over ;
        Serial_printf(&Serial1 ,"Over\n");      // 到达终点
    }
    else 
    {
        return Track_Null;
    }
}


// 小车状态转换台
void Car_Control_Change_1(void)
{
    // 当前状态和下次状态相同才能进入切换状态
    if (curr_Status != next_Status) {return;}
    if (Car_Back_Enable == false)
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                // 得到路口状态
                Track_Status_Typedef Track_Status = Car_Inter_Check() ;
                // 下一状态配置
                // 十字路口只有1处含数字
                if (Track_Status == Track_Inter)
                {
                    if      (Target_Num == 1)           { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                    else if (Target_Num == 2)           { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                    else if (Target_Num == Road2[0])    { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                    else if (Target_Num == Road2[1])    { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                    else                                { Car_Status_Change(Car_Turn_F , !Car_Back_Enable);}    // 路口直行
                }
                // T字路口有两类数字列( 4 + 2(L) + 2(R) )
                else if (Track_Status == Track_T_Inter)
                {
                    // 还在直道
                    if (StatusStack_Size(&stack_car) < 8)
                    {
                        if      (Target_Num == Road3[0]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                        else if (Target_Num == Road3[1]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                        else if (Target_Num == Road3[2]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                        else if (Target_Num == Road3[3]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                        else { Serial_printf(&Serial1, "T[4] Catch Failed") ;}    // 报警
                    }
                    // 在最后的T字路口
                    else
                    {
                        // 左转路口
                        if (Target_Num == Road3[0] || Target_Num == Road3[1])
                        {
                            if (Target_Num == Road4_L[0])         { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                            else if (Target_Num == Road4_L[1])    { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                            else { Serial_printf(&Serial1, "T[2] Left Catch Failed") ;}    // 报警
                        }
                        // 右转路口
                        else if (Target_Num == Road3[2] || Target_Num == Road3[3])
                        {
                            if (Target_Num == Road4_R[0])         { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                            else if (Target_Num == Road4_R[1])    { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                            else { Serial_printf(&Serial1, "T[2] Right Catch Failed") ;}    // 报警
                        }
                    }
                }
                else if (Track_Status == Track_Over )
                {
                    Car_Status_Change(Car_Stop , !Car_Back_Enable);
                    // 打印栈元素
                }
                Track_Status = Track_Null ;
                break;
            }
            case Car_Turn_L : 
            {
                if (Con_MPU_Get_Yaw() > 90) {Car_Status_Change(Car_Forward , !Car_Back_Enable);}
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) {Car_Status_Change(Car_Forward , !Car_Back_Enable);}
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {Car_Status_Change(Car_Forward , !Car_Back_Enable);}
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) {Car_Status_Change(Car_Forward , !Car_Back_Enable);}
                break;
            }
            case Car_Stop:
            {
                break;
            }
        }
    }
    else
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                // 得到路口状态
                Track_Status_Typedef Track_Status = Car_Inter_Check() ;
                // 下一状态配置
                if (Track_Status == Track_Inter)
                {
                    Serial_printf(&Serial1 ,"Inter\n"); 
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                // T字路口
                else if (Track_Status == Track_T_Inter)
                {
                    Serial_printf(&Serial1 ,"T_Inter\n"); 
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                else if (Track_Status == Track_Over )
                {
                    next_Status = Car_Stop ;    // 直接停车
                }
                break;
            }
            
            case Car_Turn_L : 
            {
                if (Con_MPU_Get_Yaw() > 90) 
                {
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) 
                {
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) 
                {
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) 
                {
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                break;
            }
            case Car_Stop:
            {
                break;
            }
        }
    }
}
