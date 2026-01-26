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

// extern volatile uint32_t interruptVectors[];

int a ;
uint32_t status ;   // 读取PB21引脚的中断状态,这里设置为全局变量是为了读清楚整个流程

int main(void)
{
    SYSCFG_DL_init();

    /*
     * Turn OFF LED if SW is open, ON if SW is closed.
     * LED starts OFF by default.
     */
    // 开启按键(GPIOB)的触发中断,实际上是开启NVIC的Group1内的GPIOB的触发中断
    NVIC_EnableIRQ(GPIO_SWITCHES_INT_IRQN);

    while (1) 
    {
        status = DL_GPIO_getEnabledInterruptStatus(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN) ;
    }
}

// GPIOA和GPIOB的触发中断都存在于GRP1中，所以触发句柄都是GRP1
void GROUP1_IRQHandler(void)
{
    // 检查是谁开启的中断
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
    {
        // 这个其实是封装过了,应该是GPIOB的IIDX：DL_INTERRUPT_GROUP1_IIDX_GPIOB
        case GPIO_SWITCHES_INT_IIDX:
            // 检测是GPIOB的哪一个引脚触发的中断,返回值：GPIO_PIN
            status = DL_GPIO_getEnabledInterruptStatus(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN) ;
            if (status & GPIO_SWITCHES_USER_SWITCH_1_PIN)
            {
                /* If SW is high, turn the LED off */
                if (DL_GPIO_readPins(GPIO_SWITCHES_PORT, GPIO_SWITCHES_USER_SWITCH_1_PIN))  // 捕获到边缘，并且肯定是：上升沿，且值为1
                {
                    a++ ;
                    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
                }
                /* Otherwise, turn the LED on */
                else 
                {                                                                      // 捕获到边缘，并且肯定是：下降沿，且值为0
                    a-- ;
                    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
                }
                DL_GPIO_clearInterruptStatus(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN);
            }
            break;
    }
}
