#include "AllHeader.h"

void Initial_All(void)
{
    // system
    Serial_Init();

    // hardware
    OLED_Init() ;

    // tools
    Flash_Mode_Init() ;
    
    // 定时器初始化(放在最后面)
    Timer_0_Init();
}