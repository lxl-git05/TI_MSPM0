#include "Mode_2.h"
#include "AllHeader.h"

extern int check ;

void Mode_2_Setup(void)
{
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Angle=====") ;
}

void Mode_2_Loop(void)
{
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Angle.Kp) ;
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Angle.Ki) ;
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Angle.Kd) ;
        Serial_SetFloatData(&Serial1, "Angle", "Angle=%f", &PID_Angle.goalPoint) ;
    }
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        // static bool is_180 = true ;
        // if (is_180)
        // {
        //     DL_GPIO_setPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN) ;
        //     PID_Angle.goalPoint = 180 ;
        // }
        // // else 
        // // {
        // //     DL_GPIO_clearPins(GPIO_RGB_PORT, GPIO_RGB_LED_B_PIN) ;
        // //     PID_Angle.goalPoint = 0 ;
        // // }
        // // is_180 = !is_180 ;
        check = 1 ;
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        PID_Angle.goalPoint = 90 ;
    }
    OLED_Printf(0, 20, OLED_6X8, "yaw:%.2f",MPU_Real.yaw) ;
}

// 20ms让小车旋转固定角度
void Mode_2_Tick(void)
{
    Con_MPU_Motor_Turn180_Tick() ;
}

void Mode_2_Exit(void)
{
    
}
