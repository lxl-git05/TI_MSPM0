#include "AllHeader.h"
#include "Control.h"

// 提高题部分2-Car1-测试

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool isCarLoad ;
extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

bool Car1_Back_Enable_Tigao2 = false ;

void Mode_Con_1_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====[Car1]Mode_TiGao_2=====") ;
    Serial_printf(&Serial1, "=====[Car1]Mode_TiGao_2=====\n") ;
}

void Mode_Con_1_Loop(void)
{
    // 小车装填与否判断
    if (isLoad() == true) { isCarLoad = true ; }
    else {isCarLoad = false ;}

    // 起跑判断
    if (Car_Start == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (isCarLoad == true && Target_Num != 0)
        {
            Car_Start = true ;
            Serial_printf(&Serial1, "Target : %d\n\n",Target_Num) ;
            Car_Status_Change(Car_Forward , 1) ;    // 记录小车运动轨迹
            // 新增:蓝牙: 发送小车1的目标数字
            Serial_Printf_Normal(&Serial3, "@Car1_Target=%d$#" , Target_Num) ;
        }
    }
    // 回城判断:小车已经运行了为前提
    if (Car_Start == true && Car1_Back_Enable_Tigao2 == false)
    {
        if (isCarLoad == false)
        {
            Car_Back_Enable = true ;    // 回城
            Car1_Back_Enable_Tigao2 = true ;    // 只允许执行一次回城确认

            // 小车状态激活        
            Car_Status_Typedef temp;
            if (StatusStack_Pop(&stack_car, &temp))
            {
                next_Status = Car_Status_Fan_1(temp);
            }
        }
    }
}

// 小车状态转换台-提高2
void Car_Control_Change_TiGao_2(void)
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
                // 十字路口 
                if (Track_Status == Track_Inter)
                {
                    // 十字路口都是直行
                    Car_Status_Change(Car_Turn_F , 1);
                }
                // T字路口 
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
                    // 完成出发路径                    
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
                   Car_To_Next_Status_From_Stack() ;
                }
                // T字路口
                else if (Track_Status == Track_T_Inter)
                {
                   Car_To_Next_Status_From_Stack() ;
                   // 第2个T字路口通知小车2回程
                   static int T_cnt = 0 ;
                   T_cnt ++ ;
                   if (T_cnt >= 2)
                   {
                        Serial_Printf_Normal(&Serial3, "@Car2_Enable_Back$#") ; // 蓝牙: 允许小车2启动
                   }
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
                    Car_To_Next_Status_From_Stack() ;
                }
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) 
                {
                    Car_To_Next_Status_From_Stack() ;
                }
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) 
                {
                   Car_To_Next_Status_From_Stack() ;
                }
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) 
                {
                    Car_To_Next_Status_From_Stack() ;
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

void Mode_Con_1_Exit(void)
{

}

void Mode_Con_1_Tick(void)
{

}
