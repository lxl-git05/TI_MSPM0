#include "AllHeader.h"
#include "Control.h"

// 提高题部分2-Car2

// 外部参数
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern bool Car_Start ;
extern bool Car_Back_Enable ;
extern StatusStack_Typedef stack_car ;

bool Car2_Back_Enable_Tigao2 = false ;
int Car_1_Target_Num ;  // 小车1的目标数字

void Mode_Con_1_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_TiGao_2") ;
    Serial_printf(&Serial1, "[Car2]Mode_TiGao_2\n") ;
}

// 小车2在病房之后亮红色的灯,小车2没有💊
void Mode_Con_1_Loop(void)
{
    // 模拟数字识别
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num = (Target_Num + 1 == 9 || Target_Num + 1 == 1) ? 5 : Target_Num + 1 ;
    }
    else if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Car_1_Target_Num = (Car_1_Target_Num + 1 == 9 || Car_1_Target_Num + 1 == 1) ? 5 : Car_1_Target_Num + 1 ;
    }
    
    // 起跑判断
    if (Car_Start == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (Target_Num != 0 && Car_1_Target_Num != 0)
        {
            Car_Start = true ;
            Serial_printf(&Serial1, "Target_Car_2 : %d , Target_Car_1 : %d \n\n",Target_Num,Car_1_Target_Num) ;
            Car_Status_Change(Car_Forward , 1) ;    // 记录小车运动轨迹
        }
    }
    // 到病房判断(小车2不需要回程):小车已经运行了为前提
    if (Car_Start == true && Car2_Back_Enable_Tigao2 == false)
    {
        if (Key_Check(KEY_1, KEY_SINGLE))
        {
            Car_Back_Enable = true ;    // 到病房
            Car2_Back_Enable_Tigao2 = true ;    // 只允许执行一次到病房确认

            // 小车状态激活        
            // Car_Status_Typedef temp;
            // if (StatusStack_Pop(&stack_car, &temp))
            // {
            //     next_Status = Car_Status_Fan_1(temp);
            // }
        }
    }
    // OLED
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "[Car2]Mode_JiChu") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "road=%d,tar=%d,c_1:%d",Road_y,Target_Num,Car_1_Target_Num) ;
    OLED_Printf(0, 40, OLED_6X8, "rd2=%d%d,rd3=%d%d%d%d",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]);
    OLED_Printf(0, 50, OLED_6X8, "rd4L=%d%d,rd4R=%d%d",Road4_L[0],Road4_L[1],Road4_R[0],Road4_R[1]);
}

void Mode_Con_1_Exit(void)
{

}

void Mode_Con_1_Tick(void)
{

}

void Car_Control_Change_TiGao_2(void)
{
    
} 
