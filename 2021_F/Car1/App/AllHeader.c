#include "AllHeader.h"

void Initial_All(void)
{
    // system
    Serial_Init();

    // hardware
    OLED_Init() ;

    // tools
    Flash_Mode_Init() ;
}