#include "Timer_Counter.h"

void Timer_Counter_Begin(void)
{
    MyGPIO_WritePin(&MyGPIO_LED_Time, 1) ;
}

void Timer_Counter_End(void)
{
    MyGPIO_WritePin(&MyGPIO_LED_Time, 0) ;
}

// 每次状态变化指示被调用一次
void Timer_Counter_Func(void)
{
    static bool Timer_Counter_1_Mode = true ;

    if (Timer_Counter_1_Mode == true)
    {
        MyGPIO_WritePin(&MyGPIO_LED_Time, 1) ;
    }
    else 
    {
        MyGPIO_WritePin(&MyGPIO_LED_Time, 0) ;
    }
    
    Timer_Counter_1_Mode = !Timer_Counter_1_Mode ;
}
