#include "AllHeader.h"

// 基础部分123-Car2-完成

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
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_JiChu") ;
    Serial_printf(&Serial1, "[Car2]Mode_JiChu\n") ;
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
        Target_Num = (Target_Num == 0 ? 1 : Target_Num) ;    // 模拟目标数字
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
            Car_Status_Change(Car_Forward , 1) ;
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
    RGB_Set(isCarLoad, !isCarLoad, 0) ; // 小车Load后为R,Load前为G
    // OLED打印栈元素
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_JiChu") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "over=%d,tar=%d,size:%d",Over_y,Target_Num,StatusStack_Size(&stack_car)) ;
    OLED_Printf(0, 40, OLED_6X8, "rd2=%d%d,rd3=%d%d%d%d",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]);
    OLED_Printf(0, 50, OLED_6X8, "rd4L=%d%d,rd4R=%d%d",Road4_L[0],Road4_L[1],Road4_R[0],Road4_R[1]);
    // // PID
    // if (Serial_GetNewPackageFlag_ABC(&Serial1))
    // {
    //     // 得到数据
    //     Serial_SetFloatData(&Serial1, "Angle", "Angle=%f", &PID_Track.realPoint_Now) ;
    //     Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Track.Ki) ;
    //     Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Track.Kd) ;
    // }
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
                    if      (Target_Num == 1)           { Car_Status_Change(Car_Turn_L , 1);}
                    else if (Target_Num == 2)           { Car_Status_Change(Car_Turn_R , 1);}
                    else if (Target_Num == Road2[0])    { Car_Status_Change(Car_Turn_L , 1);}
                    else if (Target_Num == Road2[1])    { Car_Status_Change(Car_Turn_R , 1);}
                    else                                { Car_Status_Change(Car_Turn_F , 1);}    // 路口直行
                }
                // T字路口有两类数字列( 4 + 2(L) + 2(R) )
                else if (Track_Status == Track_T_Inter)
                {
                    // 还在直道
                    if (StatusStack_Size(&stack_car) < 8)
                    {
                        if      (Target_Num == Road3[0]) { Car_Status_Change(Car_Turn_L , 1);}
                        else if (Target_Num == Road3[1]) { Car_Status_Change(Car_Turn_L , 1);}
                        else if (Target_Num == Road3[2]) { Car_Status_Change(Car_Turn_R , 1);}
                        else if (Target_Num == Road3[3]) { Car_Status_Change(Car_Turn_R , 1);}
                        else { Serial_printf(&Serial1, "T[4] Catch Failed") ;}    // 报警
                    }
                    // 在最后的T字路口
                    else
                    {
                        // 左转路口
                        if (Target_Num == Road3[0] || Target_Num == Road3[1])
                        {
                            if (Target_Num == Road4_L[0])         { Car_Status_Change(Car_Turn_L , 1);}
                            else if (Target_Num == Road4_L[1])    { Car_Status_Change(Car_Turn_R , 1);}
                            else { Serial_printf(&Serial1, "T[2] Left Catch Failed") ;}    // 报警
                        }
                        // 右转路口
                        else if (Target_Num == Road3[2] || Target_Num == Road3[3])
                        {
                            if (Target_Num == Road4_R[0])         { Car_Status_Change(Car_Turn_L , 1);}
                            else if (Target_Num == Road4_R[1])    { Car_Status_Change(Car_Turn_R , 1);}
                            else { Serial_printf(&Serial1, "T[2] Right Catch Failed") ;}    // 报警
                        }
                    }
                }
                else if (Track_Status == Track_Over )
                {
                    Car_Status_Change(Car_Stop , 1);
                    // 打印栈元素
                }
                Track_Status = Track_Null ;
                break;
            }
            case Car_Turn_L : 
            {
                if (Con_MPU_Get_Yaw() > 90) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) {Car_Status_Change(Car_Forward , 1);}
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
                    Car_To_Next_Status_From_Stack();
                }
                // T字路口
                else if (Track_Status == Track_T_Inter)
                {
                    Car_To_Next_Status_From_Stack();
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
                    Car_To_Next_Status_From_Stack();
                }
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) 
                {
                    Car_To_Next_Status_From_Stack();
                }
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) 
                {
                    Car_To_Next_Status_From_Stack();
                }
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) 
                {
                    Car_To_Next_Status_From_Stack();
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
