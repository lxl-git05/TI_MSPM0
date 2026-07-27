#include "Mode_4.h"
#include "AllHeader.h"

uint16_t send_data[4] = {1,2,3,4} ;
uint16_t get_data[4]  = {0} ;

void Mode_4_Setup(void)
{
    OLED_Clear();
}

void Mode_4_Loop(void)
{
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        Serial_Send_HEX_Package(&Serial3 , send_data , 4) ;
        send_data[0] += 1 ;
        send_data[1] += 2 ;
        send_data[2] += 3 ;
        send_data[3] += 4 ;
    }
    if (Key_Check(KEY_2, KEY_SINGLE))
    {
        Serial_printf(&Serial3, "Hello\r\n") ;
    }
    if (Serial_GetNewPackageFlag_HEX(&Serial3))
    {
        for (int i = 0 ; i < 4 ; i ++)
        {
            get_data[i] = Serial_GetHexData(&Serial3, i) ;
        }
    }
    OLED_Printf(0, 20, OLED_8X16, "%d,%d,%d,%d" , get_data[0],get_data[1],get_data[2],get_data[3]) ;
    OLED_Printf(0, 40, OLED_8X16, "%s",Serial3.ABC_Data.Serial_New_Package_ABC) ;
}

void Mode_4_Tick(void)
{
    
}

void Mode_4_Exit(void)
{
    OLED_Clear();
}
