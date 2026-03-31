#include "ti_msp_dl_config.h"
#include "string.h"

#include "Serial.h"

int main(void)
{
    SYSCFG_DL_init();

    UART_Printf_Init();

    while (1) 
    {
        static int a = 0;
        if (a == 0)
        {
            a++;
            UART_Printf("Hello %d\r\n", 123);
            UART_Printf("world %d\r\n", 123);
            UART_Printf("heiha %d\r\n", 123);
            UART_Printf("float %.2f\r\n", 1.234);
        }
    }
}
