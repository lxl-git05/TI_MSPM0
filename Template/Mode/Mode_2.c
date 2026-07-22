#include "Mode_2.h"
#include "AllHeader.h"

// int count ;
// count += Encoder_Get() ;
// OLED_Printf(0, 0, OLED_6X8, "count:%d",count) ;

float Angle ;

void Mode_2_Setup(void)
{
    OLED_Clear();
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "Mode2") ;

    Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Angle);

    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        // Stepper_PWM_Speed_Set(&Stepper1, 200, 0) ;
        Stepper_PWM_Pos_Set_Abs(&Stepper1, (int)Angle, 400, 200) ;
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        Stepper_PWM_Pos_Set_Abs(&Stepper1, 0, 400, 200) ;
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Stepper_PWM_Stop(&Stepper1) ;
    } 
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Stepper1.Pos_Now , Stepper1.Speed_Now , Stepper1.Pos_Tar);
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
