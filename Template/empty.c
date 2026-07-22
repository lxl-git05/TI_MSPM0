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
        OLED_Clear() ;
        Mode_G_Loop() ; 

        if (curr_mode == next_mode)
        {
            switch (curr_mode)
            {
                case Mode_Null : break; // 啥也不干,也就是只有Global模式在干活
                case Mode_1 : Mode_1_Loop() ; break;
                case Mode_2 : Mode_2_Loop() ; break;
                case Mode_3 : Mode_3_Loop() ; break;
                case Mode_4 : Mode_4_Loop() ; break;
                case Mode_5 : Mode_5_Loop() ; break;
                case Mode_6 : Mode_6_Loop() ; break;
                case Con_Mode_1 : Con_Mode_1_Loop() ; break;
                case Con_Mode_2 : Con_Mode_2_Loop() ; break;
                case Con_Mode_3 : Con_Mode_3_Loop() ; break;
                case Con_Mode_4 : Con_Mode_4_Loop() ; break;
                case Con_Mode_5 : Con_Mode_5_Loop() ; break;
                case Con_Mode_6 : Con_Mode_6_Loop() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
        }
        else // 模式交接,仅在模式转换才触发一次 
        {
            switch (curr_mode) 
            {
                case Mode_Null : break;
                case Mode_1 : Mode_1_Exit() ; break;
                case Mode_2 : Mode_2_Exit() ; break;
                case Mode_3 : Mode_3_Exit() ; break;
                case Mode_4 : Mode_4_Exit() ; break;
                case Mode_5 : Mode_5_Exit() ; break;
                case Mode_6 : Mode_6_Exit() ; break;
                case Con_Mode_1 : Con_Mode_1_Exit() ; break;
                case Con_Mode_2 : Con_Mode_2_Exit() ; break;
                case Con_Mode_3 : Con_Mode_3_Exit() ; break;
                case Con_Mode_4 : Con_Mode_4_Exit() ; break;
                case Con_Mode_5 : Con_Mode_5_Exit() ; break;
                case Con_Mode_6 : Con_Mode_6_Exit() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
            switch (next_mode) 
            {
                case Mode_Null : break;
                case Mode_1 : Mode_1_Setup() ; break;
                case Mode_2 : Mode_2_Setup() ; break;
                case Mode_3 : Mode_3_Setup() ; break;
                case Mode_4 : Mode_4_Setup() ; break;
                case Mode_5 : Mode_5_Setup() ; break;
                case Mode_6 : Mode_6_Setup() ; break;
                case Con_Mode_1 : Con_Mode_1_Setup() ; break;
                case Con_Mode_2 : Con_Mode_2_Setup() ; break;
                case Con_Mode_3 : Con_Mode_3_Setup() ; break;
                case Con_Mode_4 : Con_Mode_4_Setup() ; break;
                case Con_Mode_5 : Con_Mode_5_Setup() ; break;
                case Con_Mode_6 : Con_Mode_6_Setup() ; break;
                case Mode_End  : break; // 到头了,不要到这里来,写case是因为不然报出警告
            }
        }
        curr_mode = next_mode ; // 状态更新
        OLED_Update() ;
    }
}
