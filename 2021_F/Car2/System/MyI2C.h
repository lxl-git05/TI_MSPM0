#ifndef __MYI2C_H
#define __MYI2C_H

#include "ti_msp_dl_config.h"

void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t Ackbit);
uint8_t MyI2C_ReceiveAck(void);

void IIC_WriteBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len);
void IIC_ReadBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len);

#endif 
