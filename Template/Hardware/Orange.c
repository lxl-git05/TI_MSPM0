// Orange 模块 — 初始化与通信更新
#include "Orange.h"
#include "AllHeader.h"

uint8_t Oran_cmd = 0 ;

float x_real = 0.0f;
float y_real = 0.0f;
float x_tar  = 0.0f;
float y_tar  = 0.0f;

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
			x_tar  = (float)(Serial_GetHexData(&Serial2 , 1) == 0 ?  x_tar  :  Serial_GetHexData(&Serial2 , 1)) ;
			y_tar  = (float)(Serial_GetHexData(&Serial2 , 2) == 0 ?  y_tar  :  Serial_GetHexData(&Serial2 , 2)) ;
		}
        // 
		else if (Oran_cmd == 1)
		{

        }

    }
}


