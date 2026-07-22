#include "Serial_porting.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// ============== 全局实例 ==============
Serial_Typedef Serial1;
#ifdef Serial2_Enable
Serial_Typedef Serial2;
#endif
#ifdef Serial4_Enable
Serial_Typedef Serial4;
#endif

// ============== 内部函数声明 ==============
static void Serial_Parse_HEX(Serial_Typedef *pSerial, uint16_t Size);
static void Serial_Parse_ABC(Serial_Typedef *pSerial);

// ============== 初始化 ==============
void Serial_Init(void)
{
    // ----- Serial1 (UART_0, USB调试, 115200) -----
    Serial1.uart  = UART_0_INST;
    Serial1.IRQN  = UART_0_INST_INT_IRQN;
    Serial1.rxLen = 0;
    Serial1.rxOverflow = false;
    Serial1.rxState = Serial_Rx_Idle;
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
    Serial2.rxState = Serial_Rx_Idle;
    memset(Serial2.rxBuf, 0, sizeof(Serial2.rxBuf));
    memset(&Serial2.ABC_Data, 0, sizeof(Serial2.ABC_Data));
    memset(&Serial2.HEX_Data, 0, sizeof(Serial2.HEX_Data));
    NVIC_ClearPendingIRQ(Serial2.IRQN);
    NVIC_EnableIRQ(Serial2.IRQN);
#endif

#ifdef Serial4_Enable
    // ----- Serial4 (UART_4, 串口屏, 115200) -----
    Serial4.uart  = UART_4_INST;
    Serial4.IRQN  = UART_4_INST_INT_IRQN;
    Serial4.rxLen = 0;
    Serial4.rxOverflow = false;
    Serial4.rxState = Serial_Rx_Idle;
    memset(Serial4.rxBuf, 0, sizeof(Serial4.rxBuf));
    memset(&Serial4.ABC_Data, 0, sizeof(Serial4.ABC_Data));
    memset(&Serial4.HEX_Data, 0, sizeof(Serial4.HEX_Data));
    NVIC_ClearPendingIRQ(Serial4.IRQN);
    NVIC_EnableIRQ(Serial4.IRQN);
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
    // count≤40 时：5 + 120 = 125 字节，128字节缓冲区足够
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

// ====================================================================
// 各UART中断句柄 — 全部路由到共享的Serial_RxISR
// ====================================================================

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            Serial_RxISR(&Serial1);
            break;
        default:
            break;
    }
}

void UART_1_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_1_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            Serial_RxISR(&Serial2);
            break;
        default:
            break;
    }
}

#ifdef Serial4_Enable
void UART_4_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_4_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            Serial_RxISR(&Serial4);
            break;
        default:
            break;
    }
}
#endif

// ====================================================================
// Serial_RxISR — 显式状态机驱动（借鉴待移植库 Status 0/1/2 模式）
//
// 状态转换：
//   Idle → HEX: 收到 0xFF（字节2验证 0xAA，否则退回Idle）
//   Idle → ABC: 收到 '@'
//   HEX  → Idle: 帧完成/帧错误/超时
//   ABC  → Idle: 帧完成/帧超长
//
// 优势：
//   1. O(1) 状态判断（整数比较），无需每字节检查 rxBuf[0]
//   2. 自然错误恢复 — 任何完成/错误都回到 Idle
//   3. 不会因噪声锁死在 HEX 等待模式
// ====================================================================
void Serial_RxISR(Serial_Typedef *pSerial)
{
    uint8_t byte = DL_UART_Main_receiveData(pSerial->uart);

    // 缓冲区溢出保护
    if (pSerial->rxLen >= Serial_RX_BUF_SIZE) {
        pSerial->rxLen = 0;
        pSerial->rxState = Serial_Rx_Idle;
        pSerial->rxOverflow = true;
        return;
    }

    switch (pSerial->rxState) {

    // ===== 空闲状态：等待帧头 =====
    case Serial_Rx_Idle:
        pSerial->rxLen = 0;   // 复位缓冲区指针

        if (byte == Serial_Agreement_HEX.head1) {        // 0xFF
            pSerial->rxBuf[pSerial->rxLen++] = byte;
            pSerial->rxState = Serial_Rx_HEX;
            pSerial->frame_start_tick = Timer_Get_Ticks();
        }
        else if (byte == Serial_Agreement_ABC.head) {    // '@'
            pSerial->rxBuf[pSerial->rxLen++] = byte;
            pSerial->rxState = Serial_Rx_ABC;
        }
        // 其他字节：非帧头数据，静默丢弃，保持Idle
        return;

    // ===== HEX帧接收 =====
    case Serial_Rx_HEX:
        pSerial->rxBuf[pSerial->rxLen++] = byte;

        // 验证第二字节必须为 0xAA（防止 0xFF + 噪声 误触发）
        if (pSerial->rxLen == 2) {
            if (byte != Serial_Agreement_HEX.head2) {
                pSerial->rxState = Serial_Rx_Idle;       // 假帧头，退回Idle
            }
            return;
        }

        // rxLen >= 5 时可读取 LEN 并判断帧完整性
        if (pSerial->rxLen >= 5) {
            uint8_t LEN = pSerial->rxBuf[2];
            if (LEN > Serial_RX_MAX_WORDS) {
                pSerial->rxState = Serial_Rx_Idle;       // 非法LEN，退回
                return;
            }
            uint16_t needed = 2 + 1 + LEN * 3 + 2;       // 完整帧所需字节数
            if (pSerial->rxLen > needed) {
                pSerial->rxState = Serial_Rx_Idle;       // 超出预期，非真实帧
                return;
            }
            if (pSerial->rxLen == needed) {
                // 帧尾验证
                uint16_t tail_idx = 3 + LEN * 3;
                if (pSerial->rxBuf[tail_idx]     == Serial_Agreement_HEX.end1 &&
                    pSerial->rxBuf[tail_idx + 1] == Serial_Agreement_HEX.end2) {
                    Serial_Parse_HEX(pSerial, pSerial->rxLen);
                }
                pSerial->rxState = Serial_Rx_Idle;       // 无论成败都回Idle
                return;
            }
        }

        // 超时保护（帧接收中，但长时间未完成）
        if (pSerial->rxLen > 5) {
            uint32_t elapsed = Timer_Get_Ticks() - pSerial->frame_start_tick;
            if (elapsed > HEX_FRAME_TIMEOUT_TICKS) {
                pSerial->rxState = Serial_Rx_Idle;
                return;
            }
        }

        // 字节数上限保护（借鉴待移植库 Serial_Wait_Tail_MAX 模式）
        if (pSerial->rxLen > (uint16_t)(Serial_Wait_Tail_MAX + 5)) {
            pSerial->rxState = Serial_Rx_Idle;
        }
        return;

    // ===== ABC帧接收 =====
    case Serial_Rx_ABC:
        pSerial->rxBuf[pSerial->rxLen++] = byte;

        // 帧尾检测：最后两字节为 "$#"
        if (pSerial->rxLen >= 3 &&
            pSerial->rxBuf[pSerial->rxLen - 2] == Serial_Agreement_ABC.end1 &&
            pSerial->rxBuf[pSerial->rxLen - 1] == Serial_Agreement_ABC.end2) {
            Serial_Parse_ABC(pSerial);
            pSerial->rxState = Serial_Rx_Idle;
            return;
        }

        // ABC帧长度上限（帧头@ + 数据 + 帧尾$# = Wait_Tail_MAX + 3）
        if (pSerial->rxLen > (uint16_t)(Serial_Wait_Tail_MAX + 3)) {
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
            pSerial->rxState = Serial_Rx_Idle;
        }
        return;
    }
}

// ====================================================================
// 以下协议解析/获取/设置函数 — 完全复用F407代码，纯struct操作
// ====================================================================

// ========== HEX协议：解析 ==========
static void Serial_Parse_HEX(Serial_Typedef *pSerial, uint16_t Size)
{
    uint8_t LEN = pSerial->rxBuf[2];

    if (LEN > Serial_RX_MAX_WORDS) {
        pSerial->HEX_Data.err = Serial_Err_HEX_Len_OverFlow;
        return;
    }

    uint16_t needed_len = 2 + 1 + LEN * 3 + 2;
    if (Size < needed_len) return;

    uint16_t tail_idx = 3 + LEN * 3;
    if (pSerial->rxBuf[tail_idx] != Serial_Agreement_HEX.end1 ||
        pSerial->rxBuf[tail_idx + 1] != Serial_Agreement_HEX.end2) {
        pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
        return;
    }

    // 逐字解析（校验失败清零，避免残留旧数据）
    for (uint8_t i = 0; i < LEN; i++) {
        uint8_t dh = pSerial->rxBuf[3 + i * 3];
        uint8_t dl = pSerial->rxBuf[3 + i * 3 + 1];
        uint8_t ck = pSerial->rxBuf[3 + i * 3 + 2];
        if ((dh ^ dl) == ck) {
            pSerial->HEX_Data.data[i] = (int16_t)((dh << 8) | dl);
        } else {
            pSerial->HEX_Data.data[i] = 0;   // 校验失败清零
        }
    }

    pSerial->HEX_Data.len = LEN;
    pSerial->HEX_Data.frame_valid = true;
    pSerial->HEX_Data.err = Serial_Err_None;  // 成功解析后清除旧错误
}

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

// ========== ABC协议：解析 ==========
static void Serial_Parse_ABC(Serial_Typedef *pSerial)
{
    if (pSerial->rxBuf[0] != Serial_Agreement_ABC.head) {
        pSerial->ABC_Data.err = Serial_Err_ABC_Head;
        return;
    }

    int i = 0;
    for (i = 1; pSerial->rxBuf[i + 1] != Serial_Agreement_ABC.end1; i++) {
        pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];
        if (i > Serial_Wait_Tail_MAX) break;
    }
    pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];

    if (pSerial->rxBuf[i + 1] == Serial_Agreement_ABC.end1) {
        if (pSerial->rxBuf[i + 2] != Serial_Agreement_ABC.end2) {
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
            memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
                   sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
            return;
        } else {
            pSerial->ABC_Data.Serial_New_Package_ABC[i] = '\0';
            pSerial->ABC_Data.err = Serial_Err_None;
            pSerial->ABC_Data.Serial_New_Package_Flag = 1;
        }
    } else {
        pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
        memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
               sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
    }
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
