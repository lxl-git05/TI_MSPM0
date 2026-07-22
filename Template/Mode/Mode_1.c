#include "AllHeader.h"
// ========================== 脱机调参模式 ==========================
// 用途：专门用于脱机调整参数并保存到存储（如 AT24C02）
// =================================================================

/* ==================== 参数表（地址自动分配，mode放第1个）==================== */
const AT_ParamItem s_AT_Params[] = {
    // 模式存储
    { AT_PARAM_I8(&curr_mode	   , 1            )} ,   // addr=0,  int8_t,  default=0
    
    // 香橙派数据
    // 各个任务的阈值
};	

int At_Size = sizeof(s_AT_Params)/sizeof(s_AT_Params[0]) ;

void Mode_1_Setup(void)
{
    OLED_Clear() ;
    // 初始化调值表
    Param_Init();
	// Param_Register 内会自动检测 AT 关联并载入已存值
	Param_Register("curr_mode",&curr_mode,1,PARAM_INT8);
    Param_Register("Stp1_Kp",  &Motor_A.PID_s.Kp,0.01f,PARAM_FLOAT);
}

void Mode_1_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_1=====") ;
	// 非编辑模式就是正常工作
	if (!Param_IsActive())
	{
		
	}
	// Param_Loop 内部会自行处理 OLED 显示 (Param_Show)
	// 编辑模式下 Param_Show 绘制完整参数列表
	Param_Loop();
}

void Mode_1_Exit(void)
{

}

// 打印电机A参数
void Mode_1_Tick(void)
{

}
