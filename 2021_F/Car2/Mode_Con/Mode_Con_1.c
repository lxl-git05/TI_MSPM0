#include "AllHeader.h"
#include "Control.h"

void Mode_Con_1_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====[Car2]Mode_TiGao_2=====") ;
    Serial_printf(&Serial1, "=====[Car2]Mode_TiGao_2=====\n") ;
}

void Mode_Con_1_Loop(void)
{
    
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
