#include "AllHeader.h"
#include "Con_Car.h"

// 提高题部分1-Car2

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool isCarLoad ;
extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

extern bool HandMode; // 手动模拟启动信号: 一旦开启,那么只能手动进行
int Car_1_Target_Num ;  // 小车1的目标数字

void Mode_Con_3_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====[Car2]Mode_TiGao_1=====") ;
    Serial_printf(&Serial1, "=====[Car2]Mode_TiGao_1=====\n") ;
}

void Mode_Con_3_Loop(void)
{
    // 小车装填与否判断
    if (isLoad() == true && HandMode == false)
    {
        isCarLoad = true ;
    }
    else if (isLoad() == false && HandMode == false)
    {
        isCarLoad = false ;
    }

    // 模拟
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num = Target_Num == 4 ? 3 : 4 ;      // 3和4之间选择
    }
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num = (Target_Num == 0 ? 4 : Target_Num) ;    // 模拟目标数字
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
            Serial_printf(&Serial1, "[Car2]Target : %d\n\n",Target_Num) ;
            Car_Status_Change(Car_Forward , 1) ;    // 记录小车运动轨迹
        }
    }
    // 回城判断
    // 1. 模拟回城 2. 正式版:蓝牙控制回城
    if ( (Key_Check(KEY_1, KEY_LONG) || Car2_Enable_Back) && Car_Start)
    {
        // 小车开始回城
        Car_Back_Enable = true ;
        // 小车状态激活        
        Car_Status_Typedef temp;
        if (StatusStack_Pop(&stack_car, &temp))
        {
            next_Status = Car_Status_Fan_1(temp);
        }
    }
    // OLED
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_TiGao_1") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "road=%d,tar=%d,c_1:%d",Road_y,Target_Num,Car_1_Target_Num) ;
    OLED_Printf(0, 40, OLED_6X8, "rd2=%d%d,rd3=%d%d%d%d",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]);
    OLED_Printf(0, 50, OLED_6X8, "rd4L=%d%d,rd4R=%d%d",Road4_L[0],Road4_L[1],Road4_R[0],Road4_R[1]);
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
                break;
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
                    if (Target_Num == Road2[0])
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
                if (MPU6050_Turn_Yaw_Is_Ok(180)) 
                {
                    Car_To_Next_Status_From_Stack() ;
                }
                break;
            }
            case Car_Stop:
            {
                break;
            }
            case Car_Turn_L:
            {
                if (MPU6050_Turn_Yaw_Is_Ok(90)) 
                {
                    Car_Status_Change(Car_Forward , 0);
                }
                break;
            }
            
            case Car_Turn_R:
            {
                if (MPU6050_Turn_Yaw_Is_Ok(-90)) 
                {
                    Car_Status_Change(Car_Forward , 0);
                }
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
