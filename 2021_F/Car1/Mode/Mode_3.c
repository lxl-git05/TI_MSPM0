#include "Mode_3.h"
#include "AllHeader.h"

void Mode_3_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_3=====") ;

    Motor_SetSpeed(&Motor_A, 0) ;
    Motor_SetSpeed(&Motor_B, 0) ;
}

void Mode_3_Loop(void)
{
    // 长按开启小车电机
    if (Key_Check(KEY_0, KEY_LONG))
    {
        Motor_SetSpeed(&Motor_A, 80) ;
        Motor_SetSpeed(&Motor_B, 80) ;
    }
}

void Mode_3_Exit(void)
{
    OLED_Clear() ;
}

void Mode_3_Tick(void)
{
    
}
