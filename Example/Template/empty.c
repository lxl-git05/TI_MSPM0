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
        }

        // OLED测试
        static int a = 0 ;
        OLED_ShowNum(0, 0, a++, 4, OLED_8X16);
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
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_LED_PIN_0_PIN) ;
    }

    // 功能2:按键
    Key_Tick() ;
    
}
