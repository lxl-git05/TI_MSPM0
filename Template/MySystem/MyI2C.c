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

#define I2C_TIMEOUT  100000UL   // 超时计数值（~10ms @32MHz）

#ifdef I2C_DEBUG_RESET_COUNT
volatile uint32_t IIC_Reset_Count = 0;   // I2C 总线复位次数
#endif

// ---- 超时版写入 ----
bool IIC_WriteBytes_Ex(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    uint32_t index  = 0;
    uint32_t tick   = 0;

    // 等待总线空闲（带超时）
    tick = I2C_TIMEOUT;
    while (!(DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_IDLE))
    {
        if (--tick == 0) return false;
    }

    // 启动发送传输
    DL_I2C_startControllerTransfer(i2c_inst, devAddr, DL_I2C_CONTROLLER_DIRECTION_TX, len);

    // 填充 TX FIFO
    tick = I2C_TIMEOUT;
    while (index < len)
    {
        if (!DL_I2C_isControllerTXFIFOFull(i2c_inst))
        {
            DL_I2C_transmitControllerData(i2c_inst, data[index++]);
            tick = I2C_TIMEOUT;          // 有数据流动，重置超时
        }
        else if (--tick == 0)
        {
            // TX FIFO 始终满——总线卡死
            return false;
        }
    }

    // 等待完成（带超时）
    tick = I2C_TIMEOUT;
    while (DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_BUSY)
    {
        if (--tick == 0) return false;
    }

    return true;
}

// ---- 超时版读取 ----
bool IIC_ReadBytes_Ex(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    uint32_t index = 0;
    uint32_t tick  = 0;

    // 等待总线空闲（带超时）
    tick = I2C_TIMEOUT;
    while (!(DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_IDLE))
    {
        if (--tick == 0) return false;
    }

    // 启动接收传输
    DL_I2C_startControllerTransfer(i2c_inst, devAddr, DL_I2C_CONTROLLER_DIRECTION_RX, len);

    // 读取 RX FIFO
    tick = I2C_TIMEOUT;
    while (index < len)
    {
        if (!DL_I2C_isControllerRXFIFOEmpty(i2c_inst))
        {
            data[index++] = DL_I2C_receiveControllerData(i2c_inst);
            tick = I2C_TIMEOUT;          // 有数据流入，重置超时
        }
        else if (--tick == 0)
        {
            // RX FIFO 始终空——从机未应答
            return false;
        }
    }

    // 等待完成（带超时）
    tick = I2C_TIMEOUT;
    while (DL_I2C_getControllerStatus(i2c_inst) & DL_I2C_CONTROLLER_STATUS_BUSY)
    {
        if (--tick == 0) return false;
    }

    return true;
}

// ---- 总线复位 ----
void IIC_Reset(I2C_Regs *i2c_inst)
{
#ifdef I2C_DEBUG_RESET_COUNT
    IIC_Reset_Count++;
#endif
    // 先中止当前传输，再调用 SysConfig 的完整初始化恢复
    if (i2c_inst == I2C_0_INST)
    {
        DL_I2C_resetControllerTransfer(I2C_0_INST);
        SYSCFG_DL_I2C_0_init();
    }
    else if (i2c_inst == I2C_1_INST)
    {
        DL_I2C_resetControllerTransfer(I2C_1_INST);
        SYSCFG_DL_I2C_1_init();
    }
}

// ---- 旧接口兼容（内部调用超时版，忽略返回值） ----
void IIC_WriteBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    if (!IIC_WriteBytes_Ex(i2c_inst, devAddr, data, len))
    {
        IIC_Reset(i2c_inst);       // 失败后复位总线
    }
}

void IIC_ReadBytes(I2C_Regs *i2c_inst , uint8_t devAddr , uint8_t* data , uint32_t len)
{
    if (!IIC_ReadBytes_Ex(i2c_inst, devAddr, data, len))
    {
        IIC_Reset(i2c_inst);       // 失败后复位总线
    }
}
