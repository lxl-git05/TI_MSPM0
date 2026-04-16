#include "Orange.h"

int Oran_Data[4] = {0} ;    // 读取的数据
int line_x ;
int goal_x ;
Pid_Typedef PID_Track ;
extern float goalSpeed_All ;

void Oran_Init(void)
{
    PID_Init(&PID_Track, 0.0f, 0.0f, 0.0f, 250, -250, 1000) ;
    PID_Track.goalPoint = 0 ;
}

void Oran_Data_Update(void)
{
    if (Serial_GetNewPackageFlag_HEX(&Serial2))
    {
        Timer_Counter_Func() ;
        // 第1个数据是偏转角度暂时不需要
        line_x = Serial2.Hex_Data.Serial_New_Package[2] - 1000 ;    // x_line 
    }
}

// 寻迹PID代码 20ms
void Oran_Track_Tick(void)
{
    // 1. 更新真实值
    Oran_Data_Update();
    PID_Track.realPoint_Now = (float)(Serial2.Hex_Data.Serial_New_Package[2] - 1000) ;

    // 2. PID计算
    PID_Update(&PID_Track, PID_Track.realPoint_Now) ;

    // 3. 输出速度
    Motor_SetSpeed(&Motor_A, (int)goalSpeed_All + PID_Track.setPoint) ;
    Motor_SetSpeed(&Motor_B, (int)goalSpeed_All - PID_Track.setPoint) ;

    // 4. 展示效果
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",PID_Track.goalPoint ,PID_Track.realPoint_Now ,PID_Track.setPoint );
}
