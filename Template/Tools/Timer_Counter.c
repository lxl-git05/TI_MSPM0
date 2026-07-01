#include "Timer_Counter.h"

void Timer_Counter_Begin(void)
{
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
}

void Timer_Counter_End(void)
{
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
}

// 每次状态变化指示被调用一次
void Timer_Counter_Func(void)
{
    static bool Timer_Counter_1_Mode = true ;

    if (Timer_Counter_1_Mode == true)
    {
        DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
    }
    else 
    {
        DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
    }
    
    Timer_Counter_1_Mode = !Timer_Counter_1_Mode ;
}
