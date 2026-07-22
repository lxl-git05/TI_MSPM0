#include "Mode_2.h"
#include "AllHeader.h"

// ===== 编码器缓存 =====
static int32_t last_cnt_A = 0;
static int32_t last_cnt_B = 0;

void Mode_2_Setup(void)
{
    OLED_Clear();
    OLED_Printf(0, 0, OLED_8X16, "Serial1/2 Test");

    // Serial1/2已在Initial_All中由Serial_Init()初始化
    Serial_printf(&Serial1, "Serial1 Hello\r\n");
    Serial_printf(&Serial2, "Serial2 Hello\r\n");
}

void Mode_2_Loop(void)
{
    // 编码器
    OLED_Printf(0, 16, OLED_6X8, "A:%d T:%d   ", last_cnt_A, MyEncoder_Get_Total_CNT(&Motor_A_Encoder));
    OLED_Printf(0, 24, OLED_6X8, "B:%d T:%d   ", last_cnt_B, MyEncoder_Get_Total_CNT(&Motor_B_Encoder));

    // Serial1 ABC协议回显
    if (Serial_GetNewPackageFlag_ABC(&Serial1)) {
        OLED_Printf(0, 36, OLED_6X8, "S1:%s   ", Serial1.ABC_Data.Serial_New_Package_ABC);
        Serial_printf(&Serial1, "@%s$#\r\n", Serial1.ABC_Data.Serial_New_Package_ABC);
    }

    // Serial2 ABC协议回显
    if (Serial_GetNewPackageFlag_ABC(&Serial2)) {
        OLED_Printf(0, 48, OLED_6X8, "S2:%s   ", Serial2.ABC_Data.Serial_New_Package_ABC);
        Serial_printf(&Serial2, "@%s$#\r\n", Serial2.ABC_Data.Serial_New_Package_ABC);
    }
}

void Mode_2_Tick(void)
{
    last_cnt_A = MyEncoder_Get_CNT(&Motor_A_Encoder);
    last_cnt_B = MyEncoder_Get_CNT(&Motor_B_Encoder);
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
