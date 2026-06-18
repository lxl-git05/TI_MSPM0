#ifndef __BLE_H
#define __BLE_H

#include "ti_msp_dl_config.h"
#include "Serial.h"

/* HEX 帧 8×uint16（FF AA | len | Data[0..7] | 55 FE）
 * [1] Target  [2] Car2_Enable_Back(提高1)  [3] Road2  [4] Road3  [5] Road4L  [6] Road4R
 * [7] Car2_Enable_Go(提高2,Car1发)  [8] Car1_Enable_Back(提高2,Car2发,Car1收) */
#define BLE_DATA_COUNT 8

void BLE_SendData(void) ;
void BLE_Get(void) ;
void Manual_Serial1_Parse(void) ;

extern bool Car2_Enable_Back ;
extern bool Car2_Enable_Go ;
extern bool Car1_Enable_Back ;

#endif
