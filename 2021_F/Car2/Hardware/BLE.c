#include "BLE.h"

// 3号串口实现信息接收
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern int Car_1_Target_Num ; 

bool Car2_Enable_Back = false ;

void BLE_Get(void)
{
    // BLE
    if (Serial_GetNewPackageFlag_HEX(&Serial3))
    {
        // 1. 目标数字
        Car_1_Target_Num = Serial3.Hex_Data.Serial_New_Package[1] > 8 ? Car_1_Target_Num : Serial3.Hex_Data.Serial_New_Package[1];

        // // 2. 是否允许倒车
        Car2_Enable_Back = Serial3.Hex_Data.Serial_New_Package[2] == 100 ? true : false ;

        // 3. Road2
        Road2[0] = Serial3.Hex_Data.Serial_New_Package[3] / 10;
        Road2[1] = Serial3.Hex_Data.Serial_New_Package[3] % 10;

        // 4. Road3
        Road3[0] = Serial3.Hex_Data.Serial_New_Package[4] / 1000;
        Road3[1] = (Serial3.Hex_Data.Serial_New_Package[4] / 100) % 10;
        Road3[2] = (Serial3.Hex_Data.Serial_New_Package[4] / 10) % 10;
        Road3[3] = Serial3.Hex_Data.Serial_New_Package[4] % 10;

        // 5. 左路
        Road4_L[0] = Serial3.Hex_Data.Serial_New_Package[5] / 10;
        Road4_L[1] = Serial3.Hex_Data.Serial_New_Package[5] % 10;

        // 6. 右路
        Road4_R[0] = Serial3.Hex_Data.Serial_New_Package[6] / 10;
        Road4_R[1] = Serial3.Hex_Data.Serial_New_Package[6] % 10;

        static int send_cnt = 0 ;
        send_cnt ++ ;
        if (send_cnt >= 50)
        {
            send_cnt = 0 ;
            Serial_printf(&Serial1, "Car_1_Target_Num=%d,Car2_Enable_Back=%d\n",Car_1_Target_Num , Car2_Enable_Back) ;
        }
    }
}