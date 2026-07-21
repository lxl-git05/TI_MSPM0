#ifndef __MYGPIO_H
#define __MYGPIO_H

#include "MySystem.h"

// GPIO结构体定义
typedef struct
{
    GPIO_Regs *GPIO_Port;       // GPIO端口（GPIOA / GPIOB）
    uint32_t  GPIO_Pin;         // GPIO引脚（DL_GPIO_PIN_n）
} MyGPIO_Typedef;

// ==================== GPIO 外部实例（在 MySystem.c 中定义）====================

// 基础 I/O
extern MyGPIO_Typedef MyGPIO_LED0          ;   // LED 指示灯

extern MyGPIO_Typedef MyGPIO_Key0          ;   // 按键0（已预留，勿用于测试）
extern MyGPIO_Typedef MyGPIO_Key1          ;   // 按键1（可用于交互测试）
extern MyGPIO_Typedef MyGPIO_Key2          ;   // 按键2（可用于交互测试）
extern MyGPIO_Typedef MyGPIO_Key3          ;   // 按键3（预留）

// OLED 软件 I2C
extern MyGPIO_Typedef MyGPIO_OLED_SCL      ;   // OLED 时钟线
extern MyGPIO_Typedef MyGPIO_OLED_SDA      ;   // OLED 数据线

// 电机驱动（DC Motor A/B）
extern MyGPIO_Typedef MyGPIO_Motor_A_IN1   ;   // 电机A 方向控制1
extern MyGPIO_Typedef MyGPIO_Motor_A_IN2   ;   // 电机A 方向控制2
extern MyGPIO_Typedef MyGPIO_Motor_B_IN1   ;   // 电机B 方向控制1
extern MyGPIO_Typedef MyGPIO_Motor_B_IN2   ;   // 电机B 方向控制2

// RGB LED
extern MyGPIO_Typedef MyGPIO_RGB_R         ;   // RGB 红色通道
extern MyGPIO_Typedef MyGPIO_RGB_G         ;   // RGB 绿色通道
extern MyGPIO_Typedef MyGPIO_RGB_B         ;   // RGB 蓝色通道

// 其他外设（来自F407，暂未分配引脚）
extern MyGPIO_Typedef MyGPIO_Buzzer        ;   // 蜂鸣器
extern MyGPIO_Typedef MyGPIO_EC11_Key      ;   // EC11 编码器按键
extern MyGPIO_Typedef MyGPIO_Elec          ;   // 电磁阀
extern MyGPIO_Typedef MyGPIO_TCRT          ;   // TCRT 载重传感器

// 步进电机
extern MyGPIO_Typedef MyGPIO_Stepper_En    ;   // 步进1 使能
extern MyGPIO_Typedef MyGPIO_Stepper2_En   ;   // 步进2 使能
extern MyGPIO_Typedef MyGPIO_Stepper_Dir   ;   // 步进1 方向
extern MyGPIO_Typedef MyGPIO_Stepper2_Dir  ;   // 步进2 方向

// Y8 巡线模块地址线
extern MyGPIO_Typedef MyGPIO_Y8_Addr0     ;   // Y8 地址位0
extern MyGPIO_Typedef MyGPIO_Y8_Addr1     ;   // Y8 地址位1
extern MyGPIO_Typedef MyGPIO_Y8_Addr2     ;   // Y8 地址位2

// ==================== GPIO 操作函数 ====================

// 兼容旧名（过渡期）
#define My_GPIO_WritePin   MyGPIO_WritePin
#define My_GPIO_ReadPin    MyGPIO_ReadPin

void MyGPIO_WritePin(MyGPIO_Typedef* MyGPIO , int isHigh) ;    // GPIO 写
int  MyGPIO_ReadPin(MyGPIO_Typedef* MyGPIO) ;                  // GPIO 读

#endif // !__MYGPIO_H
