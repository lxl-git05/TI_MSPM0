#include "AllHeader.h"
#include "Con_Car.h"

// 提高题部分-Car2

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool isCarLoad ;
extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

void Mode_Con_3_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_3=====") ;
}

void Mode_Con_3_Loop(void)
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
        }
    }
    // 回城判断
    // 1. 模拟回城 2. 正式版:蓝牙控制回城
    if ( Key_Check(KEY_1, KEY_SINGLE) || ( Serial_GetNewPackageFlag_ABC(&Serial3) && Serial_CheckCmd(&Serial3,"Car2_Enable_Back")) )
    {
        // 小车状态激活        
        Car_Status_Typedef temp;
        if (StatusStack_Pop(&stack_car, &temp))
        {
            next_Status = Car_Status_Fan_1(temp);
        }
    }
}

// 状态转换台
void Car_Control_Change_TiGao_1(void)
{
    // 当前状态和下次状态相同才能进入切换状态
    if (curr_Status != next_Status) {return;}

    // 开始进行状态转换以及记录
    // 直接就是 stop -> F -> T_F -> F -> T_F -> F -> 识别到T型,stop
    if (Car_Back_Enable == false)
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                Track_Status_Typedef Track_Status = Car_Inter_Check() ; // 得到路口状态
                // 十字路口 直接直行
                if (Track_Status == Track_Inter)
                {
                    // 直接装填 T_F , 不需要拐弯
                    Car_Status_Change(Car_Turn_F , 1);
                }
                // T字路口作为停止判定
                else if (Track_Status == Track_T_Inter)
                {
                    // 停止
                    Car_Status_Change(Car_Stop , 1);
                }
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {Car_Status_Change(Car_Forward , 1);}
                break;
            }
            case Car_Stop:
            {
                break;
            }
            default:
            {
                // 其他状态都默认不理
                Serial_printf(&Serial1, "Car2 Get Error Status\n") ;
                break;
            }
        }
    }
    else    // 开始去病房,是直接去到Car_1的病房,而非回城
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
                    // 识别到十字路口,需要根据目标数字进行左转或者是右转
                    if (Target_Num == 3)
                    {
                        Car_Status_Change(Car_Turn_R , 0);
                    }
                    else 
                    {
                        Car_Status_Change(Car_Turn_L , 0);  // 4号左转
                    }
                }
                // T字路口
                else if (Track_Status == Track_T_Inter)
                {
                    // 不可能识别到T型路口
                    Serial_printf(&Serial1, "Car2 Tigao Back Get Error Data") ;
                }
                else if (Track_Status == Track_Over )
                {
                    next_Status = Car_Stop ;    // 直接停车
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
            default:
            {
                // 其他状态都默认不理
                Serial_printf(&Serial1, "[Back]Car2 Get Error Status\n") ;
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
