#include "ti_msp_dl_config.h"
#include "AllHeader.h"

/*
    模式配置:
        增加模式: 先去Mode_Typedef加一个枚举模式, 然后建立一个新库,然后在下面加case即可
        初始模式:直接去next_mode定义处修改mode的初始值即可,程序运行后会自动跳到该模式
*/

int main(void)
{
    SYSCFG_DL_init();
    Mode_G_Setup() ;    // 全局初始化

    while (1)
    {
        Mode_G_Loop() ; 
        OLED_Update() ;

        if (curr_mode == next_mode)
        {
            switch (curr_mode)
            {
                case Mode_Null : break; // 啥也不干,也就是只有Global模式在干活
                case 1 : Mode_1_Loop() ; break;
                case 2 : Mode_2_Loop() ; break;
                case 3 : Mode_3_Loop() ; break;
                // 比赛代码
                case 4 : Mode_Con_1_Loop() ; break;
                case 5 : Mode_Con_2_Loop() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
        }
        else // 模式交接,仅在模式转换才触发一次 
        {
            switch (curr_mode) 
            {
                case Mode_Null : break;
                case 1 : Mode_1_Exit() ; break;
                case 2 : Mode_2_Exit() ; break;
                case 3 : Mode_3_Exit() ; break;
                // 比赛代码
                case 4 : Mode_Con_1_Exit() ; break;
                case 5 : Mode_Con_2_Exit() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
            switch (next_mode) 
            {
                case Mode_Null : break;
                case 1 : Mode_1_Setup() ; break;
                case 2 : Mode_2_Setup() ; break;
                case 3 : Mode_3_Setup() ; break;
                // 比赛代码
                case 4 : Mode_Con_1_Setup() ; break;
                case 5 : Mode_Con_2_Setup() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
        }
        curr_mode = next_mode ; // 状态更新
    }
}
