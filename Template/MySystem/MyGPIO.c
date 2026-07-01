#include "MyGPIO.h"

// 0. 全局GPIO声明(需要进行操作的才声明)
MyGPIO_Typedef MyGPIO_LED0          = {GPIO_LED_PORT , GPIO_LED_LED_PIN_0_PIN};

MyGPIO_Typedef MyGPIO_Key0          = {GPIO_KEY_KEY_0_PORT , GPIO_KEY_KEY_0_PIN};
MyGPIO_Typedef MyGPIO_Key1          = {GPIO_KEY_KEY_1_PORT , GPIO_KEY_KEY_1_PIN};
MyGPIO_Typedef MyGPIO_Key2          = {GPIO_KEY_KEY_2_PORT , GPIO_KEY_KEY_2_PIN};

MyGPIO_Typedef MyGPIO_OLED_SCL      = {GPIO_OLED_PORT , GPIO_OLED_OLED_SCL_PIN};
MyGPIO_Typedef MyGPIO_OLED_SDA      = {GPIO_OLED_PORT , GPIO_OLED_OLED_SDA_PIN};

MyGPIO_Typedef MyGPIO_Motor_A_IN1   = {GPIO_PWM_MOTOR_PWM_C0_PORT , GPIO_PWM_MOTOR_PWM_C0_PIN};
MyGPIO_Typedef MyGPIO_Motor_A_IN2   = {GPIO_MOTOR_IN2_PORT , GPIO_MOTOR_IN2_AIN2_PIN};

MyGPIO_Typedef MyGPIO_Motor_B_IN1   = {GPIO_PWM_MOTOR_PWM_C1_PORT , GPIO_PWM_MOTOR_PWM_C1_PIN};
MyGPIO_Typedef MyGPIO_Motor_B_IN2   = {GPIO_MOTOR_IN2_PORT , GPIO_MOTOR_IN2_BIN2_PIN};

// 1. GPIO写
void My_GPIO_WritePin(MyGPIO_Typedef* MyGPIO , int isHigh)
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
int My_GPIO_ReadPin(MyGPIO_Typedef* MyGPIO)
{
    if ((MyGPIO == 0) || (MyGPIO->GPIO_Port == 0) || (MyGPIO->GPIO_Pin == 0U))
    {
        return 0;
    }
	return (DL_GPIO_readPins(MyGPIO->GPIO_Port , MyGPIO->GPIO_Pin) != 0U) ? 1 : 0;
}
