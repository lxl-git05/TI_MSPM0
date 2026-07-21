#include "RGB.h"

// ========================== RGB总控制 ==========================
void RGB_Set(bool isR_On , bool isG_On , bool isB_On)
{
    if (isR_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_R, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_R, 0);
    }

    if (isG_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_G, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_G, 0);
    }

    if (isB_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_B, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_B, 0);
    }
}

// ========================== 单色控制 ==========================

// 红灯
void RGB_R_Set(bool isR_On)
{
    if (isR_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_R, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_R, 0);
    }
}

// 绿灯
void RGB_G_Set(bool isG_On)
{
    if (isG_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_G, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_G, 0);
    }
}
// 蓝灯
void RGB_B_Set(bool isB_On)
{
    if (isB_On)
    {
        MyGPIO_WritePin(&MyGPIO_RGB_B, 1);
    }
    else
    {
        MyGPIO_WritePin(&MyGPIO_RGB_B, 0);
    }
}