#include "AllHeader.h"

// 停车 寻迹直行 路口直行 左转 右转 掉头

Car_Status_Typedef curr_Status = Car_Stop;
Car_Status_Typedef next_Status = Car_Stop;

int load_cnt = 0 ;


int next_cnt = 0 ;  // 第0个状态肯定是回头,所以从1开始
int Car_Status_Load[30]      = {0,9,9,9,9,9,9,9,9,9};   // 记录小车状态
int Car_Back_Status_Load[30] = {9,9,9,9,9,9,9,9,9,9};   // 小车回城状态
bool isBack = false ;
int print_cnt = 0 ;
int T_cnt = 0 ;

extern int Road2[2] ;
extern int Road3[4] ;

// 1: 01210     check -> 51315
// 2: 01310     check -> 51215
// 3: 0141210 -> 5131415

Car_Status_Typedef Car_Status_Fan(Car_Status_Typedef Before)
{
    switch (Before) 
    {
        case Car_Stop    : return Car_Turn_H ;
        case Car_Forward : return Car_Forward ;
        case Car_Turn_L  : return Car_Turn_R ;
        case Car_Turn_R  : return Car_Turn_L ;
        case Car_Turn_F  : return Car_Turn_F ;
        default: return 9;
    }
}

void Car_Status_Back_Set(void)
{
    int back_index = 0;
    Car_Back_Status_Load[back_index++] = Car_Turn_H ;
    
    for (int i = load_cnt; i >= 0; i--)
    {
        Car_Back_Status_Load[back_index++] =
        Car_Status_Fan(Car_Status_Load[i]);
        Serial_printf(&Serial1, "Car_Status_Load[i] = %d, Car_Back_Status_Load[back_index]%d\n",Car_Status_Load[i],Car_Back_Status_Load[back_index-1]);
    }
}

const char *Str[] =  {"Stop" , "Forward" , "Turn_L" , "Turn_R" , "Car_Turn_F" , "Car_Turn_H" } ;

typedef enum 
{
    Track_Null ,
    Track_Inter ,
    Track_T_Inter ,
    Track_Over ,
}Track_Status_Typedef;

Track_Status_Typedef Track_Status      = Track_Null;
Track_Status_Typedef Track_Status_Back = Track_Null;

// 小车状态转换台
void Car_Control_Change(void)
{
    // 当前状态和下次状态相同才能进入切换状态
    if (curr_Status != next_Status) {return;}
    // 打印读取的数据
    
    if (isBack == false)
    {
        if ((Oran_Num[0]))
        {
            Serial_printf(&Serial1 ,"road2:%d%droad4:%d%d%d%d\n",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]) ; 
        }
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                if (Road_y > 230)     {Track_Status = Track_Inter   ; Serial_printf(&Serial1 ,"Inter\n");}                      // 检测到路口 
                if (isRoad_T == true) {Track_Status = Track_T_Inter ; isRoad_T = false ;Serial_printf(&Serial1 ,"T_Inter\n");}  // 检测到丁字路口
                if (Over_y > 180)     {Track_Status = Track_Over    ; Serial_printf(&Serial1 ,"Over\n");}                       // 到达终点
                // 下一状态配置
                if (Track_Status == Track_Inter)
                {
                    if      (Target_Num == 1)           {next_Status = Car_Turn_L ;}
                    else if (Target_Num == 2)           {next_Status = Car_Turn_R ;}
                    else if (Target_Num == Road2[0])    {next_Status = Car_Turn_L ;}
                    else if (Target_Num == Road2[1])    {next_Status = Car_Turn_R ;}
                    else                                {next_Status = Car_Turn_F ;}    // 路口直行
                }
                else if (Track_Status == Track_T_Inter)
                {
                    T_cnt ++ ;
                    if (T_cnt == 1)
                    {
                        if      (Target_Num == Road3[0]) {next_Status = Car_Turn_L ;}
                        else if (Target_Num == Road3[1]) {next_Status = Car_Turn_L ;}
                        else if (Target_Num == Road3[2]) {next_Status = Car_Turn_R ;}
                        else if (Target_Num == Road3[3]) {next_Status = Car_Turn_R ;}
                    }
                    else
                    {
                        if (Target_Num == Road2[0])         {next_Status = Car_Turn_L ;}
                        else if (Target_Num == Road2[1])    {next_Status = Car_Turn_R ;}
                    }
                }
                else if (Track_Status == Track_Over )
                {
                    next_Status = Car_Stop ;
                    Car_Status_Back_Set() ;
                }
                Track_Status = Track_Null ;
                break;
            }
            case Car_Turn_L : 
            {
                if (Con_MPU_Get_Yaw() > 90) {next_Status = Car_Forward ;}
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) {next_Status = Car_Forward ;}
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {next_Status = Car_Forward;}
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) {next_Status = Car_Forward ;}
                break;
            }
            case Car_Stop:
            {
                break;
            }
        }
    }
    else
    {
        switch (curr_Status) 
        {
            case Car_Forward: 
            {
                if (Road_y > 230)     {Track_Status_Back = Track_Inter   ; Serial_printf(&Serial1 ,"Inter\n"); next_Status = Car_Back_Status_Load[next_cnt++] ;}                      // 检测到路口 
                if (isRoad_T == true) {Track_Status_Back = Track_T_Inter ; isRoad_T = false ;Serial_printf(&Serial1 ,"T_Inter\n"); next_Status = Car_Back_Status_Load[next_cnt++] ;}  // 检测到丁字路口
                if (Over_y > 180)     {Track_Status_Back = Track_Over    ; Serial_printf(&Serial1 ,"Over\n");next_Status = Car_Stop;}                       // 到达终点
                Track_Status_Back = Track_Null ;
                break;
            }
            case Car_Turn_L : 
            {
                if (Con_MPU_Get_Yaw() > 90) {next_Status = Car_Back_Status_Load[next_cnt++] ;}
                break;
            }
            case Car_Turn_R : 
            {
                if (Con_MPU_Get_Yaw() < -90) {next_Status = Car_Back_Status_Load[next_cnt++] ;}
                break;
            }
            case Car_Turn_F : 
            {
                if (Road_y < 20) {next_Status = Car_Back_Status_Load[next_cnt++] ;}
                break;
            }
            case Car_Turn_H : 
            {
                if (Con_MPU_Get_Yaw() > 180) {next_Status = Car_Back_Status_Load[next_cnt++] ;}
                break;
            }
            case Car_Stop:
            {
                break;
            }
        }
    }
    
}

// 电机外环控制台
void Car_Control(void)
{
    if (curr_Status == next_Status)
    {
        switch (curr_Status)
        {
            case Car_Forward  : Oran_Track_Tick() ;       break;    // 寻迹环
            case Car_Turn_L   : Con_MPU_Motor_Tick() ;    break;    // 角度环左转
            case Car_Turn_R   : Con_MPU_Motor_Tick() ;    break;    // 角度环右转
            case Car_Turn_F   : Motor_SetSpeed(&Motor_A , 120) ;Motor_SetSpeed(&Motor_B , 120) ; break; // 路口直行
            case Car_Turn_H   : Con_MPU_Motor_Tick() ;    break;    // half圈,也就是180度翻转
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    if (curr_Status != next_Status) // 这里一定要用if,因为条件改变是在==的条件下完成的,否则下一刻curr会与next相等,导致永远到不了!=     
    {
        if (isBack == false) { Car_Status_Load[++load_cnt] = next_Status ; }    // 记录送药过程的状态,0号就是Stop

        Serial_printf(&Serial1 , "%d,next:%s\n",print_cnt ++ ,Str[next_Status]) ;

        switch (next_Status) // setup
        {
            case Car_Forward  : break;
            case Car_Turn_L   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw( 100) ; break;  // 左转
            case Car_Turn_R   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw(-100) ; break;  // 右转
            case Car_Turn_F   : break; // 路口直行
            case Car_Turn_H   : Con_MPU_Yaw_Reset() ; Con_MPU_Tar_Yaw( 200) ; break;  // 180度旋转
            case Car_Stop     : Motor_SetSpeed(&Motor_A , 0) ;Motor_SetSpeed(&Motor_B , 0) ; break; // 停车
        }
    }
    // 状态切换
    curr_Status = next_Status ;
}

