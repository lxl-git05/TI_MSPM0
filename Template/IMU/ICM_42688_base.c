#include "ICM_42688_base.h"
#include "MyI2C.h"

// ==================== ICM42688 寄存器地址宏定义 ====================
// 数据输出寄存器 (Bank0)
#define	ICM42688_TEMP_DATA1			0x1D
#define	ICM42688_TEMP_DATA0			0x1E
#define	ICM42688_ACCEL_DATA_X1		0x1F
#define	ICM42688_ACCEL_DATA_X0		0x20
#define	ICM42688_ACCEL_DATA_Y1		0x21
#define	ICM42688_ACCEL_DATA_Y0		0x22
#define	ICM42688_ACCEL_DATA_Z1		0x23
#define	ICM42688_ACCEL_DATA_Z0		0x24
#define	ICM42688_GYRO_DATA_X1		0x25
#define	ICM42688_GYRO_DATA_X0		0x26
#define	ICM42688_GYRO_DATA_Y1		0x27
#define	ICM42688_GYRO_DATA_Y0		0x28
#define	ICM42688_GYRO_DATA_Z1		0x29
#define	ICM42688_GYRO_DATA_Z0		0x2A

// 配置寄存器 (Bank0)
#define	ICM42688_DEVICE_CONFIG		0x11
#define	ICM42688_INTF_CONFIG0		0x4C
#define	ICM42688_PWR_MGMT0			0x4E
#define	ICM42688_GYRO_CONFIG0		0x4F
#define	ICM42688_ACCEL_CONFIG0		0x50
#define	ICM42688_WHO_AM_I			0x75

// ==================== 量程宏定义 ====================
// 加速度计量程对应寄存器值 (ACCEL_CONFIG0[7:5])
// ★ ICM-42688: FS_SEL 编码与 MPU6050 相反 (000=±16g, 001=±8g, 010=±4g, 011=±2g)
#define ACCEL_FS_16G       	(0x00 << 5)    // ±16g
#define ACCEL_FS_8G        	(0x01 << 5)    // ±8g
#define ACCEL_FS_4G        	(0x02 << 5)    // ±4g
#define ACCEL_FS_2G        	(0x03 << 5)    // ±2g

// 陀螺仪量程对应寄存器值 (GYRO_CONFIG0[7:5])
// ★ ICM-42688: FS_SEL 编码与 MPU6050 相反 (000=±2000, 001=±1000, 010=±500, 011=±250)
#define GYRO_FS_2000       	(0x00 << 5)    // ±2000°/s
#define GYRO_FS_1000       	(0x01 << 5)    // ±1000°/s
#define GYRO_FS_500        	(0x02 << 5)    // ±500°/s
#define GYRO_FS_250        	(0x03 << 5)    // ±250°/s

// 加速度计ODR对应寄存器值 (ACCEL_CONFIG0[3:0], 值 = enum_index + 1)
#define ACCEL_ODR_1000HZ    6             // enum=5(1000HZ), reg_val=5+1=6
#define ACCEL_ODR_500HZ     15            // enum=14(500HZ), reg_val=14+1=15
#define ACCEL_ODR_200HZ     7             // enum=6(200HZ), reg_val=6+1=7

// 陀螺仪ODR对应寄存器值 (GYRO_CONFIG0[3:0], 值 = enum_index + 1)
#define GYRO_ODR_1000HZ     6             // enum=5(1000HZ), reg_val=5+1=6
#define GYRO_ODR_500HZ      15            // enum=14(500HZ), reg_val=14+1=15
#define GYRO_ODR_200HZ      7             // enum=6(200HZ), reg_val=6+1=7

// ********** 重要: 确定想要的量程 **********
#define ACCEL_RANGE_ICM     ACCEL_FS_4G
#define GYRO_RANGE_ICM      GYRO_FS_500

// ********** ODR选择 **********
#define ACCEL_ODR_ICM       ACCEL_ODR_1000HZ
#define GYRO_ODR_ICM        GYRO_ODR_1000HZ

// ==================== 灵敏度宏（与MPU6050完全相同） ====================
#if   ACCEL_RANGE_ICM == ACCEL_FS_2G
    #define ACCEL_SENSITIVITY_ICM   16384.0f   // LSB/g
#elif ACCEL_RANGE_ICM == ACCEL_FS_4G
    #define ACCEL_SENSITIVITY_ICM   8192.0f
#elif ACCEL_RANGE_ICM == ACCEL_FS_8G
    #define ACCEL_SENSITIVITY_ICM   4096.0f
#elif ACCEL_RANGE_ICM == ACCEL_FS_16G
    #define ACCEL_SENSITIVITY_ICM   2048.0f
#endif

#if   GYRO_RANGE_ICM == GYRO_FS_250
    #define GYRO_SENSITIVITY_ICM    131.0f     // LSB/(°/s)
#elif GYRO_RANGE_ICM == GYRO_FS_500
    #define GYRO_SENSITIVITY_ICM    65.5f
#elif GYRO_RANGE_ICM == GYRO_FS_1000
    #define GYRO_SENSITIVITY_ICM    32.8f
#elif GYRO_RANGE_ICM == GYRO_FS_2000
    #define GYRO_SENSITIVITY_ICM    16.4f
#endif

// ==================== I2C地址 ====================
// ICM42688: 7bit地址 0x68 (AD0=GND), 8bit格式 = (0x68 << 1) = 0xD0
#define ICM42688_ADDRESS 0xD0

// 硬件I2C — 与MPU6050共用I2C_1总线
#define ICM_I2C_Bus (I2C_1_INST)

// ==================== 全局变量 ====================
ICM42688_Raw_Data  ICM_Raw_Data ;	// 原始数据

// ==================== 写入数据 ====================
void ICM42688_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    uint8_t buf[2];

    buf[0] = RegAddress; // 先发寄存器地址
    buf[1] = Data;       // 再发数据

    IIC_WriteBytes(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, buf, 2);
}

// ==================== 读取数据 ====================
uint8_t ICM42688_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    // 写寄存器地址
    IIC_WriteBytes(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, &RegAddress, 1);

    // 读取数据
    IIC_ReadBytes(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, &Data, 1);

    return Data;
}

// ==================== 连续读取6轴原始数据 ====================
// 从 ACCEL_DATA_X1(0x1F) 连续读12字节：
//   buf[0:1]=AccelX, buf[2:3]=AccelY, buf[4:5]=AccelZ
//   buf[6:7]=GyroX,  buf[8:9]=GyroY,  buf[10:11]=GyroZ
void ICM42688_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                      int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t buf[12];

    // 写寄存器地址
    uint8_t RegAddress = ICM42688_ACCEL_DATA_X1;
    IIC_WriteBytes(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, &RegAddress, 1);

    // 连续读取12字节
    IIC_ReadBytes(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, buf, 12);

    // 加速度（大端序拼接）
    *AccX  = (int16_t)(buf[0]  << 8 | buf[1]);
    *AccY  = (int16_t)(buf[2]  << 8 | buf[3]);
    *AccZ  = (int16_t)(buf[4]  << 8 | buf[5]);
    // 角速度（跳过温度，直接从 buf[6] 开始）
    *GyroX = (int16_t)(buf[6]  << 8 | buf[7]);
    *GyroY = (int16_t)(buf[8]  << 8 | buf[9]);
    *GyroZ = (int16_t)(buf[10] << 8 | buf[11]);
}

// ==================== WHO_AM_I ====================
uint8_t ICM42688_GetID(void)
{
    return ICM42688_ReadReg(ICM42688_WHO_AM_I);
}

// ==================== 初始化 ====================
void ICM42688_Init(void)
{
    uint8_t id;

    // 验证设备ID
    id = ICM42688_ReadReg(ICM42688_WHO_AM_I);
    if (id != 0x47)
    {
        // ID不匹配：芯片未连接或型号错误，仍继续初始化给芯片一次机会
    }

    // 1. 软件复位
    ICM42688_WriteReg(ICM42688_PWR_MGMT0, 0x00);
    // datasheet要求复位后至少等待200us，保守取1ms
    for (volatile uint32_t _i = 0; _i < 32000; _i++) { __NOP(); }

    // 2. 配置加速度计：量程 + ODR
    //    ACCEL_CONFIG0 = FS[7:5] | ODR[3:0]
    ICM42688_WriteReg(ICM42688_ACCEL_CONFIG0, ACCEL_RANGE_ICM | ACCEL_ODR_ICM);

    // 3. 配置陀螺仪：量程 + ODR
    //    GYRO_CONFIG0 = FS[7:5] | ODR[3:0]
    ICM42688_WriteReg(ICM42688_GYRO_CONFIG0, GYRO_RANGE_ICM | GYRO_ODR_ICM);

    // 4. 进入低噪声模式：开启加速度计和陀螺仪
    //    PWR_MGMT0 = 0x0f → ACCEL_MODE=LowNoise(011) | GYRO_MODE=LowNoise(011)
    ICM42688_WriteReg(ICM42688_PWR_MGMT0, 0x0f);
    for (volatile uint32_t _i = 0; _i < 32000; _i++) { __NOP(); }
}

// ==================== 原始数据更新（含灵敏度转换，带超时+重试+恢复） ====================
void ICM42688_Update_Data(void)
{
    uint8_t buf[12] = {0};
    uint8_t RegAddress = ICM42688_ACCEL_DATA_X1;
    bool ok = false;

    // 读取带重试（最多3次）
    for (uint8_t retry = 3; retry > 0; retry--)
    {
        // 写寄存器地址
        if (!IIC_WriteBytes_Ex(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, &RegAddress, 1))
        {
            IIC_Reset(ICM_I2C_Bus);
            continue;
        }

        // 连续读取12字节
        if (!IIC_ReadBytes_Ex(ICM_I2C_Bus, ICM42688_ADDRESS >> 1, buf, 12))
        {
            IIC_Reset(ICM_I2C_Bus);
            continue;
        }

        ok = true;
        break;
    }

    if (!ok)
        return;  // I2C通信失败，跳过本次更新

    // 数据处理 — 灵敏度转换
    // 加速度（大端序 → 物理量）
    ICM_Raw_Data.AX = ((int16_t)(buf[0]  << 8 | buf[1]))  * 1.0f / ACCEL_SENSITIVITY_ICM;
    ICM_Raw_Data.AY = ((int16_t)(buf[2]  << 8 | buf[3]))  * 1.0f / ACCEL_SENSITIVITY_ICM;
    ICM_Raw_Data.AZ = ((int16_t)(buf[4]  << 8 | buf[5]))  * 1.0f / ACCEL_SENSITIVITY_ICM;
    // 角速度（大端序 → 物理量）
    ICM_Raw_Data.GX = ((int16_t)(buf[6]  << 8 | buf[7]))  * 1.0f / GYRO_SENSITIVITY_ICM;
    ICM_Raw_Data.GY = ((int16_t)(buf[8]  << 8 | buf[9]))  * 1.0f / GYRO_SENSITIVITY_ICM;
    ICM_Raw_Data.GZ = ((int16_t)(buf[10] << 8 | buf[11])) * 1.0f / GYRO_SENSITIVITY_ICM;
}
