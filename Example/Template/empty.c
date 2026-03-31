#include "ti_msp_dl_config.h"
#include "AllHeader.h"

int a =  0 ;

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
            Serial_printf(&Serial1, "Hello %d\r\n", 666);
            Serial_printf(&Serial1, "heiha %.2f\r\n", 1.24);
        }

        // if (Serial_GetNewPackageFlag_HEX(&Serial2))
        // {
        //     OLED_ShowNum(0,  0, Serial2.Hex_Data.Serial_New_Package[0], 4, OLED_8X16) ;
        //     OLED_ShowNum(0, 15, Serial2.Hex_Data.Serial_New_Package[1], 4, OLED_8X16) ;
        //     OLED_ShowNum(0, 30, Serial2.Hex_Data.Serial_New_Package[2], 4, OLED_8X16) ;
        // }

        if (Serial_GetNewPackageFlag_ABC(&Serial1))
        {
            OLED_ShowString(0, 50, Serial1.ABC_Data.Serial_New_Package_ABC, OLED_6X8) ;
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

