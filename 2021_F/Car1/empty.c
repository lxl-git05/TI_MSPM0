#include "ti_msp_dl_config.h"
#include "AllHeader.h"
#include "Motor.h"
#include "Con_Motor.h"

int count ;
int speed ;
int k = 0 ;
int m = 0 ;
int add = 10 ;

float Kp = 0.0f;

int main(void)
{
    SYSCFG_DL_init();
    Initial_All() ;    
    
    Motor_Init() ;  // 电机初始化,当前只有A
    
    // 定时器初始化(放在最后面)
    Timer_0_Init() ;
    Timer_1_Init() ;
    
    while (1) 
    {
        if (Key_Check(KEY_0, KEY_SINGLE))
        {
            // 单击
            Flash_Mode_Set(Flash_Mode_Fast) ;
            Serial_printf(&Serial1, "hello%d\n",666) ;
        }
        // OLED检查
        OLED_ShowSignedNum(0,  0, Motor_A.PID_s.realPoint_Now, 3, OLED_8X16);
        // Serial参数更改
        if (Serial_GetNewPackageFlag_ABC(&Serial1))
        {
            // 得到数据
            Serial_SetFloatData(&Serial1, "Kp", "Kp=%f", &Motor_A.PID_s.Kp) ;
            Serial_SetFloatData(&Serial1, "Ki", "Ki=%f", &Motor_A.PID_s.Ki) ;
            Serial_SetFloatData(&Serial1, "Kd", "Kd=%f", &Motor_A.PID_s.Kd) ;
            Serial_SetFloatData(&Serial1, "goalPoint_A", "goalPoint_A=%f", &Motor_A.PID_s.goalPoint) ;
        
            Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint) ;
        }
        // OLED展示
        OLED_Printf(0, 20, OLED_6X8, "%.2f,%.2f,%.2f" , Motor_A.PID_s.Kp , Motor_A.PID_s.Ki , Motor_A.PID_s.Kd) ;
        
        // OLED_Printf(0, 40, OLED_6X8, "%d,%d,%d" , Motor_A.PID_s.goalPoint , Motor_A.PID_s.realPoint_Now , Motor_A.PID_s.setPoint) ;

        // OLED更新
        OLED_Update();
    }
}

// 1ms定时器
void Timer_0_Callback(void)
{
    // 功能1:LED闪烁
    static int cnt = 0 ;
    cnt ++ ;
    if (cnt >= 1000 -1)
    {
        cnt = 0 ;
    }

    // 功能2:按键
    Key_Tick() ;

    // 功能3:LED闪烁监控
    Flash_Mode_Tick() ;
}

// 20ms定时器
void Timer_1_Callback(void)
{
    Motor_Update_Tick(&Motor_A) ;
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",Motor_A.PID_s.goalPoint ,Motor_A.PID_s.realPoint_Now ,Motor_A.PID_s.setPoint );
}
