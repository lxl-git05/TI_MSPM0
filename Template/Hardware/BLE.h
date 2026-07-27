#ifndef __BLE_H
#define __BLE_H

#include "MySystem.h"
#include "Serial_porting.h"

#define BLE_Serial Serial3

void BLE_Send_Data(uint16_t *ble_send_data, uint8_t count);

void BLE_Update(void) ;

#endif
