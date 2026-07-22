#include "Mode_2.h"
#include "AllHeader.h"

int count ;

void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    count += Encoder_Get() ;
    OLED_Printf(0, 0, OLED_6X8, "count:%d",count) ;
}

void Mode_2_Tick(void)
{
    
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
