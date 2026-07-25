#include "Mode_2.h"
#include "AllHeader.h"

static int demo_kp = 50, demo_kd = 10 ;

void Mode_2_Setup(void)
{

}

void Mode_2_Loop(void)
{
    

    // ====== 虚拟按键演示 ======
    if (LCD_Key_Pressed(LCD_KEY_1)) { demo_kp++ ; }
    if (LCD_Key_Pressed(2)) { demo_kp-- ; }
    if (LCD_Key_Pressed(3)) { demo_kd++ ; }
    if (LCD_Key_Pressed(4)) { demo_kd-- ; }

    // ====== 滑块参数演示 ======
    static float demo_ki = 0.5f ;
    LCD_Param_Set(1, &demo_kp, 0, 100) ;             // 滑块1 → Kp 0~100
    LCD_Param_Set_Float(2, &demo_ki, 0.0f, 2.0f) ;    // 滑块2 → Ki 0.0~2.0

    // OLED 显示
    OLED_Printf(0, 0, OLED_6X8, "Mode2 LCD") ;
    OLED_Printf(0, 10, OLED_6X8, "Raw:%s", Serial4.ABC_Data.Serial_New_Package_ABC) ;
    OLED_Printf(0, 20, OLED_6X8, "Kp=%d  Kd=%d", demo_kp, demo_kd) ;
    OLED_Printf(0, 30, OLED_6X8, "Ki=%.2f", demo_ki) ;
}

void Mode_2_Tick(void)
{
    Serial_printf(&Serial1, "%d,%d\n" , demo_kp , demo_kd) ;
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
