#include "BLE.h"



extern int Road2[2] ;

extern int Road3[4] ;

extern int Road4_L[2] ;

extern int Road4_R[2] ;

extern int Target_Num ;



bool Car2_Enable_Back = 0 ;

bool Car2_Enable_Go = 0 ;

bool Car1_Enable_Back = 0 ;



void BLE_SendData(void)

{

    uint16_t Data[BLE_DATA_COUNT];

    Data[0] = Target_Num ;

    Data[1] = Car2_Enable_Back ? 100 : 0 ;

    Data[2] = Road2[0] * 10 + Road2[1] ;

    Data[3] = Road3[0] * 1000 + Road3[1] * 100 + Road3[2] * 10 + Road3[3] ;

    Data[4] = Road4_L[0] * 10 + Road4_L[1] ;

    Data[5] = Road4_R[0] * 10 + Road4_R[1] ;

    Data[6] = Car2_Enable_Go ? 100 : 0 ;

    Data[7] = 0 ;

    Serial_Send_HEX_Package(&Serial3, Data, BLE_DATA_COUNT);

}



void BLE_Get(void)

{

    int cnt ;



    if (!Serial_GetNewPackageFlag_HEX(&Serial3)) { return ; }



    cnt = Serial3.Hex_Data.Serial_New_Package[0] ;

    if (cnt >= 8)

    {

        Car1_Enable_Back = (Serial3.Hex_Data.Serial_New_Package[8] == 100) ;

    }

}



void Manual_Serial1_Parse(void)

{

    int v ;

    if (!Serial_GetNewPackageFlag_ABC(&Serial1)) { return ; }

    if (Serial_SetIntData(&Serial1, "EnBack", "EnBack=%d", &v)) { Car2_Enable_Back = (v != 0) ; }

    if (Serial_SetIntData(&Serial1, "EnGo", "EnGo=%d", &v)) { Car2_Enable_Go = (v != 0) ; }

}

