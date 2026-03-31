#include "ti_msp_dl_config.h"
#include "AllHeader.h"

int main(void)
{
    SYSCFG_DL_init();
    Initial_All() ;    
    
    while (1) 
    {
        if (Key_Check(KEY_1, KEY_SINGLE))
        {
            // 单击
            Flash_Mode_Set(Flash_Mode_Fast) ;
        }
        
        // OLED测试
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
