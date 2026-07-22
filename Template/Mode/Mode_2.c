#include "Mode_2.h"
#include "AllHeader.h"

int count1 ;
int count2 ;
float count_f ;
int8_t count_8 ;


void Mode_2_Setup(void)
{
    OLED_Clear();
    // 调参初始化
    Param_Init() ;
    // 注册参数
    Param_Register("count1" , &count1 , 1.0f , PARAM_INT32) ;
    Param_Register("count2" , &count2 , 2.0f , PARAM_INT32) ;
    Param_Register("count_f" , &count_f , 0.1f , PARAM_FLOAT) ;
    Param_Register("count_8" , &count_8 , 1.0f , PARAM_INT8) ;
}

void Mode_2_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_2=====") ;
	// 非编辑模式就是正常工作
	if (!Param_IsActive())
	{
		// 正常工作代码
        
	}

	// Param_Loop 内部会自行处理 OLED 显示 (Param_Show)
	// 编辑模式下 Param_Show 绘制完整参数列表
	Param_Loop();
}

void Mode_2_Tick(void)
{
    
}

void Mode_2_Exit(void)
{
    OLED_Clear();
}
