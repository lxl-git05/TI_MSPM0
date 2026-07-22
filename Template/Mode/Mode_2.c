#include "Mode_2.h"
#include "AllHeader.h"

// 缓存最近一次Tick中读取的两路编码器增量
static int32_t last_cnt_A = 0;
static int32_t last_cnt_B = 0;

void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_8X16, "Encoder Test");
    // 显示编码器A的脉冲增量（速度指标）和累计脉冲（位置指标）
    OLED_Printf(0, 16, OLED_6X8, "A_CNT: %d   ", last_cnt_A);
    OLED_Printf(0, 26, OLED_6X8, "A_Total: %d   ", MyEncoder_Get_Total_CNT(&Motor_A_Encoder));

    // 显示编码器B的脉冲增量（速度指标）和累计脉冲（位置指标）
    OLED_Printf(0, 38, OLED_6X8, "B_CNT: %d   ", last_cnt_B);
    OLED_Printf(0, 48, OLED_6X8, "B_Total: %d   ", MyEncoder_Get_Total_CNT(&Motor_B_Encoder));
}

void Mode_2_Tick(void)
{
    // 20ms周期读取两路编码器增量，自动累加到total_cnt
    last_cnt_A = MyEncoder_Get_CNT(&Motor_A_Encoder);
    last_cnt_B = MyEncoder_Get_CNT(&Motor_B_Encoder);
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
