#include "Serial_porting.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// ============== 全局实例 ==============
Serial_Typedef Serial1;
#ifdef Serial2_Enable
Serial_Typedef Serial2;
#endif
#ifdef Serial3_Enable
Serial_Typedef Serial3;
#endif
#ifdef Serial4_Enable
Serial_Typedef Serial4;
#endif

// ============== 内部函数声明 ==============
static void Serial_Data_Check_HEX(Serial_Typedef *pSerial);
static void Serial_Data_Check_ABC(Serial_Typedef *pSerial);
static uint16_t Merge_2Bytes(uint8_t high, uint8_t low);

// ============== 初始化 ==============
void Serial_Init(void)
{
    // ----- Serial1 (UART_0, USB调试, 115200) -----
    Serial1.uart  = UART_0_INST;
    Serial1.IRQN  = UART_0_INST_INT_IRQN;
    Serial1.rxLen = 0;
    Serial1.rxOverflow = false;
    Serial1.Status = 0;
    Serial1.rx_temp = 0;
    memset(Serial1.rxBuf, 0, sizeof(Serial1.rxBuf));
    memset(&Serial1.ABC_Data, 0, sizeof(Serial1.ABC_Data));
    memset(&Serial1.HEX_Data, 0, sizeof(Serial1.HEX_Data));
    NVIC_ClearPendingIRQ(Serial1.IRQN);
    NVIC_EnableIRQ(Serial1.IRQN);

#ifdef Serial2_Enable
    // ----- Serial2 (UART_1, 树莓派通信, 115200) -----
    Serial2.uart  = UART_1_INST;
    Serial2.IRQN  = UART_1_INST_INT_IRQN;
    Serial2.rxLen = 0;
    Serial2.rxOverflow = false;
    Serial2.Status = 0;
    Serial2.rx_temp = 0;
    memset(Serial2.rxBuf, 0, sizeof(Serial2.rxBuf));
    memset(&Serial2.ABC_Data, 0, sizeof(Serial2.ABC_Data));
    memset(&Serial2.HEX_Data, 0, sizeof(Serial2.HEX_Data));
    NVIC_ClearPendingIRQ(Serial2.IRQN);
    NVIC_EnableIRQ(Serial2.IRQN);
#endif

#ifdef Serial3_Enable
    // ----- Serial3 (UART_2, PB15/PB16, 115200) -----
    Serial3.uart  = UART_2_INST;
    Serial3.IRQN  = UART_2_INST_INT_IRQN;
    Serial3.rxLen = 0;
    Serial3.rxOverflow = false;
    Serial3.Status = 0;
    Serial3.rx_temp = 0;
    memset(Serial3.rxBuf, 0, sizeof(Serial3.rxBuf));
    memset(&Serial3.ABC_Data, 0, sizeof(Serial3.ABC_Data));
    memset(&Serial3.HEX_Data, 0, sizeof(Serial3.HEX_Data));
    NVIC_ClearPendingIRQ(Serial3.IRQN);
    NVIC_EnableIRQ(Serial3.IRQN);
#endif

#ifdef Serial4_Enable
    // ----- Serial4 (UART_4, 串口屏, 115200) -----
    Serial4.uart  = UART_4_INST;
    Serial4.IRQN  = UART_4_INST_INT_IRQN;
    Serial4.rxLen = 0;
    Serial4.rxOverflow = false;
    Serial4.Status = 0;
    Serial4.rx_temp = 0;
    memset(Serial4.rxBuf, 0, sizeof(Serial4.rxBuf));
    memset(&Serial4.ABC_Data, 0, sizeof(Serial4.ABC_Data));
    memset(&Serial4.HEX_Data, 0, sizeof(Serial4.HEX_Data));
    NVIC_ClearPendingIRQ(Serial4.IRQN);
    NVIC_EnableIRQ(Serial4.IRQN);
#endif

    // ★ 使能错误中断（借鉴Car1：防止噪声导致FIFO锁死）
    DL_UART_Main_enableInterrupt(UART_0_INST,
        DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
        DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
        DL_UART_MAIN_INTERRUPT_BREAK_ERROR);
#ifdef Serial2_Enable
    DL_UART_Main_enableInterrupt(UART_1_INST,
        DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
        DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
        DL_UART_MAIN_INTERRUPT_BREAK_ERROR);
#endif
#ifdef Serial3_Enable
    DL_UART_Main_enableInterrupt(UART_2_INST,
        DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
        DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
        DL_UART_MAIN_INTERRUPT_BREAK_ERROR);
#endif
#ifdef Serial4_Enable
    DL_UART_Main_enableInterrupt(UART_4_INST,
        DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
        DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
        DL_UART_MAIN_INTERRUPT_BREAK_ERROR);
#endif

    // 初始化协议常量
    Serial_Agreement_ABC_Init();
    Serial_Agreement_HEX_Init();
}

// ============== 发送（阻塞式printf）==============
void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len > 0) {
        if (len >= (int)sizeof(buffer))
            len = sizeof(buffer) - 1;
        for (int i = 0; i < len; i++) {
            DL_UART_transmitDataBlocking(pSerial->uart, (uint8_t)buffer[i]);
        }
    }
}

// ============== 原始字节发送（借鉴待移植库）==============
void Serial_SendBytes(Serial_Typedef *pSerial, uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        DL_UART_transmitDataBlocking(pSerial->uart, buf[i]);
    }
}

// ============== HEX帧发送（借鉴待移植库，适配校验和格式）==============
// 帧格式：[0xFF][0xAA][count][D1_H][D1_L][D1_CK]...[DN_H][DN_L][DN_CK][0x55][0xFE]
void Serial_Send_HEX_Package(Serial_Typedef *pSerial, int16_t *data, uint8_t count)
{
    if (count > Serial_RX_MAX_WORDS) return;

    // 栈缓冲区：帧头(2) + LEN(1) + 数据(count*3) + 帧尾(2)
    uint8_t txBuf[128];
    uint16_t idx = 0;

    txBuf[idx++] = Serial_Agreement_HEX.head1;   // 0xFF
    txBuf[idx++] = Serial_Agreement_HEX.head2;   // 0xAA
    txBuf[idx++] = count;                         // LEN = 字个数

    for (uint8_t i = 0; i < count; i++) {
        int16_t val = data[i];
        uint8_t dh = (uint8_t)((val >> 8) & 0xFF);
        uint8_t dl = (uint8_t)(val & 0xFF);
        txBuf[idx++] = dh;
        txBuf[idx++] = dl;
        txBuf[idx++] = dh ^ dl;                   // 校验码 = DH XOR DL
    }

    txBuf[idx++] = Serial_Agreement_HEX.end1;    // 0x55
    txBuf[idx++] = Serial_Agreement_HEX.end2;    // 0xFE

    Serial_SendBytes(pSerial, txBuf, idx);
}

// ============== 高位低位合并（借鉴Car1）==============
static uint16_t Merge_2Bytes(uint8_t high, uint8_t low)
{
    return ((uint16_t)high << 8) | low;
}

// ====================================================================
// 接收状态机（借鉴Car1的 Status 0/1/2 模式）
//
// Status 0: 空闲，等待帧头（0xFF→HEX, '@'→ABC）
// Status 1: 正在接收HEX帧
// Status 2: 正在接收ABC帧
//
// 每次接收一个字节（pSerial->rx_temp），处理后返回状态标志
// ====================================================================
Serial_RX_FLAG_Typedef Serial_Rx_State_Check(Serial_Typedef *pSerial)
{
    uint8_t rxData = pSerial->rx_temp;

    // ===== Status 0: 空闲状态，等待帧头 =====
    if (pSerial->Status == 0)
    {
        pSerial->rxLen = 0;   // 复位缓冲区指针

        // 等待帧头-HEX模式
        if (rxData == Serial_Agreement_HEX.head1)
        {
            pSerial->rxBuf[pSerial->rxLen++] = rxData;
            pSerial->Status = 1;            // 进入HEX接收
            pSerial->frame_start_tick = Timer_Get_Ticks();
            return RX_BUSY;
        }
        // 等待帧头-ABC模式
        else if (rxData == Serial_Agreement_ABC.head)
        {
            pSerial->rxBuf[pSerial->rxLen++] = rxData;
            pSerial->Status = 2;            // 进入ABC接收
            return RX_BUSY;
        }
        else
        {
            return RX_WAIT;   // 非帧头，继续等待
        }
    }
    // ===== Status 1: 正在接收HEX帧 =====
    else if (pSerial->Status == 1)
    {
        pSerial->rxBuf[pSerial->rxLen++] = rxData;

        // 验证第二字节必须为 0xAA（防止 0xFF + 噪声 误触发）
        if (pSerial->rxLen == 2)
        {
            if (rxData != Serial_Agreement_HEX.head2)
            {
                pSerial->Status = 0;          // 假帧头，退回Idle
                return RX_BUSY;
            }
            return RX_BUSY;
        }

        // rxLen >= 5 时可读取 LEN 并判断帧完整性
        if (pSerial->rxLen >= 5)
        {
            uint8_t LEN = pSerial->rxBuf[2];
            if (LEN > Serial_RX_MAX_WORDS)
            {
                pSerial->Status = 0;          // 非法LEN，退回
                pSerial->HEX_Data.err = Serial_Err_HEX_Len_OverFlow;
                return RX_BUSY;
            }
            uint16_t needed = 2 + 1 + LEN * 3 + 2;   // 完整帧所需字节数

            // 超出预期字节数（噪声数据）
            if (pSerial->rxLen > needed)
            {
                pSerial->Status = 0;
                return RX_BUSY;
            }

            // 帧接收完成
            if (pSerial->rxLen == needed)
            {
                pSerial->Status = 0;
                return RX_OK_HEX;
            }
        }

        // 超时保护（借鉴Car1的溢出保护 + Template的超时机制）
        if (pSerial->rxLen > 5)
        {
            uint32_t elapsed = Timer_Get_Ticks() - pSerial->frame_start_tick;
            if (elapsed > HEX_FRAME_TIMEOUT_TICKS)
            {
                pSerial->Status = 0;
                pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
                return RX_Error_Tail_HEX;
            }
        }

        // 字节数上限保护（借鉴Car1的 Wait_Tail_MAX 模式）
        if (pSerial->rxLen > (uint16_t)(Serial_Wait_Tail_MAX + 5))
        {
            pSerial->Status = 0;
            memset(pSerial->rxBuf, 0, sizeof(pSerial->rxBuf));
            pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
            return RX_Error_Tail_HEX;
        }

        return RX_BUSY;
    }
    // ===== Status 2: 正在接收ABC帧 =====
    else if (pSerial->Status == 2)
    {
        pSerial->rxBuf[pSerial->rxLen++] = rxData;

        // 帧尾检测：最后两字节为 "$#"
        if (pSerial->rxLen >= 3 &&
            pSerial->rxBuf[pSerial->rxLen - 2] == Serial_Agreement_ABC.end1 &&
            pSerial->rxBuf[pSerial->rxLen - 1] == Serial_Agreement_ABC.end2)
        {
            pSerial->Status = 0;
            return RX_OK_ABC;
        }

        // ABC帧长度上限（借鉴Car1溢出保护）
        if (pSerial->rxLen > (uint16_t)(Serial_Wait_Tail_MAX + 3))
        {
            pSerial->Status = 0;
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
            return RX_Error_Tail_ABC;
        }

        return RX_BUSY;
    }
    return RX_BUSY;
}

// ====================================================================
// HEX数据检测+处理（借鉴Car1的 Serial_Data_Check_HEX）
// ====================================================================
static void Serial_Data_Check_HEX(Serial_Typedef *pSerial)
{
    uint8_t LEN = pSerial->rxBuf[2];

    // 1. 检测帧头合规性
    if (pSerial->rxBuf[0] != Serial_Agreement_HEX.head1 ||
        pSerial->rxBuf[1] != Serial_Agreement_HEX.head2)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Head;
        memset(pSerial->HEX_Data.data, 0, sizeof(pSerial->HEX_Data.data));
        pSerial->HEX_Data.frame_valid = false;
        return;
    }

    if (LEN > Serial_RX_MAX_WORDS)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Len_OverFlow;
        return;
    }

    uint16_t tail_idx = 3 + LEN * 3;

    // 2. 借助数据长度检测帧尾合规性
    if (pSerial->rxBuf[tail_idx]     != Serial_Agreement_HEX.end1 ||
        pSerial->rxBuf[tail_idx + 1] != Serial_Agreement_HEX.end2)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
        memset(pSerial->HEX_Data.data, 0, sizeof(pSerial->HEX_Data.data));
        pSerial->HEX_Data.frame_valid = false;
        return;
    }

    // 3. 逐字解析（校验失败清零，避免残留旧数据）
    for (uint8_t i = 0; i < LEN; i++)
    {
        uint8_t dh = pSerial->rxBuf[3 + i * 3];
        uint8_t dl = pSerial->rxBuf[3 + i * 3 + 1];
        uint8_t ck = pSerial->rxBuf[3 + i * 3 + 2];
        if ((dh ^ dl) == ck)
        {
            pSerial->HEX_Data.data[i] = (int16_t)((dh << 8) | dl);
        }
        else
        {
            pSerial->HEX_Data.data[i] = 0;   // 校验失败清零
        }
    }

    pSerial->HEX_Data.len = LEN;
    pSerial->HEX_Data.frame_valid = true;
    Timer_Counter_Func() ;
    pSerial->HEX_Data.err = Serial_Err_None;  // 成功解析后清除旧错误
}

// ====================================================================
// ABC数据检测+处理（借鉴Car1的 Serial_Data_Check_ABC）
// ====================================================================
static void Serial_Data_Check_ABC(Serial_Typedef *pSerial)
{
    // 1. 检测数据包帧头是否错误
    if (pSerial->rxBuf[0] != Serial_Agreement_ABC.head)
    {
        pSerial->ABC_Data.err = Serial_Err_ABC_Head;
        return;
    }

    // 2. 边处理数据边检测帧尾，从1开始（跳过帧头）
    int i = 0;
    for (i = 1; pSerial->rxBuf[i + 1] != Serial_Agreement_ABC.end1; i++)
    {
        pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];
        // 检测是否溢出
        if (i > Serial_Wait_Tail_MAX)
            break;
    }
    // 补1位（被迫离开for，少1位）
    pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];

    // 情况1: 退出for是因为检测到了帧尾
    if (pSerial->rxBuf[i + 1] == Serial_Agreement_ABC.end1)
    {
        // 检测第2个帧尾
        if (pSerial->rxBuf[i + 2] != Serial_Agreement_ABC.end2)
        {
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
            memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
                   sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
            return;
        }
        else
        {
            pSerial->ABC_Data.Serial_New_Package_ABC[i] = '\0';   // 加结尾符号
            pSerial->ABC_Data.err = Serial_Err_None;
            pSerial->ABC_Data.Serial_New_Package_Flag = 1;
        }
    }
    // 情况2: 退出for是因为溢出，第1个帧尾没检测到
    else
    {
        pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
        memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
               sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
    }
}

// ====================================================================
// 各UART中断句柄（借鉴Car1：读rx_temp → State_Check → Data_Check）
// 新增 RX_ERROR 处理，防止噪声导致的FIFO锁死
// ====================================================================

void UART_0_INST_IRQHandler(void)
{
    Serial_RX_FLAG_Typedef Rx_State;
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            // ★ 必须读出数据，否则FIFO存不下，再也进不去中断（借鉴Car1）
            Serial1.rx_temp = DL_UART_Main_receiveData(UART_0_INST);

            Rx_State = Serial_Rx_State_Check(&Serial1);

            if (Rx_State == RX_OK_HEX)
                Serial_Data_Check_HEX(&Serial1);
            else if (Rx_State == RX_OK_ABC)
                Serial_Data_Check_ABC(&Serial1);
            break;

        // ★ 新增：RX错误中断处理，清除错误防止FIFO锁死
        case DL_UART_MAIN_IIDX_OVERRUN_ERROR:
        case DL_UART_MAIN_IIDX_BREAK_ERROR:
        case DL_UART_MAIN_IIDX_PARITY_ERROR:
        case DL_UART_MAIN_IIDX_FRAMING_ERROR:
        case DL_UART_MAIN_IIDX_NOISE_ERROR:
            DL_UART_Main_receiveData(UART_0_INST);   // 读出错误字节并丢弃
            break;

        default:
            break;
    }
}

void UART_1_INST_IRQHandler(void)
{
    Serial_RX_FLAG_Typedef Rx_State;
    switch (DL_UART_Main_getPendingInterrupt(UART_1_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            
            Serial2.rx_temp = DL_UART_Main_receiveData(UART_1_INST);

            Rx_State = Serial_Rx_State_Check(&Serial2);

            if (Rx_State == RX_OK_HEX)
                Serial_Data_Check_HEX(&Serial2);
            else if (Rx_State == RX_OK_ABC)
                Serial_Data_Check_ABC(&Serial2);
            break;

        case DL_UART_MAIN_IIDX_OVERRUN_ERROR:
        case DL_UART_MAIN_IIDX_BREAK_ERROR:
        case DL_UART_MAIN_IIDX_PARITY_ERROR:
        case DL_UART_MAIN_IIDX_FRAMING_ERROR:
        case DL_UART_MAIN_IIDX_NOISE_ERROR:
            DL_UART_Main_receiveData(UART_1_INST);
            break;

        default:
            break;
    }
}

void UART_2_INST_IRQHandler(void)
{
    Serial_RX_FLAG_Typedef Rx_State;
    switch (DL_UART_Main_getPendingInterrupt(UART_2_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            Serial3.rx_temp = DL_UART_Main_receiveData(UART_2_INST);

            Rx_State = Serial_Rx_State_Check(&Serial3);

            if (Rx_State == RX_OK_HEX)
                Serial_Data_Check_HEX(&Serial3);
            else if (Rx_State == RX_OK_ABC)
                Serial_Data_Check_ABC(&Serial3);
            break;

        case DL_UART_MAIN_IIDX_OVERRUN_ERROR:
        case DL_UART_MAIN_IIDX_BREAK_ERROR:
        case DL_UART_MAIN_IIDX_PARITY_ERROR:
        case DL_UART_MAIN_IIDX_FRAMING_ERROR:
        case DL_UART_MAIN_IIDX_NOISE_ERROR:
            DL_UART_Main_receiveData(UART_2_INST);
            break;

        default:
            break;
    }
}

#ifdef Serial4_Enable
void UART_4_INST_IRQHandler(void)
{
    Serial_RX_FLAG_Typedef Rx_State;
    switch (DL_UART_Main_getPendingInterrupt(UART_4_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            Serial4.rx_temp = DL_UART_Main_receiveData(UART_4_INST);

            Rx_State = Serial_Rx_State_Check(&Serial4);

            if (Rx_State == RX_OK_HEX)
                Serial_Data_Check_HEX(&Serial4);
            else if (Rx_State == RX_OK_ABC)
                Serial_Data_Check_ABC(&Serial4);
            break;

        case DL_UART_MAIN_IIDX_OVERRUN_ERROR:
        case DL_UART_MAIN_IIDX_BREAK_ERROR:
        case DL_UART_MAIN_IIDX_PARITY_ERROR:
        case DL_UART_MAIN_IIDX_FRAMING_ERROR:
        case DL_UART_MAIN_IIDX_NOISE_ERROR:
            DL_UART_Main_receiveData(UART_4_INST);
            break;

        default:
            break;
    }
}
#endif

// ====================================================================
// 以下协议解析/获取/设置函数 — 保持原有API不变
// ====================================================================

// ========== HEX协议：获取数据 ==========
int16_t Serial_GetHexData(Serial_Typedef *pSerial, uint8_t index)
{
    if (index >= Serial_RX_MAX_WORDS) return 0;
    return pSerial->HEX_Data.data[index];
}

uint8_t Serial_GetHexLen(Serial_Typedef *pSerial)
{
    return pSerial->HEX_Data.len;
}

uint8_t Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial)
{
    if (pSerial->HEX_Data.frame_valid == true) {
        pSerial->HEX_Data.frame_valid = false;
        return 1;
    }
    return 0;
}

int Serial_GetError_HEX(Serial_Typedef *pSerial)
{
    return (int)pSerial->HEX_Data.err;
}

// ========== ABC协议：获取新包标志 ==========
uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial)
{
    if (pSerial->ABC_Data.Serial_New_Package_Flag == 1) {
        pSerial->ABC_Data.Serial_New_Package_Flag = 0;
        return 1;
    }
    return 0;
}

int Serial_GetError_ABC(Serial_Typedef *pSerial)
{
    return (int)pSerial->ABC_Data.err;
}

// ========== ABC协议：设置浮点数 ==========
bool Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data)
{
    if (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL) {
        sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd, Data);
        return true;
    }
    return false;
}

// ========== ABC协议：设置整数 ==========
bool Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data)
{
    if (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL) {
        sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd, Data);
        return true;
    }
    return false;
}

// ========== ABC协议：子串匹配（strstr）==========
bool Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord)
{
    if (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL)
        return true;
    return false;
}

// ========== ABC协议：精确匹配（strcmp，借鉴待移植库）==========
bool Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd)
{
    return (strcmp(pSerial->ABC_Data.Serial_New_Package_ABC, cmd) == 0);
}
