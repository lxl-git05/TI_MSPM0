#include "Mode_2.h"
#include "AllHeader.h"

int count ;


void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "Mode2") ;
    count += Encoder_Get() ;
    OLED_Printf(0, 0, OLED_6X8, "count:%d",count) ;
    
    
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1 , "S1=%d S2=%d\r\n", MyGPIO_ReadPin(&MyGPIO_EC11_S1), MyGPIO_ReadPin(&MyGPIO_EC11_S2));
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
