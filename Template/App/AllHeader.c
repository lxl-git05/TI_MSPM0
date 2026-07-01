#include "AllHeader.h"

void Initial_All(void)
{
    // system
    Serial_Init();

    // hardware
    OLED_Init() ;
    MPU6050_Angle_Init() ;

    // tools
    Flash_Mode_Init() ;

    // function
    Motor_Init() ;
    Con_MPU_Init() ;
}

void Timer_Init(void)
{
    // 定时器初始化(放在最后面)
    Timer_Init();
}
