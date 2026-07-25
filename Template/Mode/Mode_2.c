#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    // OLED_Printf(0, 0, OLED_8X16, "Serial1/2 Test");
    // // Serial1 ABC协议回显
    // OLED_Printf(0, 10, OLED_6X8, "S1_Str:%s   ", Serial1.ABC_Data.Serial_New_Package_ABC);
    // // Serial2 ABC协议回显
    // OLED_Printf(0, 20, OLED_6X8, "S2_Str:%s   ", Serial2.ABC_Data.Serial_New_Package_ABC);
    // // Serial1 HEX回显
    // OLED_Printf(0, 30, OLED_6X8, "S1_Hex_Len:%d", Serial1.HEX_Data.len);
    // // Serial2 HEX回显
    // OLED_Printf(0, 40, OLED_6X8, "S2_Hex_Len:%d", Serial2.HEX_Data.len);
}

void Mode_2_Tick(void)
{
    
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
