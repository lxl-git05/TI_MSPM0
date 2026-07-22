#include "AllHeader.h"

void Initial_All(void)
{
    // system
    MyEncoder_Init(&Motor_A_Encoder);
    MyEncoder_Init(&Motor_B_Encoder);

    // Hardware
    OLED_Init() ;
    // MPU6050_Angle_Init() ;
    Con_Motor_Init() ;

    // tools
    Flash_Mode_Init() ;
    Timer_Counter_Init() ;

    // function
    Serial_Init();
    Encoder_Init();             // EC11 旋转编码器（GPIOA中断初始化）
    Param_AT24C02_Init() ;      // AT24C02初始化+从EEPROM恢复参数
    Stepper_Init();             // 步进电机初始化（PWM + PID + 限位）

}
