#include "Orange.h"

// Pid_Typedef PID_Line;   // 寻迹角度环
Pid_Typedef PID_Track ; // 寻迹PID
int goalSpeed_All = Track_Speed ; // 寻迹的真实速度

// 寻迹状态机
typedef enum
{
    ROAD_IDLE = 0,
    ROAD_GET_2,
    ROAD_GET_3,
    ROAD_GET_4L,
    ROAD_GET_4R,
    ROAD_FINISH
}Road_State;

const char *Road_State_Str[] =
{
    "ROAD_IDLE",
    "ROAD_GET_2",
    "ROAD_GET_3",
    "ROAD_GET_4L",
    "ROAD_GET_4R",
    "ROAD_FINISH"
};

Road_State road_State = ROAD_IDLE ;

void road_State_Change(Road_State state)
{
    road_State = state ;
    Serial_printf(&Serial1, "state = %s\n",Road_State_Str[state]) ;
}

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

int Pos_x;              // 1.
int line_x ;            // 2.
int Road_y = 0 ;        // 4.
int Oran_Num[4] = {0} ; // 5.6.7.8. 
int Target_Num = 0 ;    // 目标数字
int Over_y ;            // 10.
bool isRoad_T = false;  // 11. 判断是否到达T字路口

bool Angle_Track_Check = true ;

// 寻路判定
bool Is_Road2(void)
{
    return ( Oran_Num[0] && Oran_Num[1] && !Oran_Num[2] && !Oran_Num[3] );
}

bool Is_Road4(void)
{
    return ( Oran_Num[0] && Oran_Num[1] && Oran_Num[2] && Oran_Num[3] );
}

bool Is_Left_Road(void)
{
    // 左边必须同时属于左集合
    if(
        (Oran_Num[0] == Road3[0] || Oran_Num[0] == Road3[1]) &&
        (Oran_Num[1] == Road3[0] || Oran_Num[1] == Road3[1])
    )
    {
        return true;
    }

    return false;
}

bool Is_Right_Road(void)
{
    if(
        (Oran_Num[0] == Road3[2] || Oran_Num[0] == Road3[3]) &&
        (Oran_Num[1] == Road3[2] || Oran_Num[1] == Road3[3])
    )
    {
        return true;
    }

    return false;
}

// 获取各个路线的数据
void Road_Get(void)
{
    switch(road_State)
    {
        // ============================
        // 初始状态：等待识别中间双数字
        // ============================
        case ROAD_IDLE:

            if(Is_Road2())
            {
                Road2[0] = Oran_Num[0];
                Road2[1] = Oran_Num[1];
                road_State_Change(ROAD_GET_2);
                Serial_printf( &Serial1, "Road2 = [%d %d]\n", Road2[0], Road2[1] );
            }

            break;

        // ============================
        // 已经识别Road2
        // 等待识别4数字区域
        // ============================
        case ROAD_GET_2:
            if(Is_Road4())
            {
                Road3[0] = Oran_Num[0];
                Road3[1] = Oran_Num[1];
                Road3[2] = Oran_Num[2];
                Road3[3] = Oran_Num[3];

                road_State_Change(ROAD_GET_3);

                Serial_printf( &Serial1, "Road3 = [%d %d %d %d]\n", Road3[0], Road3[1], Road3[2], Road3[3] );
            }

            break;

        // ============================
        // 已识别4数字
        // 等待识别左右双数字
        // ============================
        case ROAD_GET_3:
            // Road4重新更新
            if(Is_Road4())
            {
                Road3[0] = Oran_Num[0];
                Road3[1] = Oran_Num[1];
                Road3[2] = Oran_Num[2];
                Road3[3] = Oran_Num[3];

                Serial_printf( &Serial1, "Road3 again = [%d %d %d %d]\n", Road3[0], Road3[1], Road3[2], Road3[3] );
            }

            if(Is_Road2())
            {
                // 如果寻找到的数字不包含目标数字就放弃本次数字记录
                if ( (Target_Num == Oran_Num[0] || Target_Num == Oran_Num[1]) == false)
                {
                    Serial_printf( &Serial1, "Bad Num = [%d %d]\n", Oran_Num[0], Oran_Num[1] );
                    break;
                }
                // 左路
                else if(Is_Left_Road())
                {
                    Road4_L[0] = Oran_Num[0];
                    Road4_L[1] = Oran_Num[1];

                    road_State_Change(ROAD_GET_4L);

                    Serial_printf( &Serial1, "Road4_L = [%d %d]\n", Road4_L[0], Road4_L[1] );
                }
                // 右路
                else if (Is_Right_Road())
                {
                    Road4_R[0] = Oran_Num[0];
                    Road4_R[1] = Oran_Num[1];

                    road_State_Change(ROAD_GET_4R);

                    Serial_printf( &Serial1, "Road4_R = [%d %d]\n", Road4_R[0], Road4_R[1] );
                }
                // 其他情况
                else 
                {
                    Serial_printf( &Serial1, "Bad Num = [%d %d]\n", Oran_Num[0], Oran_Num[1] );
                }
            }
            break;
        default:
            break;
    }
}

// 树莓派通信
void Oran_Init(void)
{
    PID_Init(&PID_Track, 1.32f, 0.0f, 1.39f, 30, -30, 1000) ;
    // PID_Init(&PID_Line ,  0.0f, 0.0f,  0.0f, 30, -30, 1000) ;
    // PID_Line.goalPoint = 0;
    Serial2.Hex_Data.Serial_New_Package[2] = 1000 ; // 初始化没寻到线就是0(1000-1000)
}

// 目标数字获取
void Oran_Get_Target(void)
{
    if (Serial2.Hex_Data.Serial_New_Package[5] != 0)
    {
        Target_Num = Serial2.Hex_Data.Serial_New_Package[5] > 8 ? Target_Num : Serial2.Hex_Data.Serial_New_Package[5];
    }
}

// 树莓派数据更新
void Oran_Data_Update(void)
{
    if (Serial_GetNewPackageFlag_HEX(&Serial2))
    {
        Pos_x       = Serial2.Hex_Data.Serial_New_Package[2] - 180  ;    // 1: Pos_x 
        line_x      = Serial2.Hex_Data.Serial_New_Package[2] - 1000 ;    // 2: x_line 
        Road_y      = Serial2.Hex_Data.Serial_New_Package[4] ;           // 4: Road_y

        Oran_Num[0] = Serial2.Hex_Data.Serial_New_Package[5];            // 5.6.7.8: 发送过来的数字
        Oran_Num[1] = Serial2.Hex_Data.Serial_New_Package[6];
        Oran_Num[2] = Serial2.Hex_Data.Serial_New_Package[7];
        Oran_Num[3] = Serial2.Hex_Data.Serial_New_Package[8];

        Road_Get() ;

        Over_y      = Serial2.Hex_Data.Serial_New_Package[10];           // 10. 终点的y值

        isRoad_T = Serial2.Hex_Data.Serial_New_Package[11];

    }
}


// 寻迹PID代码 20ms
void Oran_Track_Tick(int BaseSpeed)
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
    // PID_Update(&PID_Line , Pos_x) ; // 外环

    // 3.0 外环

    // 3.1 加1步校准
    if ((PID_Track.realPoint_Now > 20 || PID_Track.realPoint_Now < -20) && Angle_Track_Check == true)
    {
        Motor_SetSpeed(&Motor_A,  - PID_Track.setPoint) ;
        Motor_SetSpeed(&Motor_B,  + PID_Track.setPoint) ;
    }
    // 3.2 正常跑
    else 
    {
        Motor_SetSpeed(&Motor_A, BaseSpeed - PID_Track.setPoint) ;
        Motor_SetSpeed(&Motor_B, BaseSpeed + PID_Track.setPoint) ;
        Angle_Track_Check = false ;
    }

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
