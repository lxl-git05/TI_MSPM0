#include "MyGPIO.h"

// 1. GPIO写
void MyGPIO_WritePin(MyGPIO_Typedef* MyGPIO , int isHigh)
{
    if ((MyGPIO == 0) || (MyGPIO->GPIO_Port == 0) || (MyGPIO->GPIO_Pin == 0U))
    {
        return;
    }

    if (isHigh != 0)
    {
        DL_GPIO_setPins(MyGPIO->GPIO_Port , MyGPIO->GPIO_Pin) ;
    }
    else
    {
        DL_GPIO_clearPins(MyGPIO->GPIO_Port , MyGPIO->GPIO_Pin) ;
    }
}

// 2. GPIO读
int MyGPIO_ReadPin(MyGPIO_Typedef* MyGPIO)
{
    if ((MyGPIO == 0) || (MyGPIO->GPIO_Port == 0) || (MyGPIO->GPIO_Pin == 0U))
    {
        return 0;
    }
    return (DL_GPIO_readPins(MyGPIO->GPIO_Port , MyGPIO->GPIO_Pin) != 0U) ? 1 : 0;
}
