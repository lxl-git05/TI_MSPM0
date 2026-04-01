#include "Con_Track.h"

uint8_t Y8_Line_Num ;							// 巡线检测到的点数
#define  Car_Length 160.0f 
float Y8_Black_Width_Arr[9] = {0,-40.25f,-28.75f,-17.25f,-5.75f,5.75f,17.25f,28.75f,40.25f} ;
float Y8_Error_Arr[9] = {0,-14.12f,-10.18f,-6.15f,-2.05f,2.05f,6.15f,10.18f,-14.12f};
float Y8_Error[25] = {0} ;
int Y8_Error_Count ;

// ====================== 寻迹特殊情况处理代码 ======================

// Y8巡线对照函数
static bool Y8_Line_Contrast(int EX1 , int EX2 , int EX3 , int EX4 , int EX5 , int EX6 , int EX7 , int EX8 )
{
	return Y8_Line_Array[1] == EX1 && Y8_Line_Array[2] == EX2 && Y8_Line_Array[3] == EX3 && Y8_Line_Array[4] == EX4 &&
		Y8_Line_Array[5] == EX5 && Y8_Line_Array[6] == EX6 && Y8_Line_Array[7] == EX7 && Y8_Line_Array[8] == EX8 ;
}

// Y8巡线采样,放入中断1ms计次
void Y8_Error_Update_Tick(void)
{	// 更新
	Y8_LineSensor_Update() ;
	// 采样
	if (Y8_Update_Flag == true)
	{
		Y8_Update_Flag = false ;
		// 上次值
		static float last_Error  = 0.0f;
		
		float sum = 0 ;
		int blackCount = 0;
		
		// 计算偏差
		for (int i = 1; i < 9; i++)
		{
			if (Y8_Line_Array[i] == 1) // 黑线有效
			{
				sum += Y8_Error_Arr[i];
				blackCount++;
			}
		}
		
		Y8_Line_Num = blackCount ;
		
		// 采样一次
		if (blackCount == 0)
		{
			Y8_Error[Y8_Error_Count] = last_Error ;
		}
		else
		{
			Y8_Error[Y8_Error_Count] = sum * 1.0f / blackCount ;
			last_Error = Y8_Error[Y8_Error_Count] ;
		}
		
		Y8_Error_Count ++ ;
		if (Y8_Error_Count == 25)
		{
			Y8_Error_Count = 0 ;
		}	
		// MPU配合巡线检查,得到巡线转数
	}
}
 