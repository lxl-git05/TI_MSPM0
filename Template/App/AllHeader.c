#include "AllHeader.h"

void Initial_All(void)
{
    // system
    MyEncoder_Init(&Motor_A_Encoder);
    MyEncoder_Init(&Motor_B_Encoder);

    // Hardware
    OLED_Init() ;
    Con_Motor_Init() ;
    Buzzer_Init() ;

    // tools
    Flash_Mode_Init() ;
    Timer_Counter_Init() ;

    // function
    Serial_Init();
    Encoder_Init();             // EC11 旋转编码器（GPIOA中断初始化）
    Oran_XY_Init() ;            // 香橙派寻迹PID初始化（默认参数→随后Param可覆盖）
    Param_AT24C02_Init() ;      // AT24C02初始化+从EEPROM恢复参数
    Stepper_Init();             // 步进电机初始化（PWM + PID + 限位）
    // 舵机初始化（TIMG8, 50Hz, 1us分辨率, 0~180°/500~2500us）
    Servo_Init(&Servo1, &MyPWM_Servo1, 0.0f, 180.0f, 500, 2500);  // 舵机1: PA29
    Servo_Init(&Servo2, &MyPWM_Servo2, 0.0f, 180.0f, 500, 2500);  // 舵机2: PA2
    IMU_Mahony_Init(0);         // 陀螺仪初始化

}
