#include "Mode_4.h"
#include "AllHeader.h"


void Mode_4_Setup(void)
{
    OLED_Clear();
}

void Mode_4_Loop(void)
{
    OLED_Printf(0, 0, OLED_8X16, "===Mode_4===") ;
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Motor_SetSpeed(&Motor_A, 100);
        Motor_SetSpeed(&Motor_B, 100);
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Motor_Stop(&Motor_A) ;
        Motor_Stop(&Motor_B) ;
    }
}

void Mode_4_Tick(void)
{
    
}

void Mode_4_Exit(void)
{
    OLED_Clear();
}
