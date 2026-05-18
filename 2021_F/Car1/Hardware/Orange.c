#include "Orange.h"

Pid_Typedef PID_Track ; // 寻迹PID

#define Track_Speed 120 // 寻迹的基础速度
int goalSpeed_All = Track_Speed ; // 寻迹的真实速度

// 串口通信传输消息
/*
    1. Pos偏转(+180)
    2. Line_x (+1000)
    3. Road_x (绝对值)
    4. Road_y (绝对值,240->0)
    5.6.7.8 数字,从左到右 (初始为0)
    9. Over_x (绝对值)
   10. Over_y (绝对值)
   11. isRoad_T (1 or 0)
*/

int Road2[2] = {0,0} ;          // 中端数字(2个)
int Road3[4] = {0,0,0,0} ;      // 远端数字(4个)
int Road4_L[2] = {0,0} ;        // 远端左边(2个)
int Road4_R[2] = {0,0} ;        // 远端右边(2个)

int Pos_last;           // 1.1
int Pos_curr;           // 1.2
int line_x ;            // 2.
int Road_y = 0 ;        // 4.
int Oran_Num[4] = {0} ; // 5.6.7.8. 
int Target_Num = 0 ;    // 目标数字
int Over_y ;            // 10.
bool isRoad_T = false;  // 11. 判断是否到达T字路口

void Oran_Init(void)
{
    PID_Init(&PID_Track, 1.15f, 0.0f, 0.6f, 250, -250, 1000) ;
    Serial2.Hex_Data.Serial_New_Package[2] = 1000 ; // 初始化没寻到线就是0(1000-1000)
}

void Oran_Get_Target(void)
{
    if (Serial_GetNewPackageFlag_HEX(&Serial2))
    {
        if (Serial2.Hex_Data.Serial_New_Package[5] != 0)
        {
            Target_Num = Serial2.Hex_Data.Serial_New_Package[5] ;
        }
    }
}

void Oran_Data_Update(void)
{
    if (Serial_GetNewPackageFlag_HEX(&Serial2))
    {
        line_x      = Serial2.Hex_Data.Serial_New_Package[2] - 1000 ;    // 2: x_line 
        Road_y      = Serial2.Hex_Data.Serial_New_Package[4] ;           // 4: Road_y

        Oran_Num[0] = Serial2.Hex_Data.Serial_New_Package[5];      // 5.6.7.8: 发送过来的数字
        Oran_Num[1] = Serial2.Hex_Data.Serial_New_Package[6];
        Oran_Num[2] = Serial2.Hex_Data.Serial_New_Package[7];
        Oran_Num[3] = Serial2.Hex_Data.Serial_New_Package[8];

        if (Oran_Num[0] && Oran_Num[1] && !(Oran_Num[2] && Oran_Num[3]))
        {
            Road2[0] = Oran_Num[0] ;
            Road2[1] = Oran_Num[1] ;
        }
        if (Oran_Num[0] && Oran_Num[1] && (Oran_Num[2] && Oran_Num[3]))
        {
            Road3[0] = Oran_Num[0] ;
            Road3[1] = Oran_Num[1] ;
            Road3[2] = Oran_Num[2] ;
            Road3[3] = Oran_Num[3] ;
        }
        Over_y      = Serial2.Hex_Data.Serial_New_Package[10];           // 10. 终点的y值

        isRoad_T = Serial2.Hex_Data.Serial_New_Package[11];

    }
}


// 寻迹PID代码 20ms
void Oran_Track_Tick(void)
{
    // 1. 更新香橙派参数
    Oran_Data_Update();
    // 数据处理
    int x_line_Get = 0 ;
    x_line_Get = Serial2.Hex_Data.Serial_New_Package[2] - 1000 ;
    if (x_line_Get > 160.0f){ x_line_Get = 160.0f ;}
    else if (x_line_Get < -160.0f) {x_line_Get = -160.0f ;}
    PID_Track.realPoint_Now = x_line_Get ;
    
    // 2. PID计算
    PID_Update(&PID_Track, PID_Track.realPoint_Now) ;

    // 3. 输出速度(基础速度+偏置速度)
    Motor_SetSpeed(&Motor_A, goalSpeed_All - PID_Track.setPoint) ;
    Motor_SetSpeed(&Motor_B, goalSpeed_All + PID_Track.setPoint) ;

    // 4. 展示效果
    // Serial_printf(&Serial1, "%.2f,%.2f,%.2f,%d\n",PID_Track.goalPoint ,PID_Track.realPoint_Now ,PID_Track.setPoint,Road_y);
}

void Oran_Go(void)
{
    goalSpeed_All = Track_Speed ;
}

void Oran_Stop(void)
{
    goalSpeed_All = 0 ;
}
