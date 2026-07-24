#ifndef __SERIAL_PORTING_H
#define __SERIAL_PORTING_H

#include "Serial_base.h"
#include "Timer_Counter.h"

// HEX帧超时（4MHz ticks, 10ms = 40000 ticks）
#define HEX_FRAME_TIMEOUT_TICKS  40000U

// ============== 接收状态机（借鉴Car1的显式状态机模式）==============
typedef enum {
    Serial_Rx_Idle = 0,      // 空闲，等待帧头字节
    Serial_Rx_HEX  = 1,      // 正在接收HEX帧
    Serial_Rx_ABC  = 2,      // 正在接收ABC帧
} Serial_Rx_State;

// ============== 数据接收标志位（借鉴Car1）==============
typedef enum {
    RX_OK_HEX         = 0x00U,   // HEX数据包接收完成
    RX_OK_ABC         = 0x01U,   // ABC数据包接收完成
    RX_BUSY           = 0x02U,   // 数据包正在接收中
    RX_WAIT           = 0x03U,   // 等待数据传入
    RX_Error_Tail_HEX = 0x06U,   // HEX帧尾超时/溢出
    RX_Error_Tail_ABC = 0x07U,   // ABC帧尾超时/溢出
} Serial_RX_FLAG_Typedef;

// ============== 串口实例结构体 ==============
typedef struct {
    UART_Regs *uart;                       // MSPM0 UART外设寄存器
    IRQn_Type  IRQN;                       // UART中断号

    uint8_t  rx_temp;                      // 中断暂存字节（借鉴Car1：必须读出否则FIFO锁死）
    uint8_t  rxBuf[Serial_RX_BUF_SIZE];   // 接收缓冲区
    uint16_t rxLen;                       // 当前接收数据长度
    bool     rxOverflow;                  // 缓冲区溢出标志
    uint8_t  Status;                      // 接收状态机（0=Idle, 1=HEX, 2=ABC）
    uint32_t frame_start_tick;            // HEX帧检测开始时刻（tick值）

    Serial_ABC_Data_Typedef ABC_Data;     // ABC协议数据
    Serial_HEX_Data_Typedef HEX_Data;     // HEX协议数据
} Serial_Typedef;

// ============== 外部实例声明 ==============
#define Serial2_Enable 1
#define Serial3_Enable 1
#define Serial4_Enable 1

extern Serial_Typedef Serial1;   // UART_0 — USB调试 (115200)
#ifdef Serial2_Enable
extern Serial_Typedef Serial2;   // UART_1 — 树莓派通信 (115200)
#endif
#ifdef Serial3_Enable
extern Serial_Typedef Serial3;   // UART_2 — PB15/PB16 (115200)
#endif
#ifdef Serial4_Enable
extern Serial_Typedef Serial4;   // UART_4 — 串口屏 (115200)
#endif

// ============== 初始化/发送 ==============
void Serial_Init(void);
void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...);

// ============== 原始字节发送（借鉴待移植库）==============
void Serial_SendBytes(Serial_Typedef *pSerial, uint8_t *buf, uint16_t len);

// ============== HEX帧发送（借鉴待移植库，适配校验和格式）==============
// 帧格式：[0xFF][0xAA][count][D1_H][D1_L][D1_CK]...[0x55][0xFE]
void Serial_Send_HEX_Package(Serial_Typedef *pSerial, int16_t *data, uint8_t count);

// ============== 接收状态机（借鉴Car1：统一字节处理入口）==============
Serial_RX_FLAG_Typedef Serial_Rx_State_Check(Serial_Typedef *pSerial);

// ============== HEX协议 ==============
int16_t Serial_GetHexData(Serial_Typedef *pSerial, uint8_t index);
uint8_t Serial_GetHexLen(Serial_Typedef *pSerial);
uint8_t Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial);
int Serial_GetError_HEX(Serial_Typedef *pSerial);

// ============== ABC协议 ==============
uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial);
int Serial_GetError_ABC(Serial_Typedef *pSerial);
bool Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data);
bool Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data);
bool Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord);     // 子串匹配（strstr）
bool Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd);          // 精确匹配（strcmp，借鉴待移植库）

#endif // !__SERIAL_PORTING_H
