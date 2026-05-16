#include "RGB.h"

// ========================== RGB总控制 ==========================
void RGB_Set(bool isR_On , bool isG_On , bool isB_On)
{
    if (isR_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_R_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_R_PIN);
    }

    if (isG_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_G_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_G_PIN);
    }

    if (isB_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN);
    }
}

// ========================== 单色控制 ==========================

// 红灯
void RGB_R_Set(bool isR_On)
{
    if (isR_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_R_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_R_PIN);
    }
}

// 绿灯
void RGB_G_Set(bool isG_On)
{
    if (isG_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_G_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_G_PIN);
    }
}
// 蓝灯
void RGB_B_Set(bool isB_On)
{
    if (isB_On)
    {
        DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN);
    }
}