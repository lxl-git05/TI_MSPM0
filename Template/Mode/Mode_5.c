#include "Mode_5.h"
#include "AllHeader.h"

void Mode_5_Setup(void)
{
    Oran_XY_Init() ;
}

float Ste_Angle ;

void Mode_5_Loop(void)
{
    OLED_Printf(0, 20, OLED_6X8, "Stepper2");
    if (LCD_Key_Check(LCD_KEY_1))
    {
        Stepper_PWM_Pos_Set_Abs(&Stepper2, Ste_Angle, 400, 200) ;
    }
    LCD_Set_Float(1, &Ste_Angle, -500, 500) ;
    OLED_Printf(0, 40, OLED_6X8, "%.2f",Ste_Angle) ;
}

void Mode_5_Tick(void)
{
    
    
}

void Mode_5_Exit(void)
{
    
}
