#include "AllHeader.h"

void Initial_All(void)
{
    // system
    MyEncoder_Init(&Motor_A_Encoder);
    MyEncoder_Init(&Motor_B_Encoder);

    // hardware
    OLED_Init() ;
    MPU6050_Angle_Init() ;

    // tools
    Flash_Mode_Init() ;
    Timer_Counter_Init() ;

    // function
    // Serial_Init();

}
