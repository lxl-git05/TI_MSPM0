#include "MyI2C.h"
#include "stdint.h"

void Delay_us_diy_MPU(uint32_t us)
{
	us *= 10; // 校准因子，需实测调整
	while (us--)
	{
		__NOP(); __NOP(); __NOP(); __NOP(); // 4个NOP，防止被优化
	}
}

static void GPIO_Write(GPIO_Regs* gpio, uint32_t pins,bool isON)
{
	if (isON)
	{
		DL_GPIO_setPins(gpio, pins) ;
	}
	else 
	{
		DL_GPIO_clearPins(gpio, pins) ;
	}
}

void MyI2C_W_SCL(uint8_t Bitvalue)
{
	GPIO_Write(GPIO_I2C_0_SCL_PORT, GPIO_I2C_0_SCL_PIN, Bitvalue);
	Delay_us_diy_MPU(10);
}

void MyI2C_W_SDA(uint8_t Bitvalue)
{
    GPIO_Write(GPIO_I2C_0_SDA_PORT, GPIO_I2C_0_SDA_PIN, Bitvalue);
}

uint8_t MyI2C_R_SDA(void)
{
    return DL_GPIO_readPins(GPIO_I2C_0_SDA_PORT , GPIO_I2C_0_SDA_PIN);
}

void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);	// 这里的SDA放在前面是因为如果SDA先拉低，那么SCL拉低时，SDA会因为上拉电阻而拉高
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}	

void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}	

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}	
}

// 源代码会导致数据偏移一位,增加延时后改为下方代码
//uint8_t MyI2C_ReceiveByte(void)
//{
//	uint8_t i , Byte = 0x00 ;
//	MyI2C_W_SDA(1);
//	for (i = 0; i < 8; i++)
//	{
//		MyI2C_W_SCL(1);
//		if (MyI2C_R_SDA() == 1)
//		{
//			Byte |= (0x80 >> i);
//		}
//		MyI2C_W_SCL(0);
//	}
//	return Byte ;
//}

// 延时版接收代码
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    MyI2C_W_SDA(1);  // 释放SDA线，准备接收

    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SCL(1);          // 产生时钟高电平
        Delay_us_diy_MPU(5);     // 增加采样稳定性
        if (MyI2C_R_SDA())       // 读取SDA电平
        {
            Byte |= (1 << (7 - i)); // 从高位到低位填充
        }
        MyI2C_W_SCL(0);          // 产生时钟低电平
        Delay_us_diy_MPU(5);
    }
    return Byte;
}


void MyI2C_SendAck(uint8_t Ackbit)
{
	MyI2C_W_SDA(Ackbit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit ;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit ;
}

// ================ 硬件IIC封装 ================

void IIC_WriteBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    uint32_t index = 0 ;
    // 等待总线空闲
    while (!(DL_I2C_getControllerStatus(i2c_inst) &DL_I2C_CONTROLLER_STATUS_IDLE)) ;
    
    // 启动接收传输,准备读取len字节
    DL_I2C_startControllerTransfer(i2c_inst, devAddr, DL_I2C_CONTROLLER_DIRECTION_TX, len) ;

    // 不断读取FIFO中的数据
    while (index < len) 
    {
        if (!DL_I2C_isControllerTXFIFOFull(i2c_inst))
        {
            DL_I2C_transmitControllerData(i2c_inst, data[index++]);
        }
    }

    // 等待完成
    while (DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_BUSY);
}

void IIC_ReadBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    uint32_t index = 0 ;
    // 等待总线空闲
    while (!(DL_I2C_getControllerStatus(i2c_inst) &DL_I2C_CONTROLLER_STATUS_IDLE)) ;
    
    // 启动接收传输,准备读取len字节
    DL_I2C_startControllerTransfer(i2c_inst, devAddr, DL_I2C_CONTROLLER_DIRECTION_RX, len) ;

    // 不断读取FIFO中的数据
    while (index < len) 
    {
        if (!DL_I2C_isControllerRXFIFOEmpty(i2c_inst))
        {
            data[index++] = DL_I2C_receiveControllerData(i2c_inst) ;
        }
    }

    // 等待完成
    while (DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_BUSY);
}
