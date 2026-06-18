#include "BLE.h"

// Serial3：正式向 Car2 广播 HEX；Serial1 仅 @EnBack 手动置 Enable（单车测 Car1 广播时用）

extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;
extern int Target_Num ;

bool Car2_Enable_Back = 0 ;

void BLE_SendData(void)
{
    uint16_t Data[6];
    Data[0] = Target_Num ; // 来自香橙派，经 Orange 更新
    Data[1] = Car2_Enable_Back == true ? 100 : 0 ;
    Data[2] = Road2[0] * 10 + Road2[1] ;
    Data[3] = Road3[0] * 1000 + Road3[1] * 100 + Road3[2] * 10 + Road3[3] ;
    Data[4] = Road4_L[0] * 10 + Road4_L[1] ;
    Data[5] = Road4_R[0] * 10 + Road4_R[1] ;
    Serial_Send_HEX_Package(&Serial3, Data, 6);
}

// Car1 无“收对车蓝牙”；仅 @EnBack=1$# 可手动触发 Enable 广播（正式仍由回程十字逻辑置位）
void Manual_Serial1_Parse(void)
{
    int v ;
    if (!Serial_GetNewPackageFlag_ABC(&Serial1)) { return ; }
    if (Serial_SetIntData(&Serial1, "EnBack", "EnBack=%d", &v)) { Car2_Enable_Back = (v != 0) ; }
}
