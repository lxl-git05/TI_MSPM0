#include "Mode_6.h"
#include "AllHeader.h"

// ======================== 步进电机驱动 ========================

float Angle_S ;
bool Ste1_Choice = 1 ;

void Mode_6_Setup(void)
{
    OLED_Clear();
}

void Mode_6_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "===Mode6===") ;

    Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Angle_S);

    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        if (Ste1_Choice)
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper1, (int)Angle_S, 400, 200) ;
        }
        else 
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper2, (int)Angle_S, 400, 200) ;
        }
    }
    if (Key_Check(KEY_1, KEY_LONG))
    {
        if (Ste1_Choice)
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper1, 0, 400, 200) ;
        }
        else 
        {
            Stepper_PWM_Pos_Set_Abs(&Stepper2, 0, 400, 200) ;
        }
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Ste1_Choice = !Ste1_Choice ;
        Stepper_PWM_Stop(&Stepper1) ;
        Stepper_PWM_Stop(&Stepper2) ;
    } 
    OLED_Printf(0, 20, OLED_6X8, "S1:%f",Stepper1.Pos_Now) ;
    OLED_Printf(0, 30, OLED_6X8, "S2:%f",Stepper2.Pos_Now) ;
    OLED_Printf(0, 40, OLED_6X8, "Angle_S%d:%.2f",Ste1_Choice == 1? 1 : 2,Angle_S ) ;
    Angle_S += Encoder_Get() ;
}

void Mode_6_Tick(void)
{
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Stepper1.Pos_Tar, Stepper1.Pos_Now , Stepper1.Speed_Now);
}

void Mode_6_Exit(void)
{
    OLED_Clear();
}
