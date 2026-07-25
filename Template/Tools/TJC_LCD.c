#include "TJC_LCD.h"
#include "Serial_porting.h"
#include <string.h>
#include <stdio.h>

// ============== 内部状态 ==============
static struct {
    bool key_pressed[TJC_LCD_KEY_MAX];          // 按键事件标记（读后自动清零）
    bool param_updated[TJC_LCD_PARAM_MAX];      // 参数更新标记
    int  param_value[TJC_LCD_PARAM_MAX];        // 参数原始值（0-100）
} tjc;

// ============== 初始化 ==============
void TJC_LCD_Init(void)
{
    memset(&tjc, 0, sizeof(tjc));
}

// ============== 主处理函数（在 Mode_X_Loop 首行调用）==============
void TJC_LCD_Process(void)
{
    // 轮询处理所有已接收的 ABC 数据包
    while (Serial_GetNewPackageFlag_ABC(&TJC_LCD_SERIAL))
    {
        // 拷贝到局部缓冲区，防止 ISR 异步覆盖
        char buf[TJC_LCD_BUF_SIZE];
        strncpy(buf, TJC_LCD_SERIAL.ABC_Data.Serial_New_Package_ABC, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        // ----- 虚拟按键检测 -----
        if (strstr(buf, TJC_LCD_KEY_KEYWORD) != NULL)
        {
            int key_num = 0;
            sscanf(buf, TJC_LCD_KEY_KEYWORD "_%d", &key_num);
            if (key_num >= 1 && key_num <= TJC_LCD_KEY_MAX)
            {
                tjc.key_pressed[key_num - 1] = true;
            }
        }
        // ----- 滑块参数检测 -----
        else if (strstr(buf, TJC_LCD_PARAM_KEYWORD) != NULL)
        {
            int param_id = 0;
            int value = 0;
            sscanf(buf, TJC_LCD_PARAM_KEYWORD "_%d=%d", &param_id, &value);
            if (param_id >= 1 && param_id <= TJC_LCD_PARAM_MAX)
            {
                // 限幅到 0-100
                if (value < 0)   value = 0;
                if (value > 100) value = 100;
                tjc.param_value[param_id - 1]  = value;
                tjc.param_updated[param_id - 1] = true;
            }
        }
    }
}

// ============== 虚拟按键检测 ==============
bool LCD_Key_Pressed(uint8_t key_num)
{
    if (key_num < 1 || key_num > TJC_LCD_KEY_MAX) return false;

    bool pressed = tjc.key_pressed[key_num - 1];
    tjc.key_pressed[key_num - 1] = false;   // 读后清零
    return pressed;
}

// ============== 滑块整数参数映射 ==============
bool LCD_Param_Set(uint8_t param_id, int *var, int min, int max)
{
    if (param_id < 1 || param_id > TJC_LCD_PARAM_MAX) return false;
    if (!tjc.param_updated[param_id - 1]) return false;

    // 映射：slider(0-100) → [min, max] 整数（四舍五入）
    float ratio = tjc.param_value[param_id - 1] / 100.0f;
    *var = min + (int)(ratio * (max - min) + 0.5f);

    tjc.param_updated[param_id - 1] = false;  // 消耗标记
    return true;
}

// ============== 滑块浮点参数映射 ==============
bool LCD_Param_Set_Float(uint8_t param_id, float *var, float min, float max)
{
    if (param_id < 1 || param_id > TJC_LCD_PARAM_MAX) return false;
    if (!tjc.param_updated[param_id - 1]) return false;

    // 映射：slider(0-100) → [min, max] 浮点
    float ratio = tjc.param_value[param_id - 1] / 100.0f;
    *var = min + ratio * (max - min);

    tjc.param_updated[param_id - 1] = false;  // 消耗标记
    return true;
}
