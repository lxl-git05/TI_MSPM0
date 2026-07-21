#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "Mode_2 Test");
    OLED_Printf(0, 16, OLED_6X8, "K1:LED ON  D:Slow");
    OLED_Printf(0, 32, OLED_6X8, "K2:LED OFF D:Topp");
    OLED_Update();

    // LED 快闪表示进入测试模式
    Flash_Mode_Set(Flash_Mode_Fast);
}

void Mode_2_Loop(void)
{
    // Key1 单击: LED 常亮
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Flash_Mode_Set(Flash_Mode_ON);
        OLED_Printf(0, 48, OLED_6X8, "Key1: ON        ");
        OLED_Update();
    }
    // Key1 双击: LED 慢闪
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Flash_Mode_Set(Flash_Mode_Slow);
        OLED_Printf(0, 48, OLED_6X8, "Key1: SLOW      ");
        OLED_Update();
    }
    // Key2 单击: LED 熄灭
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Flash_Mode_Set(Flash_Mode_OFF);
        OLED_Printf(0, 48, OLED_6X8, "Key2: OFF       ");
        OLED_Update();
    }
    // Key2 双击: LED 呼吸（Topp）
    if (Key_Check(KEY_2, KEY_DOUBLE))
    {
        Flash_Mode_Set(Flash_Mode_Topp);
        OLED_Printf(0, 48, OLED_6X8, "Key2: TOPP      ");
        OLED_Update();
    }
}

void Mode_2_Tick(void)
{
    // 20ms Tick — 暂无需求
}

void Mode_2_Exit(void)
{
    OLED_Clear();
    OLED_Update();
}
