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

// 未分配引脚的外设 — 空占位（port=0, pin=0），MyGPIO_WritePin/ReadPin 会自动跳过
MyGPIO_Typedef MyGPIO_Key3          = {0, 0};
MyGPIO_Typedef MyGPIO_Buzzer        = {0, 0};
MyGPIO_Typedef MyGPIO_EC11_Key      = {0, 0};
MyGPIO_Typedef MyGPIO_Elec          = {0, 0};
MyGPIO_Typedef MyGPIO_TCRT          = {0, 0};
MyGPIO_Typedef MyGPIO_Stepper_En    = {0, 0};
MyGPIO_Typedef MyGPIO_Stepper2_En   = {0, 0};
MyGPIO_Typedef MyGPIO_Stepper_Dir   = {0, 0};
MyGPIO_Typedef MyGPIO_Stepper2_Dir  = {0, 0};
MyGPIO_Typedef MyGPIO_Y8_Addr0      = {0, 0};
MyGPIO_Typedef MyGPIO_Y8_Addr1      = {0, 0};
MyGPIO_Typedef MyGPIO_Y8_Addr2      = {0, 0};

// ====================================================================
// PWM 实例定义
// ====================================================================
MyPWM_Typedef MyPWM_Motor_A_IN1 = {PWM_MOTOR_PWM_INST, GPIO_PWM_MOTOR_PWM_C0_IDX, 1000.0f};
MyPWM_Typedef MyPWM_Motor_B_IN1 = {PWM_MOTOR_PWM_INST, GPIO_PWM_MOTOR_PWM_C1_IDX, 1000.0f};

// ====================================================================
// Encoder 实例定义
// ====================================================================
MyEncoder_Typedef Motor_A_Encoder  = {GPIO_ENCODER_A_PORT, GPIO_ENCODER_A_A_Encoder_1_PIN, GPIO_ENCODER_A_A_Encoder_2_PIN, GPIO_MULTIPLE_GPIOB_INT_IRQN, 2U, 0, 0};
MyEncoder_Typedef Motor_B_Encoder  = {GPIO_ENCODER_B_PORT, GPIO_ENCODER_B_B_Encoder_1_PIN, GPIO_ENCODER_B_B_Encoder_2_PIN, GPIO_MULTIPLE_GPIOB_INT_IRQN, 2U, 0, 0};
