#include "Mode_6.h"
#include "AllHeader.h"

// ======================== 回家对齐驱动 ========================

bool Start = 0 ;

void Mode_6_Setup(void)
{
    OLED_Clear();
    Oran_XY_Init() ;
}

void Mode_6_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "===Mode6===") ;
    OLED_Printf(0, 20, OLED_8X16, "isHomeIn:%d",isHomeIn) ;
    OLED_Printf(0, 40, OLED_8X16, "X:%.1f Y:%.1f",x_real , y_real) ;
    OLED_Printf(0, 55, OLED_8X16, "yaw:%.2f",IMU_Yaw_Abs_Get()) ;
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Start = 1 ;
    }
    if (Start == 0)
    {
        return;
    }
    if (isHomeIn == 0)
    {
        Motor_SetSpeed(&Motor_A, 0) ;
        Motor_SetSpeed(&Motor_B, 0) ;
    }
}

void Mode_6_Tick(void)
{
    if (isHomeIn == 1 && Start == 1)
    {
        Oran_XY_PID_Update() ;
    }
	Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",PID_Oran_X.goalPoint , PID_Oran_X.realPoint_Now , PID_Oran_X.setPoint,PID_Oran_Y.goalPoint , PID_Oran_Y.realPoint_Now , PID_Oran_Y.setPoint) ; 
}

void Mode_6_Exit(void)
{
    OLED_Clear();
}
