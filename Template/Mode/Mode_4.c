#include "Mode_4.h"
#include "AllHeader.h"

float p[4] = {0,0,10,0} ;
bool Start = 0 ;

// 寻迹测试

void Mode_4_Setup(void)
{
    Task_Oran_Track_Setup(p) ;
}

void Mode_4_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "Mode_4") ;
    // ===== OLED 显示 =====
    OLED_Printf(0, 20, OLED_6X8, "X:%.1f Y:%.1f", x_real, y_real);
    OLED_Printf(0, 40, OLED_6X8, "Yaw:%.1f", IMU_Yaw_Abs_Get());
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Start = 1 ;
    }
    if (Start == 0)
    {
        return; 
    }
    if (Task_Oran_Track_IsExit(p))
    {
        OLED_Printf(0, 30, OLED_8X16, "Oran_Track_OK!") ;
        Motor_SetSpeed(&Motor_A, 0) ;
        Motor_SetSpeed(&Motor_B, 0) ;
        Start = 0 ;
    }
}

void Mode_4_Tick(void)
{
    if (Start == 0)
    {
        return; 
    }
    Task_Oran_Track_Tick(p) ;
}

void Mode_4_Exit(void)
{
    
}
