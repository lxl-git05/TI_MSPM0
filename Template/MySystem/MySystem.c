#include "MySystem.h"

// ====================================================================
// GPIO 实例定义 — 换芯片时只需修改这里的 port/pin
// ====================================================================

// 基础 I/O
MyGPIO_Typedef MyGPIO_LED0          = {GPIO_LED_PORT , GPIO_LED_LED_PIN_0_PIN};
MyGPIO_Typedef MyGPIO_LED_Time      = {GPIO_LED_PORT , GPIO_LED_LED_Time_PIN};

MyGPIO_Typedef MyGPIO_Key0          = {GPIO_KEY_KEY_0_PORT , GPIO_KEY_KEY_0_PIN};
MyGPIO_Typedef MyGPIO_Key1          = {GPIO_KEY_KEY_1_PORT , GPIO_KEY_KEY_1_PIN};
MyGPIO_Typedef MyGPIO_Key2          = {GPIO_KEY_KEY_2_PORT , GPIO_KEY_KEY_2_PIN};

// OLED 软件 I2C
MyGPIO_Typedef MyGPIO_OLED_SCL      = {GPIO_OLED_PORT , GPIO_OLED_OLED_SCL_PIN};
MyGPIO_Typedef MyGPIO_OLED_SDA      = {GPIO_OLED_PORT , GPIO_OLED_OLED_SDA_PIN};

// 电机驱动
MyGPIO_Typedef MyGPIO_Motor_A_IN1   = {GPIO_PWM_MOTOR_PWM_C0_PORT , GPIO_PWM_MOTOR_PWM_C0_PIN};
MyGPIO_Typedef MyGPIO_Motor_A_IN2   = {GPIO_MOTOR_IN2_PORT , GPIO_MOTOR_IN2_AIN2_PIN};
MyGPIO_Typedef MyGPIO_Motor_B_IN1   = {GPIO_PWM_MOTOR_PWM_C1_PORT , GPIO_PWM_MOTOR_PWM_C1_PIN};
MyGPIO_Typedef MyGPIO_Motor_B_IN2   = {GPIO_MOTOR_IN2_PORT , GPIO_MOTOR_IN2_BIN2_PIN};

// RGB LED
MyGPIO_Typedef MyGPIO_RGB_R         = {GPIO_RGB_PORT , GPIO_RGB_LED_R_PIN};
MyGPIO_Typedef MyGPIO_RGB_G         = {GPIO_RGB_PORT , GPIO_RGB_LED_G_PIN};
MyGPIO_Typedef MyGPIO_RGB_B         = {GPIO_RGB_PORT , GPIO_RGB_LED_B_PIN};

// 其他外设（完整引脚分配 — 对照 empty.syscfg + README.md）
MyGPIO_Typedef MyGPIO_Buzzer        = {GPIO_Buzzer_PORT, GPIO_Buzzer_PIN_Buzzer_PIN};                         // PB23, +5V有源蜂鸣器
MyGPIO_Typedef MyGPIO_Elec          = {GPIO_Elec_PORT, GPIO_Elec_PIN_Elec_PIN};                               // PB24, 电磁铁
MyGPIO_Typedef MyGPIO_TCRT          = {GPIO_TCRT_PORT, GPIO_TCRT_TCRT_0_PIN};                                 // PA13, INPUT+PULL_UP

// EC11 旋转编码器（均在 PORTA → 共用端口宏）
MyGPIO_Typedef MyGPIO_EC11_Key      = {GPIO_EC11_PORT, GPIO_EC11_GPIO_EC11_KEY_PIN};                          // PA16, INPUT+PULL_UP
MyGPIO_Typedef MyGPIO_EC11_S1       = {GPIO_EC11_PORT, GPIO_EC11_GPIO_EC11_S1_PIN};                           // PA12, INPUT+PULL_UP, FALL_INT
MyGPIO_Typedef MyGPIO_EC11_S2       = {GPIO_EC11_PORT, GPIO_EC11_GPIO_EC11_S2_PIN};                           // PA14, INPUT+PULL_UP, FALL_INT

// 步进电机 — 云台1（En1=PA18 PORTA, Dir1=PB17 PORTB → 不同端口, per-pin port宏）
MyGPIO_Typedef MyGPIO_Stepper_En    = {GPIO_Stepper1_PIN_En1_PORT,  GPIO_Stepper1_PIN_En1_PIN};               // PA18
MyGPIO_Typedef MyGPIO_Stepper_Dir   = {GPIO_Stepper1_PIN_Dir1_PORT, GPIO_Stepper1_PIN_Dir1_PIN};              // PB17

// 步进电机 — 云台2（均在 PORTB → 共用端口宏）
MyGPIO_Typedef MyGPIO_Stepper2_En   = {GPIO_Stepper2_PORT, GPIO_Stepper2_PIN_En2_PIN};                         // PB18
MyGPIO_Typedef MyGPIO_Stepper2_Dir  = {GPIO_Stepper2_PORT, GPIO_Stepper2_PIN_Dir2_PIN};                        // PB19

// Y8 巡线模块（CLK=PA22 PORTA, DAT=PB20 PORTB → 不同端口, per-pin port宏）
MyGPIO_Typedef MyGPIO_Y8_CLK        = {GPIO_Y8_PIN_Y8_CLK_PORT, GPIO_Y8_PIN_Y8_CLK_PIN};                       // PA22, OUTPUT
MyGPIO_Typedef MyGPIO_Y8_DAT        = {GPIO_Y8_PIN_Y8_DAT_PORT, GPIO_Y8_PIN_Y8_DAT_PIN};                       // PB20, INPUT+PULL_UP

// ====================================================================
// PWM 实例定义
// ====================================================================
MyPWM_Typedef MyPWM_Motor_A_IN1 = {PWM_MOTOR_PWM_INST, GPIO_PWM_MOTOR_PWM_C0_IDX, 1000.0f, 0.0f, 4000000, 0};
MyPWM_Typedef MyPWM_Motor_B_IN1 = {PWM_MOTOR_PWM_INST, GPIO_PWM_MOTOR_PWM_C1_IDX, 1000.0f, 0.0f, 4000000, 0};
MyPWM_Typedef MyPWM_Stepper1    = {PWM_Stepper1_INST, GPIO_PWM_Stepper1_C0_IDX, 65535.0f, 0.0f, PWM_Stepper1_INST_CLK_FREQ, PWM_Stepper1_INST_INT_IRQN};
MyPWM_Typedef MyPWM_Stepper2    = {PWM_Stepper2_INST, GPIO_PWM_Stepper2_C0_IDX, 65535.0f, 0.0f, PWM_Stepper2_INST_CLK_FREQ, PWM_Stepper2_INST_INT_IRQN};

// ====================================================================
// Encoder 实例定义
// ====================================================================
MyEncoder_Typedef Motor_A_Encoder = {
    {GPIO_ENCODER_A_PORT, GPIO_ENCODER_A_A_Encoder_1_PIN, GPIO_ENCODER_A_A_Encoder_2_PIN},
    GPIO_MULTIPLE_GPIOB_INT_IRQN, 2U, 0, 0
};
MyEncoder_Typedef Motor_B_Encoder = {
    {GPIO_ENCODER_B_PORT, GPIO_ENCODER_B_B_Encoder_1_PIN, GPIO_ENCODER_B_B_Encoder_2_PIN},
    GPIO_MULTIPLE_GPIOB_INT_IRQN, 2U, 0, 0
};
