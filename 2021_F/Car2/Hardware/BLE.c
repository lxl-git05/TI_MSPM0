#include "BLE.h"

// 三号串口实现信息发送
void Serial_Test(void)
{
    Serial_Printf_Normal(&Serial3, "@Hello\n$#") ;
}

