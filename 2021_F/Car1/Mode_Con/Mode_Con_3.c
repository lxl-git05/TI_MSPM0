#include "AllHeader.h"
#include "Con_Car.h"

// 提高题1-Car1：识别3/4上药→去病房→卸药回程→首个十字转向完成后 BLE 发 Enable→Car2 可去同病房

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool isCarLoad ;
extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

bool Car1_Back_Enable_Tigao1 = false ; // 卸药回程只触发一次
extern bool HandMode ;

static bool car1_cross_enable_pending = false ; // 回程已遇十字、等本次 maneuver 完成
static bool car1_cross_enable_sent = false ;   // Enable 整局只发一次

static void Car1_Try_Send_Car2_Enable(void)
{
    if (car1_cross_enable_pending && !car1_cross_enable_sent)
    {
        Car2_Enable_Back = true ; // BLE Data[1]=100
        car1_cross_enable_sent = true ;
        car1_cross_enable_pending = false ;
    }
}

void Mode_Con_3_Setup(void)
{
    car1_cross_enable_pending = false ;
    car1_cross_enable_sent = false ;
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====[Car1]Mode_TiGao_1=====") ;
    Serial_printf(&Serial1, "=====[Car1]Mode_TiGao_1=====\n") ;
}

void Mode_Con_3_Loop(void)
{
    Manual_Serial1_Parse() ; // 仅 @EnBack 测广播；Target/Road 仍来自香橙派

    if (isLoad() == true && HandMode == false) 
    { 
        // 延时1s
       Delay_ms(1000) ;
        isCarLoad = true ; RGB_Set(0,1,0) ;
    }
    else if (isLoad() == false && HandMode == false) 
    {
        isCarLoad = false ;  
    }

    RGB_Set(isCarLoad,!isCarLoad,0); // 小车Load后为R, Load前为G

    // 模拟数字识别
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num = Target_Num == 4 ? 3 : 4 ;      // 3和4之间选择
    }
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num = (Target_Num == 4 ? 4 : 3) ;    // 模拟目标数字
        isCarLoad = true ;
        HandMode = true ;
    }
    
    // 起跑判断
    if (Car_Start == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (isCarLoad == true && Target_Num != 0)
        {
            Car_Start = true ;
            Serial_printf(&Serial1, "Target : %d\n\n",Target_Num) ;
            Car_Status_Change(Car_Forward , 1) ;    // 记录小车运动轨迹
        }
    }
    // 回城判断:小车已经运行了为前提
    if (Car_Start == true && Car1_Back_Enable_Tigao1 == false)
    {
        if (isCarLoad == false || Key_Check(KEY_1, KEY_LONG))
        {
            Car_Back_Enable = true ;    // 回城
            Car1_Back_Enable_Tigao1 = true ;    // 只允许执行一次回城确认

            // 小车状态激活        
            Car_Status_Typedef temp;
            if (StatusStack_Pop(&stack_car, &temp))
            {
                next_Status = Car_Status_Fan_1(temp);
            }
        }
    }
    // OLED
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car1]Mode_TiGao_1") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "road=%d,tar=%d,size:%d",Road_y,Target_Num,StatusStack_Size(&stack_car)) ;
    OLED_Printf(0, 40, OLED_6X8, "rd2=%d%d,rd3=%d%d%d%d",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]);
    OLED_Printf(0, 50, OLED_6X8, "rd4L=%d%d,rd4R=%d%d",Road4_L[0],Road4_L[1],Road4_R[0],Road4_R[1]);
    OLED_Printf(0, 60, OLED_6X8, "C2En:%d,H:%d", Car2_Enable_Back, HandMode) ;
}

void Car_Control_Change_TiGao_1(void)
{
    if (curr_Status != next_Status) {return;}

    // 开始进行状态转换以及记录
    if (Car_Back_Enable == false)
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                Track_Status_Typedef Track_Status = Car_Inter_Check() ;
                if (Track_Status == Track_Inter)
                {
                    if      (Target_Num == 1)           { Car_Status_Change(Car_Turn_L , 1);}
                    else if (Target_Num == 2)           { Car_Status_Change(Car_Turn_R , 1);}
                    else if (Target_Num == Road2[0])    { Car_Status_Change(Car_Turn_L , 1);}
                    else if (Target_Num == Road2[1])    { Car_Status_Change(Car_Turn_R , 1);}
                    else                                { Car_Status_Change(Car_Turn_F , 1);}
                }
                else if (Track_Status == Track_T_Inter)
                {
                    if (StatusStack_Size(&stack_car) < 8)
                    {
                        if      (Target_Num == Road3[0]) { Car_Status_Change(Car_Turn_L , 1);}
                        else if (Target_Num == Road3[1]) { Car_Status_Change(Car_Turn_L , 1);}
                        else if (Target_Num == Road3[2]) { Car_Status_Change(Car_Turn_R , 1);}
                        else if (Target_Num == Road3[3]) { Car_Status_Change(Car_Turn_R , 1);}
                        else { Serial_printf(&Serial1, "T[4] Catch Failed") ;}
                    }
                    else
                    {
                        if (Target_Num == Road3[0] || Target_Num == Road3[1])
                        {
                            if      (Target_Num == Road4_L[0]) { Car_Status_Change(Car_Turn_L , 1);}
                            else if (Target_Num == Road4_L[1]) { Car_Status_Change(Car_Turn_R , 1);}
                            else { Serial_printf(&Serial1, "T[2] Left Catch Failed") ;}
                        }
                        else if (Target_Num == Road3[2] || Target_Num == Road3[3])
                        {
                            if      (Target_Num == Road4_R[0]) { Car_Status_Change(Car_Turn_L , 1);}
                            else if (Target_Num == Road4_R[1]) { Car_Status_Change(Car_Turn_R , 1);}
                            else { Serial_printf(&Serial1, "T[2] Right Catch Failed") ;}
                        }
                    }
                }
                // 识别到终点,停止运动,等待后续响应
                else if (Track_Status == Track_Over )
                {
                    Car_Status_Change(Car_Stop , 1);
                }
                Track_Status = Track_Null ;
                break;
            }
            case Car_Turn_L : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(90)) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_R : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(-90)) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Turn_H : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(180)) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Stop:
            {
                break;
            }
        }
    }
    else // 回程：栈 pop；首个十字 maneuver 完成后发 Enable（丁字不 pending）
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                Track_Status_Typedef Track_Status = Car_Inter_Check() ;
                if (Track_Status == Track_Inter)
                {
                    Car_To_Next_Status_From_Stack() ;
                    if (!car1_cross_enable_sent) { car1_cross_enable_pending = true ; }
                }
                else if (Track_Status == Track_T_Inter)
                {
                    Car_To_Next_Status_From_Stack() ;
                }
                else if (Track_Status == Track_Over )
                {
                    next_Status = Car_Stop ;
                }
                break;
            }
            case Car_Turn_L : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(90)) 
                {
                    Car_To_Next_Status_From_Stack() ;
                    Car1_Try_Send_Car2_Enable() ;
                }
                break;
            }
            case Car_Turn_R : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(-90)) 
                {
                    Car_To_Next_Status_From_Stack() ;
                    Car1_Try_Send_Car2_Enable() ;
                }
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) // 回程十字也可能是直行
                {
                    Car_To_Next_Status_From_Stack() ;
                    Car1_Try_Send_Car2_Enable() ;
                }
                break;
            }
            case Car_Turn_H : 
            {
                if (MPU6050_Turn_Yaw_Is_Ok(180)) 
                {
                    Car_To_Next_Status_From_Stack() ;
                    Car1_Try_Send_Car2_Enable() ;
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

void Mode_Con_3_Exit(void)
{

}

void Mode_Con_3_Tick(void)
{

}
