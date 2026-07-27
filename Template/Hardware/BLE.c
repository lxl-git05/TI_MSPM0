#include "BLE.h"

void BLE_Send_Data(uint16_t *ble_send_data, uint8_t count)
{
    Serial_Send_HEX_Package(&BLE_Serial , ble_send_data , count) ;
}

void BLE_Update(void)
{
    if (Serial_GetNewPackageFlag_HEX(&BLE_Serial))
    {

    }
}
