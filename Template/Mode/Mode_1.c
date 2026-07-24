#include "AllHeader.h"
// ========================== 脱机调参模式 ==========================
// 用途：专门用于脱机调整参数并保存到存储（如 AT24C02）
// =================================================================

/* ==================== 参数表（地址自动分配，mode放第1个）==================== */
const AT_ParamItem s_AT_Params[] = {
    // 模式存储
    { AT_PARAM_I8(&curr_mode	   , 1            )} ,   // addr=0,  int8_t,  default=0
    // 陀螺仪的M滤波3个参数
    { AT_PARAM_F(&IMU_Mahony_GyroBiasX	   , -9.26840305f  )} ,
    { AT_PARAM_F(&IMU_Mahony_GyroBiasY	   , 0.428176761f  )} ,
    { AT_PARAM_F(&IMU_Mahony_GyroBiasZ	   , -1.27142811f  )} ,
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
	// Param_Register("curr_mode",&curr_mode,1,PARAM_INT8);
	Param_Register("IMU_GX",&IMU_Mahony_GyroBiasX,0.01f,PARAM_FLOAT);
	Param_Register("IMU_GY",&IMU_Mahony_GyroBiasY,0.01f,PARAM_FLOAT);
	Param_Register("IMU_GZ",&IMU_Mahony_GyroBiasZ,0.01f,PARAM_FLOAT);

}

void Mode_1_Loop(void)
{
    OLED_Printf(0, 0, OLED_6X8, "=====Mode_1=====") ;
	// 非编辑模式就是正常工作
	static int sub_mode = 1;   // 子模式号

	if (!Param_IsActive())
	{
		// ---- KEY_1单击: 切换子模式 ----
		if (Key_Check(KEY_1, KEY_SINGLE))
		{
			sub_mode++;
			if (sub_mode > 1) sub_mode = 1;   // 当前只有1个模式，回头
		}

		// ---- OLED 显示当前子模式 ----
		OLED_Printf(0, 20, OLED_6X8, "Mode_%d", sub_mode);

		// 各种模式
		// 1. 模式1: KEY_0长按 → IMU重新标定并写入AT内存
		if (sub_mode == 1 && Key_Check(KEY_0, KEY_LONG))
		{
            // OLED展示
            OLED_Printf(0, 40, OLED_8X16, "IMU_Check...") ;
            OLED_Update();
            Timer_Delay_Ms(1000) ;  // 延迟1s开始

            // 关闭中断，防止ISR抢占I2C导致标定失败
            Timer_DisableIRQ();
            IMU_Mahony_Calibrate(1000);
            Timer_EnableIRQ();

            // 写入AT24C02（I2C，也需关中断保护）
            Timer_DisableIRQ();
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasX);
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasY);
            Param_AT24C02_Write(&IMU_Mahony_GyroBiasZ);
            Timer_EnableIRQ();

            OLED_Printf(0, 40, OLED_8X16, "IMU_OK!") ;
		}
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
