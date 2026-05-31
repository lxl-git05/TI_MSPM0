#include "BLE.h"

// 3号串口实现信息发送
// 主要是小车1发送指令,小车2执行即可
/* 指令集:
    1. @Car1_Target=%d$#    // 小车1的目标数字,在提高2用于告知小车2
    
    2. @Car2_Enable_Back$#  // 用于提高1、2 and 拓展: 允许小车2去到病房

    3. 数模信息发送
    Serial_Printf_Normal(&Serial3, "@rd_two_L=%d$#",Road2[0]) ; // 发送中端数模给小车2
    Serial_Printf_Normal(&Serial3, "@rd_two_R=%d$#",Road2[1]) ; // 发送中端数模给小车2
*/



