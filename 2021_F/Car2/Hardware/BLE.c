#include "BLE.h"

extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;
extern int Target_Num ;

extern int Car_1_Target_Num ;

bool Car2_Enable_Back = false ;
bool Car2_Enable_Go = false ;
bool Car1_Enable_Back = false ;

void BLE_Get(void)
{
    int cnt ;

    if (!Serial_GetNewPackageFlag_HEX(&Serial3)) { return ; }

    cnt = Serial3.Hex_Data.Serial_New_Package[0] ;

    if (cnt >= 1)
    {
        Car_1_Target_Num = Serial3.Hex_Data.Serial_New_Package[1] > 8
            ? Car_1_Target_Num : Serial3.Hex_Data.Serial_New_Package[1];
    }
    if (cnt >= 2)
    {
        Car2_Enable_Back = (Serial3.Hex_Data.Serial_New_Package[2] == 100) ;
    }
    if (cnt >= 3)
    {
        Road2[0] = Serial3.Hex_Data.Serial_New_Package[3] / 10;
        Road2[1] = Serial3.Hex_Data.Serial_New_Package[3] % 10;
    }
    if (cnt >= 4)
    {
        Road3[0] = Serial3.Hex_Data.Serial_New_Package[4] / 1000;
        Road3[1] = (Serial3.Hex_Data.Serial_New_Package[4] / 100) % 10;
        Road3[2] = (Serial3.Hex_Data.Serial_New_Package[4] / 10) % 10;
        Road3[3] = Serial3.Hex_Data.Serial_New_Package[4] % 10;
    }
    // if (cnt >= 5)
    // {
    //     Road4_L[0] = Serial3.Hex_Data.Serial_New_Package[5] / 10;
    //     Road4_L[1] = Serial3.Hex_Data.Serial_New_Package[5] % 10;
    // }
    // if (cnt >= 6)
    // {
    //     Road4_R[0] = Serial3.Hex_Data.Serial_New_Package[6] / 10;
    //     Road4_R[1] = Serial3.Hex_Data.Serial_New_Package[6] % 10;
    // }
    if (cnt >= 7)
    {
        Car2_Enable_Go = (Serial3.Hex_Data.Serial_New_Package[7] == 100) ;
    }
}

void BLE_SendData(void)
{
    uint16_t Data[BLE_DATA_COUNT];
    Data[0] = Target_Num ;
    Data[1] = 0 ;
    Data[2] = Road2[0] * 10 + Road2[1] ;
    Data[3] = Road3[0] * 1000 + Road3[1] * 100 + Road3[2] * 10 + Road3[3] ;
    Data[4] = Road4_L[0] * 10 + Road4_L[1] ;
    Data[5] = Road4_R[0] * 10 + Road4_R[1] ;
    Data[6] = 0 ;
    Data[7] = Car1_Enable_Back ? 100 : 0 ;
    Serial_Send_HEX_Package(&Serial3, Data, BLE_DATA_COUNT);
}
