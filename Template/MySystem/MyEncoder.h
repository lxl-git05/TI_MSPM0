#ifndef __MYENCODER_H
#define __MYENCODER_H

/* TI MSPM0 编码器: 双GPIO双边沿中断模式(2分频)
 * 与 F407 硬件编码器模式不同，MSPM0 使用 GPIO EXTI 双边沿触发
 * Pin_A 中断 + Pin_B 电平判断方向，实现 2 倍频计数 */

#include "MySystem.h"

// 编码器引脚对 — 两相组合为一个编码器整体（同一端口，共享GROUP中断）
typedef struct {
    GPIO_Regs *port;       // GPIO端口（A/B两相必须在同一端口）
    uint32_t pin_A;        // A相引脚
    uint32_t pin_B;        // B相引脚
} MyEncoder_Pins;

// 编码器结构体
typedef struct {
    MyEncoder_Pins pins;       // 2个GPIO作为一个编码器整体
    IRQn_Type IRQN;            // 中断号
    uint32_t time_Fre;         // 倍频数(2=仅A相双边沿, 4=双相双边沿)
    volatile int32_t cnt;      // 本次周期脉冲增量
    int32_t total_cnt;         // 累计脉冲数
} MyEncoder_Typedef;

// 编码器外部实例（在MySystem.c中定义）
extern MyEncoder_Typedef Motor_A_Encoder;
extern MyEncoder_Typedef Motor_B_Encoder;

// 1. 编码器初始化 — 清除并使能NVIC中断
void MyEncoder_Init(MyEncoder_Typedef *encoder);

// 2. 编码器中断服务（放在GROUP1_IRQHandler中调用）
void MyEncoder_ISR(MyEncoder_Typedef *encoder);

// 3. 获取并清零本次周期脉冲增量（自动累加到total_cnt）
int MyEncoder_Get_CNT(MyEncoder_Typedef *encoder);

// 4. 获取累计脉冲数
int MyEncoder_Get_Total_CNT(MyEncoder_Typedef *encoder);

// 5. 清除累计脉冲数
void MyEncoder_Total_Cnt_Clear(MyEncoder_Typedef *encoder);

#endif
