#include "AllHeader.h"
#include "Control.h"

StatusStack_Typedef stack ;

// 模式：基础题三问
bool Car_Enable = false; 
int load_cnt = 0 ;
int next_cnt = 0 ;  // 第0个状态肯定是回头,所以从1开始
int Car_Status_Load[30]      = {0,9,9,9,9,9,9,9,9,9};   // 记录小车状态
int Car_Back_Status_Load[30] = {9,9,9,9,9,9,9,9,9,9};   // 小车回城状态
bool isBack = false ;
int print_cnt = 0 ;
int T_cnt = 0 ;

extern int Road2[2] ;
extern int Road3[4] ;

const char *Str[] =  {"Stop" , "Forward" , "Turn_L" , "Turn_R" , "Car_Turn_F" , "Car_Turn_H" } ;

bool isLoad_tmp = false ;

// 1: 01210     check -> 51315
// 2: 01310     check -> 51215
// 3: 0141210 -> 5131415

void Mode_Con_1_Setup(void)
{
    OLED_Clear() ;
    StatusStack_Init(&stack) ;
    StatusStack_Push(&stack , curr_Status) ;    // 第1个状态是stop
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_1=====") ;
}

void Mode_Con_1_Loop(void)
{
    // 模拟目标数字
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Target_Num += 1 ;
    }
    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        Target_Num = (Target_Num == 0 ? 1 : Target_Num) ;    // 模拟目标数字
        isLoad_tmp = true ;
    }
    // 起跑判断
    if (Car_Enable == false)
    {
        Oran_Get_Target() ; // 得到目标数字
        if (isLoad_tmp == true && Target_Num != 0)
        {
            Car_Enable = true ;
            next_Status = Car_Forward ; // 开始行进
        }
    }
    // 回城判断
    if (Key_Check(KEY_1, KEY_LONG))
    {
        isBack = true ;
        next_Status = Car_Back_Status_Load[next_cnt++] ;
    }
    
    // OLED更新
    OLED_Clear() ;
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_Con_1=====") ;
    OLED_Printf(0, 20, OLED_6X8, "yaw=%.2f,cu=%d,ne=%d", MPU_Real.yaw,curr_Status,next_Status) ;
    OLED_Printf(0, 30, OLED_6X8, "roa=%d,tar=%d,cnt=%d,n%d", Road_y,Target_Num,load_cnt,next_cnt) ;
    // 打印前进状态
    OLED_Printf(0, 40, OLED_6X8, "%d%d%d%d%d%d%d%d", Car_Status_Load[0],Car_Status_Load[1],Car_Status_Load[2],Car_Status_Load[3], Car_Status_Load[4],
    Car_Status_Load[5],Car_Status_Load[6],Car_Status_Load[7]) ;
    // 打印回家状态
    OLED_Printf(0, 50, OLED_6X8, "%d%d%d%d%d%d%d%d", Car_Back_Status_Load[0],Car_Back_Status_Load[1],Car_Back_Status_Load[2],Car_Back_Status_Load[3],
    Car_Back_Status_Load[4],Car_Back_Status_Load[5],Car_Back_Status_Load[6],Car_Back_Status_Load[7]) ;
}

void Mode_Con_1_Exit(void)
{

}

void Mode_Con_1_Tick(void)
{

}


// 控制台记录状态逻辑
void Car_Status_Store(void)
{
    // 这是额外的过程-记录状态
    if (isBack == false) { Car_Status_Load[++load_cnt] = next_Status ; }    // 记录送药过程的状态,0号就是Stop
    Serial_printf(&Serial1 , "%d,next:%s\n",print_cnt ++ ,Str[next_Status]) ;

    // 状态入栈
    StatusStack_Push(&stack , next_Status) ;
}

// 回城状态翻转逻辑
Car_Status_Typedef Car_Status_Fan(Car_Status_Typedef Before)
{
    switch (Before) 
    {
        case Car_Stop    : return Car_Turn_H ;
        case Car_Forward : return Car_Forward ;
        case Car_Turn_L  : return Car_Turn_R ;
        case Car_Turn_R  : return Car_Turn_L ;
        case Car_Turn_F  : return Car_Turn_F ;
        default: return 6;
    }
}

// 回城状态配置
void Car_Status_Back_Set(void)
{
    int back_index = 0;
    Car_Back_Status_Load[back_index++] = Car_Turn_H ;
    
    for (int i = load_cnt; i >= 0; i--)
    {
        Car_Back_Status_Load[back_index++] = Car_Status_Fan(Car_Status_Load[i]);
        Serial_printf(&Serial1, "Car_Status_Load[i] = %d, Car_Back_Status_Load[back_index]%d\n",Car_Status_Load[i],Car_Back_Status_Load[back_index-1]);
    }
}

// 小车状态转换台
void Car_Control_Change(void)
{
    if (Over_y)
    {
        Serial_printf(&Serial1 ,"Over_y = %d ", Over_y);
    }
    if (Road_y)
    {
        Serial_printf(&Serial1 ,"Road_y = %d ", Road_y);
    }
    

    // 当前状态和下次状态相同才能进入切换状态
    if (curr_Status != next_Status) {return;}
    if (isBack == false)
    {
        // 打印读取的数据
        // if ((Oran_Num[0]))
        // {
        //     Serial_printf(&Serial1 ,"road2:%d%droad4:%d%d%d%d\n",Road2[0],Road2[1],Road3[0],Road3[1],Road3[2],Road3[3]) ; 
        // }
        
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
                    // 栈打印
                    Car_Status_Typedef temp_status ;
                    while(!StatusStack_IsEmpty(&stack))
                    {
                        StatusStack_Pop(&stack , &temp_status) ;
                        Serial_printf(&Serial1, "stack:%d\n",temp_status) ;
                    }
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
