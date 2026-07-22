#ifndef __ENCODER_KEY_H
#define __ENCODER_KEY_H

#include "MySystem.h"

// EC11 旋转编码器初始化（GPIO中断已在syscfg配置，此处初始化NVIC）
void Encoder_Init(void);

// 获取编码器累计脉冲数（读取后自动清零）
int16_t Encoder_Get(void);

// EC11 编码器中断服务函数（由 GROUP1_IRQHandler GPIOA分支调用）
void EC11_Encoder_ISR(void);

#endif
