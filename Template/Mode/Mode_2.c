#include "Mode_2.h"
#include "AllHeader.h"


void Mode_2_Setup(void)
{
    OLED_Clear();
    // 初始化参数编辑器
    Param_Init();
    // 参数注册,这里的注册和AT表注册不同，这里是到时候OLED调参需要的参数，也就是删掉就意味着不需要调这个参数，并且任何全局变量都能放入本表，无论有没有在AT
    Param_Register("curr_mode",&curr_mode,1,PARAM_INT8);
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_2=====") ;
	
}

void Mode_2_Tick(void)
{
    
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
