#include "Mode_2.h"
#include "AllHeader.h"

int test = 1;

void Mode_2_Setup(void)
{
    
}

void Mode_2_Loop(void)
{
    if (Serial_SetIntData(&Serial2 , "Turn" , "Turn=%d" , &test))
    {
        Serial_Clear_ABC(&Serial2) ;
        Serial_printf(&Serial2, "@OK:%d$#",test) ;
    }
    OLED_Printf(0, 20, OLED_8X16, "test:%d",test) ;
}

void Mode_2_Tick(void)
{
    
}

void Mode_2_Exit(void)
{
    
}
