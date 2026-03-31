#ifndef __SERIAL_H
#define __SERIAL_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

void UART_Printf_Init(void);
void UART_Printf(const char *fmt, ...);
void UART_DMA_Send(uint8_t *buf, uint16_t len) ;

#endif
