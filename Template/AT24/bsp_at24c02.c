#include "bsp_at24c02.h"
#include "MyI2C.h"

/* ========================================================================
 * 简单毫秒延迟（空循环，MSPM0 @80MHz 粗略校准）
 * 待实测校准：用示波器测量5ms脉冲调整循环常数
 * ======================================================================== */
static void AT24C02_DelayMs(uint32_t ms)
{
    while (ms--)
    {
        volatile uint32_t i = 8000;
        while (i--)
        {
            __NOP();
        }
    }
}

/* ========================================================================
 * 公开API（TI DriverLib 硬件I2C）
 * ======================================================================== */

/**
 * @brief AT24C02 初始化
 * @note  I2C_0 已在 SYSCFG_DL_init() → SYSCFG_DL_I2C_0_init() 中初始化
 */
void AT24C02_Init(void)
{
    /* I2C_1 硬件已在系统启动时初始化，此处无需额外操作 */
}

/**
 * @brief 向AT24C02指定地址写入一个字节
 * @param WordAddress 字地址 (0-255)
 * @param Data        要写入的数据
 * @return 1=成功
 */
uint8_t AT24C02_WriteByte(uint8_t WordAddress, uint8_t Data)
{
    uint8_t buf[2];
    buf[0] = WordAddress;       /* 先发字地址（寄存器地址） */
    buf[1] = Data;              /* 再发数据 */

    IIC_WriteBytes(AT24C02_I2C_BUS, AT24C02_ADDR_7BIT, buf, 2);
    AT24C02_DelayMs(5);         /* 等待内部写周期完成 (tWR ≤ 5ms) */
    return 1;                   /* 成功 */
}

/**
 * @brief 从AT24C02指定地址读取一个字节
 * @param  WordAddress 字地址 (0-255)
 * @return 读取到的数据
 * @note   随机读取时序：发送字地址（伪写）→ STOP → 读取数据
 */
uint8_t AT24C02_ReadByte(uint8_t WordAddress)
{
    uint8_t data = 0;

    /* 步骤1: 发送字地址（伪写，设置内部地址指针）*/
    IIC_WriteBytes(AT24C02_I2C_BUS, AT24C02_ADDR_7BIT, &WordAddress, 1);

    /* 步骤2: 读取1字节（AT24C02随机读取支持中间有STOP）*/
    IIC_ReadBytes(AT24C02_I2C_BUS, AT24C02_ADDR_7BIT, &data, 1);

    return data;
}
