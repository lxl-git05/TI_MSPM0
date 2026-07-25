#ifndef __SERIAL_PORTING_H
#define __SERIAL_PORTING_H

#include "Serial_base.h"

// ============== 串口使能宏（注释即禁用）==============
#define Serial1_Enable      // UART_0 (TX=PA10, RX=PA11)
#define Serial2_Enable      // UART_1 (TX=PA8,  RX=PA9)
//#define Serial3_Enable      // UART_2 (TX=PB15, RX=PB16)
#define Serial4_Enable      // UART_3 (TX=PA26, RX=PA25)

// ============== 调试模式（注释即关闭）==============
#define Serial_Debug        // 开启调试计数器 + Serial_PrintDebug

// ============== TX 环形缓冲大小 ==============
#define TX_BUF_SIZE  512

// ============== 串口实例结构体 ==============
typedef struct
{
    // ---- 硬件参数 ----
    UART_Regs  *uart_INST;                      // UART 外设基址（UART_0_INST 等）
    uint8_t     uart_int_IRQN;                  // IRQ 号

    // ---- DMA（NULL = 中断模式）----
    DMA_Regs   *dma;                            // RX DMA 通道
    uint8_t     channelNum;

    // ---- TX DMA 环形缓冲（dma==NULL 时用轮询）----
    uint8_t     txBuf[TX_BUF_SIZE];
    uint16_t    txHead, txTail;
    uint8_t     dmaBusy;
    uint16_t    dmaLen;
    DMA_Regs   *tx_dma;
    uint8_t     tx_channelNum;

    // ---- 接收 ----
    uint8_t     rx_temp;                        // 中断暂存字节
    uint16_t    rxCnt;                          // 当前帧已收字节数
    uint8_t     rxBuf[Serial_RX_BUF_SIZE];      // 原始接收缓冲
    uint8_t     Status;                         // 状态机: 0=Idle, 1=HEX, 2=ABC

    // ---- 协议数据（来自 Serial_base.h）----
    Serial_HEX_Data_Typedef HEX_Data;
    Serial_ABC_Data_Typedef ABC_Data;

#ifdef Serial_Debug
    // ---- 调试计数器 ----
    uint32_t   dbg_rx_bytes;       // 总接收字节数
    uint32_t   dbg_rx_frames;      // 状态机帧检测次数（HEX+ABC，含校验失败的）
    uint32_t   dbg_parse_ok;       // 成功解析次数（校验全部通过）
    uint32_t   dbg_frame_lost;     // 丢帧次数（新帧到达时旧帧未被主循环消费）
    uint32_t   dbg_err_head;       // 帧头错误次数
    uint32_t   dbg_err_tail;       // 帧尾错误次数
    uint32_t   dbg_err_overflow;   // 协议层溢出（帧长超限/缓冲区满）
    uint32_t   dbg_err_hardware;   // 硬件层错误（OVERRUN/FRAMING/BREAK/NOISE/PARITY）
#endif

} Serial_Typedef;

// ============== 外部实例（由使能宏控制）==============
#ifdef Serial1_Enable
extern Serial_Typedef Serial1;
#endif
#ifdef Serial2_Enable
extern Serial_Typedef Serial2;
#endif
#ifdef Serial3_Enable
extern Serial_Typedef Serial3;
#endif
#ifdef Serial4_Enable
extern Serial_Typedef Serial4;
#endif

// ============== 初始化 ==============
void Serial_Init(void);

// ============== 发送 ==============
void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...);     // 发送（DMA有则用，无则轮询）
void Serial_send_string(Serial_Typedef *pSerial, char *str);           // 轮询发字符串
void Serial_SendBytes(Serial_Typedef *pSerial, uint8_t *buf, uint16_t len);
void Serial_Send_HEX_Package(Serial_Typedef *pSerial, uint16_t *data, uint8_t count);

// ============== HEX 协议 ==============
uint8_t  Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial);
int16_t  Serial_GetHexData(Serial_Typedef *pSerial, uint8_t index);
uint8_t  Serial_GetHexLen(Serial_Typedef *pSerial);
int      Serial_GetError_HEX(Serial_Typedef *pSerial);

// ============== ABC 协议 ==============
uint8_t  Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial);
int      Serial_GetError_ABC(Serial_Typedef *pSerial);
bool     Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data);
bool     Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data);
bool     Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord);          // 子串匹配
bool     Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd);               // 精确匹配

// ============== 调试 ==============
#ifdef Serial_Debug
void Serial_PrintDebug(Serial_Typedef *pSerial);    // 打印调试统计
#endif

#endif
