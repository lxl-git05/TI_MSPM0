// Orange 模块 — 初始化与通信更新
#include "Orange.h"
#include "AllHeader.h"

uint8_t Oran_cmd = 0 ;	// 0. 指令模式
float x_real = 0.0f;	// 1. x偏差
float y_real = 0.0f;	// 2. y偏差
int Oran_Item[4] = {0}; // 3.4.5.6. 三角形 正方形 五角星 圆形 入画数量
bool isHomeIn = false ;	// 7. 终点入画判断
bool isOver  = false  ;	// 8. 本题结束

// 香橙派数据更新,在Mode_G实现20ms固定更新
void Oran_Update(void)
{
	// 读取Serial2的消息
	if (Serial_GetNewPackageFlag_HEX(&Serial2))
	{
        // 第0位是cmd!!!所以后续都必须从1开始
		Oran_cmd = Serial_GetHexData(&Serial2 , 0) ;
        // 寻物资模式
		if (Oran_cmd == 0)
		{
			x_real  	   = Serial_GetHexData(&Serial2 , 1) - 1000 ;	// 去掉偏置
			y_real  	   = Serial_GetHexData(&Serial2 , 2) - 1000 ;	// 去掉偏置
			Oran_Item[0]   = Serial_GetHexData(&Serial2 , 3) ;
			Oran_Item[1]   = Serial_GetHexData(&Serial2 , 4) ;
			Oran_Item[2]   = Serial_GetHexData(&Serial2 , 5) ;
			Oran_Item[3]   = Serial_GetHexData(&Serial2 , 6) ;
			isHomeIn   	   = Serial_GetHexData(&Serial2 , 7) ;
			isOver	   	   = Serial_GetHexData(&Serial2 , 8) ;
		}
        // 
		else if (Oran_cmd == 1)
		{

        }

    }
}

// ======================= 香橙派寻迹PID =======================
// (外环: X: 小车的左右速度+-,保持中心 Y: 小车的主速度,去往目标位置  内环: 速度环)
Pid_Typedef PID_Oran_X ;
Pid_Typedef PID_Oran_Y ;
#define Oran_XY_X_Check ( -1)	// X纠正方向
#define Oran_XY_Y_Check (  1)	// Y纠正方向


void Oran_PID_Func_X(void)
{
	if (PID_Oran_X.realPoint_Now < 40 && PID_Oran_X.realPoint_Now > -40)
	{
		PID_Oran_X.Kp = 0.3f; 
	}
	else 
	{
		PID_Oran_X.Kp = 0.649f ;
	}
}

void Oran_XY_Init(void)
{
	// 最大内环速度为 200 rpm/min
	// 目标都是偏差为0
	PID_Init(&PID_Oran_X, 0.649f, 0.0f, 5.635f, 40, -40, 400) ;
	PID_Init(&PID_Oran_Y, 1.0f, 0.0f, 7.2f, 40, -40, 140) ;
	PID_Oran_X.d_filter = 0.3f ;	// 不完全微分
	PID_Oran_X.PID_Func = Oran_PID_Func_X ;
	
}

void Oran_XY_PID_Update(void)
{
	// 1. 香橙派更新数据,得到Real值:这个是全局任务，直接放在Mode_G
	// Oran_Update() ;
	// 2. PID数据更新:real更新 goal为0 set需要求
	PID_Oran_X.realPoint_Now = x_real ;
	PID_Oran_Y.realPoint_Now = y_real ;
	// 3. PID计算
	PID_Update(&PID_Oran_X, PID_Oran_X.realPoint_Now) ;
	PID_Update(&PID_Oran_Y, PID_Oran_Y.realPoint_Now) ;
	// 4. 内环驱动: Y为主速度 X为偏移速度
	Motor_SetSpeed(&Motor_A, PID_Oran_Y.setPoint * Oran_XY_Y_Check + PID_Oran_X.setPoint * Oran_XY_X_Check) ;
	Motor_SetSpeed(&Motor_B, PID_Oran_Y.setPoint * Oran_XY_Y_Check - PID_Oran_X.setPoint * Oran_XY_X_Check) ;
}

