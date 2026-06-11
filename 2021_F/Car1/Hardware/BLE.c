#include "BLE.h"

// 3号串口实现信息发送
extern int Road2[2] ;
extern int Road3[4] ;
extern int Road4_L[2] ;
extern int Road4_R[2] ;

extern int Target_Num ; 

bool Car2_Enable_Back = 0 ;

void BLE_SendData(void)
{
    uint16_t Data[6];

    // 1. 目标数字
    Data[0] = Target_Num;

    // 2. 是否允许Car2倒车
    Data[1] = Car2_Enable_Back == true ? 100 : 0 ; // 100对应true, 0对应false

    // 3. Road2
    Data[2] = Road2[0] * 10
            + Road2[1];

    // 4. Road3
    Data[3] = Road3[0] * 1000
            + Road3[1] * 100
            + Road3[2] * 10
            + Road3[3];

    // 5. 左路
    Data[4] = Road4_L[0] * 10
            + Road4_L[1];

    // 6. 右路
    Data[5] = Road4_R[0] * 10
            + Road4_R[1];

    Serial_Send_HEX_Package(&Serial3, Data, 6);

    static int send_cnt = 0 ;
    send_cnt ++ ;
//     if (send_cnt >= 50)
//     {
//         send_cnt = 0 ;
//         Serial_printf(&Serial1, "Tar=%d,Car2_Enable_Back=%d\n",Data[0] , Data[1]) ;
//     }
}

