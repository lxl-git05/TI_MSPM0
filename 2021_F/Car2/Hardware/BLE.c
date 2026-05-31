#include "BLE.h"

// 3号串口实现信息接收
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern int Car_1_Target_Num ; 

// bool Car2_Enable_Back = false ;

// void BLE_Get(void)
// {
//     if (Serial_GetNewPackageFlag_HEX(&Serial3))
//     {
//         int *Data = Serial3.Hex_Data.Serial_New_Package;

//         // 1. 目标数字
//         Car_1_Target_Num = Data[1];

//         // 2. 是否允许倒车
//         Car2_Enable_Back = Data[2] == 100 ? 1 : 0 ;

//         // 3. Road2
//         Road2[0] = Data[3] / 10;
//         Road2[1] = Data[3] % 10;

//         // 4. Road3
//         Road3[0] = Data[4] / 1000;
//         Road3[1] = (Data[4] / 100) % 10;
//         Road3[2] = (Data[4] / 10) % 10;
//         Road3[3] = Data[4] % 10;

//         // 5. 左路
//         Road4_L[0] = Data[5] / 10;
//         Road4_L[1] = Data[5] % 10;

//         // 6. 右路
//         Road4_R[0] = Data[6] / 10;
//         Road4_R[1] = Data[6] % 10;
//     }
// }