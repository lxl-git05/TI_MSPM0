#include "AllHeader.h"
#include "Y8_Driver.h"
// 0=寻迹展示模式, 1=巡线跟踪模式
static uint8_t track_mode = 0;

void Mode_5_Setup(void)
{
    Y8_Init();
    track_mode = 0;

    OLED_Printf(0, 0, OLED_6X8, "Y8 Display Mode");
}

void Mode_5_Loop(void)
{
    // KEY_1 单击: 切换展示/巡线模式
    if (Key_Check(KEY_1, KEY_SINGLE))
        track_mode = !track_mode;

    if (track_mode == 0)
    {
        // ========== 展示模式: 显示8路寻迹原始数据 ==========
        OLED_Printf(0, 0, OLED_6X8, "Y8 Display Mode   ");
        // 8路二进制
        OLED_Printf(0, 16, OLED_8X16, "%d%d%d%d%d%d%d%d",
            Y8_Data[0], Y8_Data[1], Y8_Data[2], Y8_Data[3],
            Y8_Data[4], Y8_Data[5], Y8_Data[6], Y8_Data[7]);
        // 滤波后角度
        OLED_Printf(0, 40, OLED_6X8, "Angle: %.1f deg   ", Y8_Bias);
    }
    else
    {
        // ========== 巡线模式: PID跟踪 ==========
        OLED_Printf(0, 0, OLED_6X8, "Y8 Track Mode     ");
        OLED_Printf(0, 16, OLED_8X16, "A:%.1f deg       ", Y8_Bias);
        OLED_Printf(0, 40, OLED_6X8, "Out:%.1f M_A:%.1f",
        PID_Track.setPoint,
        Motor_Get_GoalSpeed(&Motor_A));
        // PID调参
        Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Track.Kp);
        Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Track.Ki);
        Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Track.Kd);
        Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &PID_Track.realPoint_Now);
    }
}

// ★ 在 20ms 中断 (TIM7 ISR) 中调用, 保证传感器读取的实时性
void Mode_5_Tick(void)
{
    if (track_mode == 0)
    {
        // 展示模式: 仅读取传感器
        Y8_Data_Update();
    }
    else
    {
        // 巡线模式: 传感器读取 + 角度滤波 + PID + 电机控制
        Y8_PID_Update();
			Serial_printf(&Serial1 , "%.2f,%.2f,%.2f\n",PID_Track.goalPoint,PID_Track.realPoint_Now,PID_Track.setPoint);
    }
}

void Mode_5_Exit(void)
{
    Motor_Stop(&Motor_A);
    Motor_Stop(&Motor_B);
    Flash_Mode_Set(Flash_Mode_OFF);
}
