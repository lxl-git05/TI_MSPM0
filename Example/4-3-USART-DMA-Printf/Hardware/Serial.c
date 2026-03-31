#include "Serial.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ===================== 配置 ===================== */
#define TX_BUF_SIZE 512

/* ===================== 缓冲区 ===================== */
static uint8_t txBuf[TX_BUF_SIZE];
static volatile uint16_t txHead = 0;
static volatile uint16_t txTail = 0;

/* DMA状态 */
static volatile uint8_t dmaBusy = 0;

/* 记录当前DMA发送长度（关键） */
static volatile uint16_t dmaLen = 0;

/* ===================== 内部函数声明 ===================== */
static void UART_DMA_Kick(void);
static void UART_WriteBuf(uint8_t *data, uint16_t len);

/* ===================== 初始化 ===================== */
void UART_Printf_Init(void)
{
    txHead = txTail = 0;
    dmaBusy = 0;
    dmaLen = 0;

    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    /* 确保中断开启 */
    DL_UART_enableInterrupt(UART_0_INST, DL_UART_INTERRUPT_DMA_DONE_TX);
}

/* ===================== 写入缓冲区 ===================== */
static void UART_WriteBuf(uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uint16_t next = (txHead + 1) % TX_BUF_SIZE;

        /* 满了：直接丢（可改阻塞） */
        if (next == txTail)
            return;

        txBuf[txHead] = data[i];
        txHead = next;
    }
}

/* ===================== 启动DMA ===================== */
static void UART_DMA_Kick(void)
{
    if (dmaBusy) return;
    if (txHead == txTail) return;

    dmaBusy = 1;

    uint16_t len;

    if (txHead > txTail)
        len = txHead - txTail;
    else
        len = TX_BUF_SIZE - txTail;

    /* 记录本次发送长度 */
    dmaLen = len;

    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&txBuf[txTail]);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&UART_0_INST->TXDATA);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, len);

    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
}

/* ===================== DMA发送接口 ===================== */
void UART_DMA_Send(uint8_t *buf, uint16_t len)
{
    UART_WriteBuf(buf, len);
    UART_DMA_Kick();
}

/* ===================== printf ===================== */
void UART_Printf(const char *fmt, ...)
{
    char tempBuf[128];

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(tempBuf, sizeof(tempBuf), fmt, args);
    va_end(args);

    if (len <= 0) return;

    if (len > sizeof(tempBuf))
        len = sizeof(tempBuf);

    UART_WriteBuf((uint8_t *)tempBuf, len);
    UART_DMA_Kick();
}

/* ===================== 中断函数 ===================== */
void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
        {
            txTail = (txTail + dmaLen) % TX_BUF_SIZE;

            dmaBusy = 0;

            if (txHead != txTail)
            {
                UART_DMA_Kick();
            }
        }
        break;

        default:
            break;
    }
}
