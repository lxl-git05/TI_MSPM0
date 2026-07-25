#include "Serial_base.h"

// ============== 全局变量（协议常量）==============
Serial_Agreement_ABC_TypeDef   Serial_Agreement_ABC;
Serial_Agreement_HEX_TypeDef   Serial_Agreement_HEX;

// ============== 协议初始化 ==============
void Serial_Agreement_ABC_Init(void)
{
    Serial_Agreement_ABC.head = '@';
    Serial_Agreement_ABC.end1 = '$';
    Serial_Agreement_ABC.end2 = '#';
}

void Serial_Agreement_HEX_Init(void)
{
    Serial_Agreement_HEX.head1 = 0xFF;
    Serial_Agreement_HEX.head2 = 0xAA;
    Serial_Agreement_HEX.end1  = 0x55;
    Serial_Agreement_HEX.end2  = 0xFE;
}
