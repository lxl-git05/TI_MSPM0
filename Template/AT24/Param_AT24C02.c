#include "Param_AT24C02.h"
#include "bsp_at24c02.h"

#include "Mode_1.h"

/* ==================== API实现 ==================== */

/**
 * @brief 初始化：注册所有参数，从AT恢复值（失败用默认值）
 */
void Param_AT24C02_Init(void) 
{
    for (int i = 0; i < At_Size ; i++)
        AT_Manager_Register(&s_AT_Params[i]);
    AT_Manager_Init();
}

/**
 * @brief 擦除整个AT24C02所有内存（填0x00）
 * @note AT24C02共256字节，擦除需要约1.3秒（256*5ms）
 */
void Param_AT24C02_EraseAll(void)
{
    for (uint16_t addr = 0; addr < 256; addr++)
    {
        AT24C02_WriteByte((uint8_t)addr, 0xFF);
    }
}

/**
 * @brief 批量保存所有参数到AT
 */
void Param_AT24C02_SaveAll(void)
{
    AT_Manager_SaveAll();
}

/**
 * @brief 写单个变量到AT（通过变量指针）
 * @return 1=成功, 0=失败
 */
uint8_t Param_AT24C02_Write(void *varPtr)
{
    return AT_Manager_Write(varPtr);
}

/**
 * @brief 从AT读单个变量到指针（通过变量指针）
 * @return 1=成功, 0=失败
 */
uint8_t Param_AT24C02_Read(void *varPtr)
{
    return AT_Manager_Read(varPtr);
}
