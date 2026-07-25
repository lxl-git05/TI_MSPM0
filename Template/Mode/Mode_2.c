#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    Serial_printf(&Serial2, "Serial2 Ready.\r\n");
}

int cnt ;

void Mode_2_Loop(void)
{
    // ★ 每协议每轮只调一次 GetNewPackageFlag，存结果
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);

    if (has_hex)
    {
        uint8_t len = Serial_GetHexLen(&Serial2);
        Serial_printf(&Serial1, "@HEX_Len=%d$#\r\n", len);
        cnt++ ;
    }

    if (has_abc)
    {
        if (Serial_Check_Str(&Serial2, "Hello"))
        {
            Serial_printf(&Serial1, "@OK$#\r\n");
        }
    }

    // ===== 按键发送 =====
    if (Key_Check(KEY_2, KEY_LONG))
    {
        Serial_PrintDebug(&Serial2);
    }

    OLED_Printf(0, 20, OLED_6X8, "cnt:%d",cnt) ;
    OLED_Printf(0, 30, OLED_6X8, "%d,%d,%d,%d",Serial2.HEX_Data.data[1],Serial2.HEX_Data.data[2],Serial2.HEX_Data.data[3],Serial2.HEX_Data.data[4]) ;
}

void Mode_2_Tick(void)
{
}

void Mode_2_Exit(void)
{
}
