#include "Mode_6.h"
#include "AllHeader.h"

// ======================== 步进电机驱动 ========================

float Angle_S ;
bool Ste1_Choice = 1 ;

void Mode_6_Setup(void)
{
    OLED_Clear();
}

void Mode_6_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "===Mode6===") ;
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Serial_PrintDebug(&Serial2) ;
    }
    
}

void Mode_6_Tick(void)
{
    
}

void Mode_6_Exit(void)
{
    OLED_Clear();
}
