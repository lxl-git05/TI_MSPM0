#ifndef __BSP_AT24C02_H__
#define __BSP_AT24C02_H__
/*
    声明：基于TI MSPM0G3507 硬件I2C驱动AT24C02
    使用 I2C_1 (PB2=SCL, PB3=SDA) 与 MPU6050 共用总线
    底层调用 MyI2C 库的 IIC_WriteBytes / IIC_ReadBytes
*/

/* Includes ------------------------------------------------------------------*/
#include "MySystem.h"

/* ==================== AT24C02 硬件配置 ==================== */
/* I2C总线: 与MPU6050共用I2C_1 (PB2=SCL, PB3=SDA)，地址不冲突 */
#define AT24C02_I2C_BUS       (I2C_1_INST)  // I2C_0_INST是新PCB的

/* AT24C02 7-bit设备地址 = 0x50 (A2=A1=A0=GND) */
#define AT24C02_ADDR_7BIT     0x50

/* ==================== 函数声明（统一接口）==================== */
void     AT24C02_Init(void);
uint8_t  AT24C02_WriteByte(uint8_t WordAddress, uint8_t Data);
uint8_t  AT24C02_ReadByte(uint8_t WordAddress);

#endif /* __BSP_AT24C02_H__ */
