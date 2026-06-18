#ifndef __BLE_H
#define __BLE_H

#include "ti_msp_dl_config.h"
#include "Serial.h"

#define BLE_DATA_COUNT 8

void BLE_Get(void) ;
void BLE_SendData(void) ;

extern bool Car2_Enable_Back ;
extern bool Car2_Enable_Go ;
extern bool Car1_Enable_Back ;

#endif
