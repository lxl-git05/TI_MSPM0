#include "Timer_Cnt.h"

void LED_ON(void)
{
    DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN) ;
}