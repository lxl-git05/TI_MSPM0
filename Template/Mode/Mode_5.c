#include "Mode_5.h"
#include "AllHeader.h"

// ==================== 寻迹测试 ====================

bool Is_X = true ;
float Goal_XY ;

void Mode_5_Setup(void)
{
    Oran_XY_Init() ;
}

void Mode_5_Loop(void)
{
    // 切换
    if (Key_Check(KEY_1, KEY_LONG))
    {
        Is_X = !Is_X ;
    }
    // OLED展示
    OLED_Printf(0, 0, OLED_8X16, "===Mode_5===") ;
    if (Is_X)
    {
        if (Serial_GetNewPackageFlag_ABC(&Serial1))
        {
            // 得到数据
            Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Oran_X.Kp) ;
            Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Oran_X.Ki) ;
            Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Oran_X.Kd) ;
            Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Goal_XY) ;
            PID_Oran_X.goalPoint = Goal_XY ;
        }
    }
    else 
    {
        if (Serial_GetNewPackageFlag_ABC(&Serial1))
        {
            // 得到数据
            Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &PID_Oran_Y.Kp) ;
            Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &PID_Oran_Y.Ki) ;
            Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &PID_Oran_Y.Kd) ;
            Serial_SetFloatData(&Serial1, "Goal", "Goal=%f", &Goal_XY) ;
            PID_Oran_Y.goalPoint = Goal_XY ;
        }
    }
    
}

void Mode_5_Tick(void)
{
    // PID 计算
    Oran_XY_PID_Update() ;
    // 打印
    if (Is_X)
    {
        Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n" , PID_Oran_X.goalPoint , PID_Oran_X.realPoint_Now , PID_Oran_X.setPoint) ;
    }
    else 
    {
        Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n" , PID_Oran_Y.goalPoint , PID_Oran_Y.realPoint_Now , PID_Oran_Y.setPoint) ;
    }
}

void Mode_5_Exit(void)
{
    OLED_Clear();
}
