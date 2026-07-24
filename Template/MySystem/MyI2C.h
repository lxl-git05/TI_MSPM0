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
bool IIC_WriteBytes_Ex(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len);
bool IIC_ReadBytes_Ex(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len);
void IIC_Reset(I2C_Regs *i2c_inst);

// ---- I2C 复位次数监控（调试用，注释即关闭） ----
#define I2C_DEBUG_RESET_COUNT
#ifdef I2C_DEBUG_RESET_COUNT
extern volatile uint32_t IIC_Reset_Count;
#endif

#endif 
