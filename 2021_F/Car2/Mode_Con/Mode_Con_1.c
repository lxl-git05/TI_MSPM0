#include "AllHeader.h"
#include "Control.h"
#include "BLE.h"

// 提高题2-Car2：等 Car2_Enable_Go 出发→到本车终点→BLE 发 Car1_Enable_Back

extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

bool Car2_Tigao2_Done = false ;
extern int Car_1_Target_Num ;

void Mode_Con_1_Setup(void)
{
    Car1_Enable_Back = false ;
    Car2_Tigao2_Done = false ;
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_TiGao_2") ;
    Serial_printf(&Serial1, "[Car2]Mode_TiGao_2\n") ;
}

void Mode_Con_1_Loop(void)
{
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num = (Target_Num + 1 == 9 || Target_Num + 1 == 1) ? 5 : Target_Num + 1 ;
    }

    if (Car_Start == false)
    {
        Oran_Get_Target() ;
        if (Target_Num != 0 && Car2_Enable_Go)
        {
            Car_Start = true ;
            Serial_printf(&Serial1, "Target_Car_2 : %d\r\n", Target_Num) ;
            Car_Status_Change(Car_Forward , 1) ;
        }
    }

    if (Car2_Tigao2_Done)
    {
        RGB_Set(1, 0, 0) ;
    }

    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_TiGao_2") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw, curr_Status, next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "road=%d,tar=%d,c_1:%d", Road_y, Target_Num, Car_1_Target_Num) ;
    OLED_Printf(0, 40, OLED_6X8, "rd2=%d%d,rd3=%d%d%d%d", Road2[0], Road2[1], Road3[0], Road3[1], Road3[2], Road3[3]);
    OLED_Printf(0, 50, OLED_6X8, "rd4L=%d%d,rd4R=%d%d", Road4_L[0], Road4_L[1], Road4_R[0], Road4_R[1]);
    OLED_Printf(0, 60, OLED_6X8, "Go:%d,C1Bk:%d", Car2_Enable_Go, Car1_Enable_Back);
}

void Mode_Con_1_Exit(void)
{

}

void Mode_Con_1_Tick(void)
{

}

void Car_Control_Change_TiGao_2(void)
{
    if (curr_Status != next_Status) { return ; }

    if (Car_Back_Enable == false)
    {
        switch (curr_Status)
        {
            case Car_Forward:
            {
                Track_Status_Typedef Track_Status = Car_Inter_Check() ;
                if (Track_Status == Track_Inter)
                {
                    Car_Status_Change(Car_Turn_F , 1);
                }
                else if (Track_Status == Track_T_Inter)
                {
                    if (StatusStack_Size(&stack_car) < 8)
                    {
                        if      (Target_Num == Road3[0]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                        else if (Target_Num == Road3[1]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                        else if (Target_Num == Road3[2]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                        else if (Target_Num == Road3[3]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                        else { Serial_printf(&Serial1, "T[4] Catch Failed") ;}
                    }
                    else
                    {
                        if (Target_Num == Road3[0] || Target_Num == Road3[1])
                        {
                            if      (Target_Num == Road4_L[0]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                            else if (Target_Num == Road4_L[1]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                            else { Serial_printf(&Serial1, "T[2] Left Catch Failed") ;}
                        }
                        else if (Target_Num == Road3[2] || Target_Num == Road3[3])
                        {
                            if      (Target_Num == Road4_R[0]) { Car_Status_Change(Car_Turn_L , !Car_Back_Enable);}
                            else if (Target_Num == Road4_R[1]) { Car_Status_Change(Car_Turn_R , !Car_Back_Enable);}
                            else { Serial_printf(&Serial1, "T[2] Right Catch Failed") ;}
                        }
                    }
                }
                else if (Track_Status == Track_Over)
                {
                    Car_Status_Change(Car_Stop , !Car_Back_Enable);
                    static bool car2_back_sent = false ;
                    if (!car2_back_sent)
                    {
                        car2_back_sent = true ;
                        Car1_Enable_Back = true ;
                        Car2_Tigao2_Done = true ;
                        Serial_printf(&Serial1, "[Tigao2] Car1_Enable_Back sent\n") ;
                    }
                }
                Track_Status = Track_Null ;
                break;
            }
            case Car_Turn_L:
            {
                if (MPU6050_Turn_Yaw_Is_Ok(90)) { Car_Status_Change(Car_Forward , !Car_Back_Enable); }
                break;
            }
            case Car_Turn_R:
            {
                if (MPU6050_Turn_Yaw_Is_Ok(-90)) { Car_Status_Change(Car_Forward , !Car_Back_Enable); }
                break;
            }
            case Car_Turn_F:
            {
                if (Road_y < 20) { Car_Status_Change(Car_Forward , !Car_Back_Enable); }
                break;
            }
            case Car_Turn_H:
            {
                if (MPU6050_Turn_Yaw_Is_Ok(180)) { Car_Status_Change(Car_Forward , !Car_Back_Enable); }
                break;
            }
            case Car_Stop:
            {
                break;
            }
            default:
                break;
        }
    }
}
