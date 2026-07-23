#include "Mode_2.h"
#include "AllHeader.h"

float Angle = 360 ;

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
        Stepper_PWM_Pos_Set_Abs(&Stepper1,  20,  50, 0) ;
        Stepper_PWM_Pos_Set_Abs(&Stepper2, -20,  50, 0) ;
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        Stepper_PWM_Pos_Set_Abs(&Stepper1, 0, 400, 200) ;
        Stepper_PWM_Pos_Set_Abs(&Stepper1, 0, 400, 200) ;
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Stepper_PWM_Stop(&Stepper1) ;
    } 
    OLED_Printf(0, 20, OLED_6X8, "S1:%f",Stepper1.Pos_Now) ;
    OLED_Printf(0, 30, OLED_6X8, "S2:%f",Stepper2.Pos_Now) ;
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Stepper1.Pos_Now , Stepper1.Speed_Now , Stepper1.Pos_Tar);
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
