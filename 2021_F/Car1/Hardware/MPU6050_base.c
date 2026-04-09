#include "MPU6050_base.h"
#include "MyI2C.h"

#define MPU_I2C_Bus (I2C_1_INST)
#define MPU6050_ADDRESS 0xD0	// 这是含读写位和移位的地址写法(8位), (0xDx = 0x68 << 1) | x

MPU6050_Raw_Data  	MPU_Raw_Data ;	// 最初的角度

// 宏定义MPU6050的寄存器信息,使得更好理解
// 寄存器地址宏定义
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

// 加速度计量程对应寄存器值
#define ACCEL_2G        0x00    // ±2g
#define ACCEL_4G        0x08    // ±4g
#define ACCEL_8G        0x10    // ±8g
#define ACCEL_16G       0x18    // ±16g

// 陀螺仪量程对应寄存器值
#define GYRO_250        0x00    // ±250°/s
#define GYRO_500        0x08    // ±500°/s
#define GYRO_1000       0x10    // ±1000°/s
#define GYRO_2000       0x18    // ±2000°/s

// ********** 重要:确定想要的量程 **********
#define ACCEL_RANGE     ACCEL_2G
#define GYRO_RANGE      GYRO_250

// 宏定义确定满量程
/* --------------------- 灵敏度宏（用于数据转换） --------------------- */
#if   ACCEL_RANGE == ACCEL_2G
    #define ACCEL_SENSITIVITY   16384.0f   // LSB/g
#elif ACCEL_RANGE == ACCEL_4G
    #define ACCEL_SENSITIVITY   8192.0f
#elif ACCEL_RANGE == ACCEL_8G
    #define ACCEL_SENSITIVITY   4096.0f
#elif ACCEL_RANGE == ACCEL_16G
    #define ACCEL_SENSITIVITY   2048.0f
#endif

#if   GYRO_RANGE == GYRO_250
    #define GYRO_SENSITIVITY    131.0f     // LSB/(°/s)
#elif GYRO_RANGE == GYRO_500
    #define GYRO_SENSITIVITY    65.5f
#elif GYRO_RANGE == GYRO_1000
    #define GYRO_SENSITIVITY    32.8f
#elif GYRO_RANGE == GYRO_2000
    #define GYRO_SENSITIVITY    16.4f
#endif

// 写数据
void MPU6050_WriteReg(uint8_t RegAddress , uint8_t Data)
{
	// STM32写法
	// HAL_I2C_Mem_Write(hi2c_MPU6050, MPU6050_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, &Data, 1, 10000);

    uint8_t buf[2];

    buf[0] = RegAddress; // 先发寄存器地址
    buf[1] = Data;       // 再发数据

    IIC_WriteBytes(MPU_I2C_Bus, MPU6050_ADDRESS >> 1, buf, 2);
}

// 读数据
void MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *buf, uint8_t len)
{
    // 写寄存器地址
    IIC_WriteBytes(MPU_I2C_Bus, MPU6050_ADDRESS >> 1, &RegAddress, 1);

    // 读取数据
    IIC_ReadBytes(MPU_I2C_Bus, MPU6050_ADDRESS >> 1, buf, len);
}

// 初始化MPU6050相关配置
void MPU6050_Init(void)
{
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		// 电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		// 电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		// 采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);				// 配置寄存器，配置DLPF
	
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, GYRO_RANGE);	  // 陀螺仪配置寄存器
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, ACCEL_RANGE);	// 加速度计配置寄存器
}

// 原始数据更新
void MPU6050_Update_Data(void)
{
	// 数据接收区,连续读取
	uint8_t buf[14];

	// STM32写法
	// HAL_I2C_Mem_Read(hi2c_MPU6050,MPU6050_ADDRESS,MPU6050_ACCEL_XOUT_H,I2C_MEMADD_SIZE_8BIT,buf,14,1000) ; 

	// Ti写法
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, buf, 14);
	
	// 数据处理
	// 得到加速度
	MPU_Raw_Data.AX  = ((int16_t)(buf[0] << 8 | buf[1])) * 1.0 / ACCEL_SENSITIVITY ;
	MPU_Raw_Data.AY  = ((int16_t)(buf[2] << 8 | buf[3])) * 1.0 / ACCEL_SENSITIVITY ;
	MPU_Raw_Data.AZ  = ((int16_t)(buf[4] << 8 | buf[5])) * 1.0 / ACCEL_SENSITIVITY ;
	// 得到角速度(跳过了温度)
	MPU_Raw_Data.GX = ((int16_t)(buf[8]  << 8 | buf[9]))  * 1.0 / GYRO_SENSITIVITY ;
	MPU_Raw_Data.GY = ((int16_t)(buf[10] << 8 | buf[11])) * 1.0 / GYRO_SENSITIVITY ;
	MPU_Raw_Data.GZ = ((int16_t)(buf[12] << 8 | buf[13])) * 1.0 / GYRO_SENSITIVITY ;
}
