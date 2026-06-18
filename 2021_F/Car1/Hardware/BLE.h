#ifndef __BLE_H
#define __BLE_H

#include "ti_msp_dl_config.h"
#include "Serial.h"

void BLE_SendData(void) ;
void Manual_Serial1_Parse(void) ; // 仅 @EnBack，模拟对车蓝牙 Enable 位

extern bool Car2_Enable_Back ;

#endif
