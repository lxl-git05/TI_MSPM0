#include "TJC_LCD.h"
#include "Serial_porting.h"
#include <string.h>
#include <stdio.h>

// ============== 按键单击检测（类似 Key_Check(KEY_X, KEY_SINGLE)）==============
bool LCD_Key_Check(uint8_t key_num)
{
    if (key_num < 1 || key_num > TJC_LCD_KEY_MAX) return false;
    if (!Serial_GetNewPackageFlag_ABC(&TJC_LCD_SERIAL)) return false;

    char expected[16];
    snprintf(expected, sizeof(expected), "LCD_KEY_%d", key_num);

    if (strcmp(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC, expected) == 0)
        return true;   // 匹配，flag 已消费

    // 不匹配，恢复 flag 给其他函数
    TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_Flag = 1;
    return false;
}

// ============== 滑块整数映射（0-100 → [min, max]）==============
bool LCD_Set_Int(uint8_t param_id, int *value, int min, int max)
{
    if (param_id < 1 || param_id > TJC_LCD_PARAM_MAX) return false;
    if (!Serial_GetNewPackageFlag_ABC(&TJC_LCD_SERIAL)) return false;

    char prefix[16];
    snprintf(prefix, sizeof(prefix), "LCD_Param_%d=", param_id);

    int id = 0, raw = 0;
    if (strstr(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC, prefix) != NULL)
    {
        sscanf(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC,
               "LCD_Param_%d=%d", &id, &raw);
        if (id == param_id)
        {
            if (raw < 0)   raw = 0;
            if (raw > 100) raw = 100;
            *value = min + (int)((raw / 100.0f) * (max - min) + 0.5f);
            return true;
        }
    }

    TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_Flag = 1;
    return false;
}

// ============== 滑块浮点映射（0-100 → [min, max]）==============
bool LCD_Set_Float(uint8_t param_id, float *value, float min, float max)
{
    if (param_id < 1 || param_id > TJC_LCD_PARAM_MAX) return false;
    if (!Serial_GetNewPackageFlag_ABC(&TJC_LCD_SERIAL)) return false;

    char prefix[16];
    snprintf(prefix, sizeof(prefix), "LCD_Param_%d=", param_id);

    int id = 0, raw = 0;
    if (strstr(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC, prefix) != NULL)
    {
        sscanf(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC,
               "LCD_Param_%d=%d", &id, &raw);
        if (id == param_id)
        {
            if (raw < 0)   raw = 0;
            if (raw > 100) raw = 100;
            *value = min + (raw / 100.0f) * (max - min);
            return true;
        }
    }

    TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_Flag = 1;
    return false;
}

// ============== 通用指令子串匹配 ==============
bool LCD_Cmd_Check(char *keyword)
{
    if (!Serial_GetNewPackageFlag_ABC(&TJC_LCD_SERIAL)) return false;

    if (strstr(TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC, keyword) != NULL)
        return true;

    TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_Flag = 1;
    return false;
}
