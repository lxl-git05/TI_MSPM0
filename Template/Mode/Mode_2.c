#include "Mode_2.h"
#include "AllHeader.h"

void Mode_2_Setup(void)
{
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "===Mode2 A_Ang===");
}

void Mode_2_Loop(void)
{
    float old_goal = Motor_A.PID_Angle.goalPoint;

    // 接收 LCD PID 参数（float 格式 + Float100 格式，两者互补）
    Serial_SetFloatData(&Serial4, "Kp", "Kp=%f", &Motor_A.PID_Angle.Kp);
    LCD_Get_ABC_Float100("Kp",   &Motor_A.PID_Angle.Kp);
    LCD_Get_ABC_Float100("Ki",   &Motor_A.PID_Angle.Ki);
    LCD_Get_ABC_Float100("Kd",   &Motor_A.PID_Angle.Kd);
    LCD_Get_ABC_Float100("Goal", &Motor_A.PID_Angle.goalPoint);

    // Goal 变化时启动电机角度控制
    if (Motor_A.PID_Angle.goalPoint != old_goal)
        Motor_SetAngle(&Motor_A, Motor_A.PID_Angle.goalPoint);

    // OLED：Motor_A 角度环
    OLED_Printf(0, 10, OLED_6X8, "Kp:%.2f Ki:%.2f", (double)Motor_A.PID_Angle.Kp, (double)Motor_A.PID_Angle.Ki);
    OLED_Printf(0, 20, OLED_6X8, "Kd:%.2f", (double)Motor_A.PID_Angle.Kd);
    OLED_Printf(0, 30, OLED_6X8, "Goal:%.1f", (double)Motor_A.PID_Angle.goalPoint);
    OLED_Printf(0, 40, OLED_6X8, "Real:%.1f", (double)Motor_A.PID_Angle.realPoint_Now);
    OLED_Printf(0, 50, OLED_6X8, "Set:%.1f", (double)Motor_A.PID_Angle.setPoint);
}

void Mode_2_Tick(void)
{
    // 角度环 PID 更新
    Motorx_Angle_Update_Tick(&Motor_A, 1);

    // 发送到 LCD 波形（ch0=Goal, ch1=Real, ch2=Set, ch3 暂留）
    Timer_Counter_Begin() ;
    TJC_LCD_Wave_Send_Float(0, Motor_A.PID_Angle.goalPoint);
    TJC_LCD_Wave_Send_Float(1, Motor_A.PID_Angle.realPoint_Now);
    TJC_LCD_Wave_Send_Float(2, Motor_A.PID_Angle.setPoint);
    TJC_LCD_Wave_Send_Float(3, Motor_A.PID_Angle.setPoint);
}

void Mode_2_Exit(void)
{
    Motor_Stop(&Motor_A);
    OLED_Clear();
}
