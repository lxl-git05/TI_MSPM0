#include "Serial_porting.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include "Timer_Counter.h"

// ============== 全局实例 ==============
#ifdef Serial1_Enable
Serial_Typedef Serial1;
#endif
#ifdef Serial2_Enable
Serial_Typedef Serial2;
#endif
#ifdef Serial3_Enable
Serial_Typedef Serial3;
#endif
#ifdef Serial4_Enable
Serial_Typedef Serial4;
#endif

// ============== 状态机枚举（内部使用）==============
typedef enum {
    RX_OK_HEX         = 0x00U,
    RX_OK_ABC         = 0x01U,
    RX_BUSY           = 0x02U,
    RX_WAIT           = 0x03U,
    RX_Error_Tail_HEX = 0x06U,
    RX_Error_Tail_ABC = 0x07U,
} Serial_RX_FLAG_Typedef;

// ============== 内部函数声明 ==============
static void Serial_Initial(Serial_Typedef *pSerial, UART_Regs *uart_INST, DMA_Regs *dma,
                           uint8_t channelNum, uint8_t uart_int_IRQN,
                           DMA_Regs *tx_dma, uint8_t tx_channelNum);
static Serial_RX_FLAG_Typedef Serial_Rx_State_Check(Serial_Typedef *pSerial);
static void Serial_Parse_HEX(Serial_Typedef *pSerial);
static void Serial_Parse_ABC(Serial_Typedef *pSerial);
static void Serial_Rx_ProcessByte(Serial_Typedef *pSerial);
static void Serial_RxISR_Dispatch(Serial_Typedef *pSerial);
static void uart_send_char(Serial_Typedef *pSerial, char ch);
static void Serial_DMA_Kick(Serial_Typedef *pSerial);
static void Serial_WriteBuf(Serial_Typedef *pSerial, uint8_t *data, uint16_t len);

// ============== 初始化 ==============

static void Serial_Initial(Serial_Typedef *pSerial, UART_Regs *uart_INST, DMA_Regs *dma,
                           uint8_t channelNum, uint8_t uart_int_IRQN,
                           DMA_Regs *tx_dma, uint8_t tx_channelNum)
{
    // 数据链清零
    pSerial->rxCnt = 0;
    pSerial->rx_temp = 0;
    pSerial->Status = 0;
    memset(pSerial->rxBuf, 0, Serial_RX_BUF_SIZE);
    memset(&pSerial->HEX_Data, 0, sizeof(pSerial->HEX_Data));
    memset(&pSerial->ABC_Data, 0, sizeof(pSerial->ABC_Data));

#ifdef Serial_Debug
    pSerial->dbg_rx_bytes    = 0;
    pSerial->dbg_rx_frames   = 0;
    pSerial->dbg_parse_ok    = 0;
    pSerial->dbg_frame_lost  = 0;
    pSerial->dbg_err_head    = 0;
    pSerial->dbg_err_tail    = 0;
    pSerial->dbg_err_overflow = 0;
    pSerial->dbg_err_hardware = 0;
#endif

    // 硬件参数
    pSerial->uart_INST = uart_INST;
    pSerial->uart_int_IRQN = uart_int_IRQN;
    pSerial->dma = dma;
    pSerial->channelNum = channelNum;
    pSerial->tx_dma = tx_dma;
    pSerial->tx_channelNum = tx_channelNum;

    // TX 初始化
    pSerial->txHead = 0;
    pSerial->txTail = 0;
    pSerial->dmaBusy = 0;
    pSerial->dmaLen = 0;

    if (dma != NULL)
    {
        // DMA 模式：配置 RX DMA
        DL_DMA_setSrcAddr(dma, channelNum, (uint32_t)(&uart_INST->RXDATA));
        DL_DMA_setDestAddr(dma, channelNum, (uint32_t)&pSerial->rx_temp);
        DL_DMA_setTransferSize(dma, channelNum, 1);
        DL_DMA_enableChannel(dma, channelNum);
        DL_UART_enableInterrupt(uart_INST, DL_UART_INTERRUPT_DMA_DONE_TX);

        // 额外启用错误中断，防止噪声导致 FIFO/DMA 锁死
        DL_UART_Main_enableInterrupt(uart_INST,
            DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
            DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
            DL_UART_MAIN_INTERRUPT_BREAK_ERROR);

        while (!DL_DMA_isChannelEnabled(dma, channelNum)) { __BKPT(0); }
    }
    else
    {
        // 中断模式：关闭 SysConfig 残留 DMA，开启 CPU 中断
        DL_UART_Main_disableDMAReceiveEvent(uart_INST, DL_UART_DMA_INTERRUPT_RX);
        DL_UART_Main_enableInterrupt(uart_INST,
            DL_UART_MAIN_INTERRUPT_RX |
            DL_UART_MAIN_INTERRUPT_RX_TIMEOUT_ERROR |
            DL_UART_MAIN_INTERRUPT_OVERRUN_ERROR |
            DL_UART_MAIN_INTERRUPT_FRAMING_ERROR |
            DL_UART_MAIN_INTERRUPT_BREAK_ERROR);
        NVIC_ClearPendingIRQ(uart_int_IRQN);
    }

    NVIC_EnableIRQ(uart_int_IRQN);
}

void Serial_Init(void)
{
    // 初始化协议常量
    Serial_Agreement_ABC_Init();
    Serial_Agreement_HEX_Init();

#ifdef Serial1_Enable
    Serial_Initial(&Serial1, UART_0_INST, NULL, 0, UART_0_INST_INT_IRQN,
                   NULL, 0);
#endif
#ifdef Serial2_Enable
    Serial_Initial(&Serial2, UART_1_INST, NULL, 0, UART_1_INST_INT_IRQN,
                   NULL, 0);
#endif
#ifdef Serial3_Enable
    Serial_Initial(&Serial3, UART_2_INST, NULL, 0, UART_2_INST_INT_IRQN,
                   NULL, 0);
#endif
#ifdef Serial4_Enable
    Serial_Initial(&Serial4, UART_3_INST, NULL, 0, UART_3_INST_INT_IRQN,
                   NULL, 0);
#endif
}

// ============== 状态机：逐字节 RX ==============

static Serial_RX_FLAG_Typedef Serial_Rx_State_Check(Serial_Typedef *pSerial)
{
    uint8_t rxData = pSerial->rx_temp;

    // 状态0：空闲，等待帧头
    if (pSerial->Status == 0)
    {
        pSerial->rxCnt = 0;

        if (rxData == Serial_Agreement_HEX.head1)
        {
            pSerial->rxBuf[pSerial->rxCnt++] = rxData;
            pSerial->Status = 1;            // → HEX 接收
            return RX_BUSY;
        }
        else if (rxData == Serial_Agreement_ABC.head)
        {
            pSerial->rxBuf[pSerial->rxCnt++] = rxData;
            pSerial->Status = 2;            // → ABC 接收
            return RX_BUSY;
        }
        return RX_WAIT;
    }

    // 状态1：HEX 帧接收中
    if (pSerial->Status == 1)
    {
        pSerial->rxBuf[pSerial->rxCnt++] = rxData;

        if (rxData == Serial_Agreement_HEX.end2)
        {
            pSerial->Status = 0;
            return RX_OK_HEX;
        }

        // 收到 LEN 字节（rxBuf[2]）后，按预期帧长做早期溢出检测
        if (pSerial->rxCnt >= 3)
        {
            uint8_t LEN = pSerial->rxBuf[2];
            if ((uint16_t)LEN > Serial_RX_MAX_WORDS)
            {
                pSerial->Status = 0;
                pSerial->HEX_Data.err = Serial_Err_HEX_Len_OverFlow;
#ifdef Serial_Debug
                pSerial->dbg_err_overflow++;
#endif
                return RX_Error_Tail_HEX;
            }
            uint16_t expected = 5 + (uint16_t)LEN * 3;  // head(2)+LEN(1)+data(LEN×3)+tail(2)
            if (pSerial->rxCnt > expected)
            {
                pSerial->Status = 0;
                pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
#ifdef Serial_Debug
                pSerial->dbg_err_overflow++;
#endif
                return RX_Error_Tail_HEX;
            }
        }

        // 兜底：超最大缓冲区（LEN 不可用时的最后防线）
        if (pSerial->rxCnt >= Serial_RX_BUF_SIZE)
        {
            pSerial->Status = 0;
            pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
#ifdef Serial_Debug
            pSerial->dbg_err_overflow++;
#endif
            return RX_Error_Tail_HEX;
        }
        return RX_BUSY;
    }

    // 状态2：ABC 帧接收中
    if (pSerial->Status == 2)
    {
        pSerial->rxBuf[pSerial->rxCnt++] = rxData;

        if (rxData == Serial_Agreement_ABC.end2)
        {
            pSerial->Status = 0;
            return RX_OK_ABC;
        }
        if (pSerial->rxCnt >= Serial_Wait_Tail_MAX + 3)
        {
            pSerial->Status = 0;
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
#ifdef Serial_Debug
            pSerial->dbg_err_overflow++;
#endif
            return RX_Error_Tail_ABC;
        }
        return RX_BUSY;
    }

    return RX_BUSY;
}

// ============== HEX 协议解析（F407 格式，带 XOR 校验）==============
// 帧格式：[0xFF][0xAA][LEN][D1_H][D1_L][D1_CK]...[DN_H][DN_L][DN_CK][0x55][0xFE]

static void Serial_Parse_HEX(Serial_Typedef *pSerial)
{
    uint16_t Size = pSerial->rxCnt;

    // 帧头验证
    if (pSerial->rxBuf[0] != Serial_Agreement_HEX.head1 ||
        pSerial->rxBuf[1] != Serial_Agreement_HEX.head2)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Head;
        pSerial->HEX_Data.frame_valid = false;
#ifdef Serial_Debug
        pSerial->dbg_err_head++;
#endif
        return;
    }

    uint8_t LEN = pSerial->rxBuf[2];

    // LEN 超限
    if ((uint16_t)LEN > Serial_RX_MAX_WORDS)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Len_OverFlow;
        pSerial->HEX_Data.frame_valid = false;
        return;
    }

    // 完整性检查：帧头2 + LEN1 + 数据(LEN×3) + 帧尾2
    uint16_t needed = 2 + 1 + LEN * 3 + 2;
    if (Size < needed)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
        pSerial->HEX_Data.frame_valid = false;
#ifdef Serial_Debug
        pSerial->dbg_err_tail++;
#endif
        return;
    }

    // 帧尾验证
    uint16_t tail_idx = 3 + LEN * 3;
    if (pSerial->rxBuf[tail_idx] != Serial_Agreement_HEX.end1 ||
        pSerial->rxBuf[tail_idx + 1] != Serial_Agreement_HEX.end2)
    {
        pSerial->HEX_Data.err = Serial_Err_HEX_Tail;
        pSerial->HEX_Data.frame_valid = false;
#ifdef Serial_Debug
        pSerial->dbg_err_tail++;
#endif
        return;
    }

    // 逐字解析（校验失败保留原值）
    for (uint8_t i = 0; i < LEN; i++)
    {
        uint8_t dh = pSerial->rxBuf[3 + i * 3];
        uint8_t dl = pSerial->rxBuf[3 + i * 3 + 1];
        uint8_t ck = pSerial->rxBuf[3 + i * 3 + 2];

        if ((dh ^ dl) == ck)
        {
            pSerial->HEX_Data.data[i] = (int16_t)((dh << 8) | dl);
        }
    }

    // 调试
    // Timer_Counter_Func() ;
    // Serial_printf(&Serial1, "%d,%d,%d,%d,%d\n",Serial2.HEX_Data.data[0],Serial2.HEX_Data.data[1],Serial2.HEX_Data.data[2],Serial2.HEX_Data.data[3],Serial2.HEX_Data.data[4]);
    // 检测丢帧：旧帧未被主循环消费就被新帧覆盖
#ifdef Serial_Debug
    if (pSerial->HEX_Data.frame_valid)
        pSerial->dbg_frame_lost++;
#endif
    pSerial->HEX_Data.len = LEN;
    pSerial->HEX_Data.frame_valid = true;
    pSerial->HEX_Data.err = Serial_Err_None;
#ifdef Serial_Debug
    pSerial->dbg_parse_ok++;            // 成功解析
#endif
}

// ============== ABC 协议解析 ==============

static void Serial_Parse_ABC(Serial_Typedef *pSerial)
{
    // 帧头验证
    if (pSerial->rxBuf[0] != Serial_Agreement_ABC.head)
    {
        pSerial->ABC_Data.err = Serial_Err_ABC_Head;
#ifdef Serial_Debug
        pSerial->dbg_err_head++;
#endif
        return;
    }

    // 边处理数据边检测帧尾，跳过帧头（i=1 开始）
    uint16_t i;
    for (i = 1; i < pSerial->rxCnt - 1; i++)
    {
        if (pSerial->rxBuf[i + 1] == Serial_Agreement_ABC.end1)
            break;
        pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];

        if (i > Serial_Wait_Tail_MAX)
            break;
    }
    pSerial->ABC_Data.Serial_New_Package_ABC[i - 1] = pSerial->rxBuf[i];

    // 检测帧尾1
    if (pSerial->rxBuf[i + 1] == Serial_Agreement_ABC.end1)
    {
        // 检测帧尾2
        if (i + 2 >= pSerial->rxCnt ||
            pSerial->rxBuf[i + 2] != Serial_Agreement_ABC.end2)
        {
            pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
            memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
                   sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
#ifdef Serial_Debug
            pSerial->dbg_err_tail++;
#endif
            return;
        }

        // 检测丢帧：旧帧未被主循环消费就被新帧覆盖
#ifdef Serial_Debug
        if (pSerial->ABC_Data.Serial_New_Package_Flag)
            pSerial->dbg_frame_lost++;
#endif
        pSerial->ABC_Data.Serial_New_Package_ABC[i] = '\0';
        pSerial->ABC_Data.err = Serial_Err_None;
        pSerial->ABC_Data.Serial_New_Package_Flag = 1;
#ifdef Serial_Debug
        pSerial->dbg_parse_ok++;            // 成功解析
#endif
    }
    else
    {
        pSerial->ABC_Data.err = Serial_Err_ABC_Tail;
        memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0,
               sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));
#ifdef Serial_Debug
        pSerial->dbg_err_tail++;
#endif
    }
}

// ============== HEX API ==============

uint8_t Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial)
{
    if (pSerial->HEX_Data.frame_valid)
    {
        pSerial->HEX_Data.frame_valid = false;
        return 1;
    }
    return 0;
}

int16_t Serial_GetHexData(Serial_Typedef *pSerial, uint8_t index)
{
    if ((uint16_t)index >= Serial_RX_MAX_WORDS)
        return 0;
    return pSerial->HEX_Data.data[index];
}

uint8_t Serial_GetHexLen(Serial_Typedef *pSerial)
{
    return pSerial->HEX_Data.len;
}

int Serial_GetError_HEX(Serial_Typedef *pSerial)
{
    return (int)pSerial->HEX_Data.err;
}

// ============== ABC API ==============

uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial)
{
    if (pSerial->ABC_Data.Serial_New_Package_Flag == 1)
    {
        pSerial->ABC_Data.Serial_New_Package_Flag = 0;
        return 1;
    }
    return 0;
}

int Serial_GetError_ABC(Serial_Typedef *pSerial)
{
    return (int)pSerial->ABC_Data.err;
}

bool Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data)
{
    if (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL)
    {
        sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd, Data);
        return true;
    }
    return false;
}

bool Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data)
{
    if (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL)
    {
        sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd, Data);
        return true;
    }
    return false;
}

bool Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord)
{
    return (strstr(pSerial->ABC_Data.Serial_New_Package_ABC, KeyWord) != NULL);
}

bool Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd)
{
    return (strcmp(pSerial->ABC_Data.Serial_New_Package_ABC, cmd) == 0);
}

// ============== 发送 ==============

static void uart_send_char(Serial_Typedef *pSerial, char ch)
{
    while (DL_UART_isBusy(pSerial->uart_INST));
    DL_UART_Main_transmitData(pSerial->uart_INST, ch);
}

void Serial_send_string(Serial_Typedef *pSerial, char *str)
{
    while (*str)
    {
        uart_send_char(pSerial, *str++);
    }
}

void Serial_SendBytes(Serial_Typedef *pSerial, uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uart_send_char(pSerial, buf[i]);
    }
}

void Serial_Send_HEX_Package(Serial_Typedef *pSerial, uint16_t *data, uint8_t count)
{
    uint8_t txBuf[3 + 256 * 3 + 2]; // 最大帧大小
    uint16_t idx = 0;

    txBuf[idx++] = Serial_Agreement_HEX.head1;
    txBuf[idx++] = Serial_Agreement_HEX.head2;
    txBuf[idx++] = count;                           // LEN = 字数

    for (uint8_t i = 0; i < count; i++)
    {
        uint8_t dh = (data[i] >> 8) & 0xFF;
        uint8_t dl = data[i] & 0xFF;
        txBuf[idx++] = dh;
        txBuf[idx++] = dl;
        txBuf[idx++] = dh ^ dl;                     // XOR 校验码
    }

    txBuf[idx++] = Serial_Agreement_HEX.end1;
    txBuf[idx++] = Serial_Agreement_HEX.end2;

    Serial_SendBytes(pSerial, txBuf, idx);
}

// ============== DMA TX（环形缓冲，有 DMA 时使用）==============

static void Serial_WriteBuf(Serial_Typedef *pSerial, uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uint16_t next = (pSerial->txHead + 1) % TX_BUF_SIZE;
        if (next == pSerial->txTail)
            return;                                 // 缓冲满，丢弃
        pSerial->txBuf[pSerial->txHead] = data[i];
        pSerial->txHead = next;
    }
}

static void Serial_DMA_Kick(Serial_Typedef *pSerial)
{
    if (pSerial->dmaBusy) return;
    if (pSerial->txHead == pSerial->txTail) return;

    pSerial->dmaBusy = 1;

    uint16_t len;
    if (pSerial->txHead > pSerial->txTail)
        len = pSerial->txHead - pSerial->txTail;
    else
        len = TX_BUF_SIZE - pSerial->txTail;

    pSerial->dmaLen = len;

    DL_DMA_setSrcAddr(pSerial->tx_dma, pSerial->tx_channelNum,
                      (uint32_t)&pSerial->txBuf[pSerial->txTail]);
    DL_DMA_setDestAddr(pSerial->tx_dma, pSerial->tx_channelNum,
                       (uint32_t)&pSerial->uart_INST->TXDATA);
    DL_DMA_setTransferSize(pSerial->tx_dma, pSerial->tx_channelNum, len);
    DL_DMA_enableChannel(pSerial->tx_dma, pSerial->tx_channelNum);
}

void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...)
{
    if (pSerial->dma == NULL)
    {
        // 无 DMA 时回退到轮询
        char tempBuf[256];
        va_list args;
        va_start(args, fmt);
        int len = vsnprintf(tempBuf, sizeof(tempBuf), fmt, args);
        va_end(args);
        if (len > 0) Serial_send_string(pSerial, tempBuf);
        return;
    }

    char tempBuf[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(tempBuf, sizeof(tempBuf), fmt, args);
    va_end(args);

    if (len <= 0) return;
    if (len > (int)sizeof(tempBuf))
        len = sizeof(tempBuf);

    Serial_WriteBuf(pSerial, (uint8_t *)tempBuf, len);
    Serial_DMA_Kick(pSerial);
}

// ============== 统一收字节处理（DMA/中断共用）==============

static void Serial_Rx_ProcessByte(Serial_Typedef *pSerial)
{
    Serial_RX_FLAG_Typedef state = Serial_Rx_State_Check(pSerial);
    if (state == RX_OK_HEX)
    {
#ifdef Serial_Debug
        pSerial->dbg_rx_frames++;       // 状态机检测到完整HEX帧
#endif
        Serial_Parse_HEX(pSerial);
    }
    else if (state == RX_OK_ABC)
    {
#ifdef Serial_Debug
        pSerial->dbg_rx_frames++;       // 状态机检测到完整ABC帧
#endif
        Serial_Parse_ABC(pSerial);
    }
}

// ============== ISR 统一入口（所有 UART 共用）==============

static void Serial_RxISR_Dispatch(Serial_Typedef *pSerial)
{
    switch (DL_UART_Main_getPendingInterrupt(pSerial->uart_INST))
    {
        // ---- 接收完成：DMA 模式（数据已由 DMA 自动搬运到 rx_temp）----
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
#ifdef Serial_Debug
            pSerial->dbg_rx_bytes++;
#endif
            Serial_Rx_ProcessByte(pSerial);
            break;

        // ---- 接收完成：中断模式（手动读取 RX FIFO）----
        case DL_UART_MAIN_IIDX_RX:
            pSerial->rx_temp = DL_UART_Main_receiveData(pSerial->uart_INST);
#ifdef Serial_Debug
            pSerial->dbg_rx_bytes++;
#endif
            Serial_Rx_ProcessByte(pSerial);
            break;

        // ---- 发送完成：DMA 环形缓冲推进 ----
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            pSerial->txTail = (pSerial->txTail + pSerial->dmaLen) % TX_BUF_SIZE;
            pSerial->dmaBusy = 0;
            if (pSerial->txHead != pSerial->txTail)
                Serial_DMA_Kick(pSerial);
            break;

        // ---- 空闲超时：清空 FIFO 中残留字节 ----
        case DL_UART_MAIN_IIDX_RX_TIMEOUT_ERROR:
            while (!DL_UART_Main_isRXFIFOEmpty(pSerial->uart_INST))
                DL_UART_Main_receiveData(pSerial->uart_INST);
            break;

        // ---- 硬件错误中断：读出错误字节，防止 FIFO 锁死 ----
        case DL_UART_MAIN_IIDX_OVERRUN_ERROR:
        case DL_UART_MAIN_IIDX_BREAK_ERROR:
        case DL_UART_MAIN_IIDX_PARITY_ERROR:
        case DL_UART_MAIN_IIDX_FRAMING_ERROR:
        case DL_UART_MAIN_IIDX_NOISE_ERROR:
            DL_UART_Main_receiveData(pSerial->uart_INST);
#ifdef Serial_Debug
            pSerial->dbg_err_hardware++;
#endif
            break;

        default: break;
    }
}

// ============== 各 UART 中断向量入口 ==============

void UART_0_INST_IRQHandler(void)
{
#ifdef Serial1_Enable
    Serial_RxISR_Dispatch(&Serial1);
#endif
}

void UART_1_INST_IRQHandler(void)
{
#ifdef Serial2_Enable
    Serial_RxISR_Dispatch(&Serial2);
#endif
}

void UART_2_INST_IRQHandler(void)
{
#ifdef Serial3_Enable
    Serial_RxISR_Dispatch(&Serial3);
#endif
}

void UART_3_INST_IRQHandler(void)
{
#ifdef Serial4_Enable
    Serial_RxISR_Dispatch(&Serial4);
#endif
}

// ============== 调试打印 ==============
#ifdef Serial_Debug
void Serial_PrintDebug(Serial_Typedef *pSerial)
{
    Serial_printf(&Serial1,
        "=== Serial Debug ===\r\n"
        "  RX Bytes:    %lu\r\n"
        "  Frames Det:  %lu\r\n"
        "  Parse OK:    %lu\r\n"
        "  Lost:        %lu\r\n"
        "  Err Head:    %lu\r\n"
        "  Err Tail:    %lu\r\n"
        "  Overflow:    %lu\r\n"
        "  HW Errors:   %lu\r\n"
        "==================\r\n",
        pSerial->dbg_rx_bytes,
        pSerial->dbg_rx_frames,
        pSerial->dbg_parse_ok,
        pSerial->dbg_frame_lost,
        pSerial->dbg_err_head,
        pSerial->dbg_err_tail,
        pSerial->dbg_err_overflow,
        pSerial->dbg_err_hardware);
}
#endif
