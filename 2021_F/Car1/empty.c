#include "ti_msp_dl_config.h"
#include "AllHeader.h"
#include "Motor.h"

int count ;
int speed ;
int k = 0 ;
int m = 0 ;
int add = 10 ;

int main(void)
{
    SYSCFG_DL_init();
    Initial_All() ;    

    Motor_A_Init() ;
    
    // 定时器初始化(放在最后面)
    Timer_0_Init() ;
    Timer_1_Init() ;
    
    while (1) 
    {
        if (Key_Check(KEY_0, KEY_SINGLE))
        {
            // 单击
            Flash_Mode_Set(Flash_Mode_Fast) ;
        }
        // OLED检查
        OLED_ShowSignedNum(0,  0, Motor_A.PID_s.realPoint_Now, 3, OLED_8X16);
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
}
