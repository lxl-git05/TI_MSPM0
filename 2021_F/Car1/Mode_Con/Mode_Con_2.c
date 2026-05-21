#include "AllHeader.h"

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool isCarLoad ;
extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

void Mode_Con_2_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_2=====") ;
}

void Mode_Con_2_Loop(void)
{
    // 模拟目标数字
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num += 1 ;
    }
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num = (Target_Num == 0 ? 5 : Target_Num) ;    // 模拟目标数字
        isCarLoad = true ;
    }
    // 起跑判断
    if (Car_Start == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (isCarLoad == true && Target_Num != 0)
        {
            Serial_printf(&Serial1, "Target : %d\n\n",Target_Num) ;
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
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_2=====") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "tar=%d,size:%d",Target_Num,StatusStack_Size(&stack_car)) ;
    // PID
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 得到数据
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Track.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Track.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Track.Kd) ;
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
                    // Serial_printf(&Serial1 ,"Back: Inter\n"); 
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                // T字路口
                else if (Track_Status == Track_T_Inter)
                {
                    // Serial_printf(&Serial1 ,"Back: T_Inter\n"); 
                    Car_Status_Typedef  Track_Pop ;
                    StatusStack_Pop(&stack_car, &Track_Pop) ; 
                    next_Status = Car_Status_Fan_1(Track_Pop) ;
                }
                else if (Track_Status == Track_Over )
                {
                    next_Status = Car_Stop ;    // 直接停车
                    // Serial_printf(&Serial1 ,"Back: Over\n"); 
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

void Mode_Con_2_Exit(void)
{

}

void Mode_Con_2_Tick(void)
{

}
