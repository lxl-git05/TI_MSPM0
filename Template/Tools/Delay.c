#include "Delay.h"

volatile uint32_t global_time_ms = 0 ;

void Delay_Global_Tick(void)
{
    global_time_ms ++ ;
}

void Delay_ms(uint32_t delay_ms)
{
    uint32_t cur = global_time_ms ;
    while ( global_time_ms - cur < delay_ms) 
    {
        ;
    }
}

uint32_t Get_Time_ms(void)
{
    return global_time_ms;
}
