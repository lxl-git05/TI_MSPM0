#include "Encoder.h"

// 初始化
void Encoder_Init(IRQn_Type IRQN)
{
    NVIC_ClearPendingIRQ(IRQN);
    NVIC_EnableIRQ(IRQN);
}

// ==================== 更新编码器(直接放在中断处理函数子位置) ====================
// 默认两个GPIO是同一个Port,编码器为双边触发
void Encoder_Counter_Tick(GPIO_Regs* Encoder_GPIO_Port , uint32_t Encoder_Pin_1 , uint32_t Encoder_Pin_2 , int32_t* gEncoderCount)   
{
    MyEncoder_Typedef encoder = {0};
    if (gEncoderCount == 0)
    {
        return;
    }
    encoder.Encoder_GPIO_Port = Encoder_GPIO_Port;
    encoder.Encoder_Pin_1 = Encoder_Pin_1;
    encoder.Encoder_Pin_2 = Encoder_Pin_2;
    encoder.cnt = *gEncoderCount;
    MyEncoder_Counter_Tick(&encoder);
    *gEncoderCount = encoder.cnt;
}



// ==================== 获取计数值并清零 ====================
int32_t Encoder_Get_CNT(int32_t* gEncoderCount)
{
    MyEncoder_Typedef encoder = {0};
    if (gEncoderCount == 0)
    {
        return 0;
    }
    encoder.cnt = *gEncoderCount;
    *gEncoderCount = 0;
    return MyEncoder_Get_CNT(&encoder);
}
