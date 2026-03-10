/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "bsp_w25q128.h"

#define delay_ms(X)	delay_cycles((CPUCLK_FREQ/1000)*(X))

//串口发送字符串
void uart0_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
         //当串口0忙的时候等待，不忙的时候再发送传进来的字符
        while( DL_UART_isBusy(UART_0_INST) == true );
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        DL_UART_Main_transmitData(UART_0_INST, *str++);
    }
}

int main(void)
{
    char uart_output_buff[50]={0};
    unsigned char read_write_buff[10] = {0};

    SYSCFG_DL_init();

    delay_ms(1);//等待器件部署

    //读取W25Q128的ID
    sprintf(uart_output_buff,"ID = %X\r\n",W25Q128_readID());
    uart0_send_string(uart_output_buff);

    //读取0地址的5个字节数据到buff
    W25Q128_read(read_write_buff, 0, 5);
    //串口输出读取的数据
    sprintf(uart_output_buff, "read_write_buff = %s\r\n",read_write_buff);
    uart0_send_string(uart_output_buff);

    //往0地址写入5个字节长度的数据 lckfb
    W25Q128_write((uint8_t*)"lckfb", 0, 5);

    delay_ms(10);//等待写入完毕

    //读取0地址的5个字节数据到buff
    W25Q128_read(read_write_buff, 0, 5);

    //串口输出读取的数据
    sprintf(uart_output_buff, "read_write_buff = %s\r\n", read_write_buff);
    uart0_send_string(uart_output_buff);

    while (1) {
    }
}