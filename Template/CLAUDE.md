# TI MSPM0 智能小车项目 CLAUDE.md

> 基于 2026-07-21 深度分析生成，2026-07-22 更新（Serial状态机重构 + HEX修复 + 引脚完善 + EC11移植）

## 项目概述

TI MSPM0G3507 (Cortex-M0+) 智能小车竞赛项目。支持巡线、路口识别、MPU6050 姿态控制、双车蓝牙通信。

## 分层架构

| 层 | 目录 | 职责 |
|---|------|------|
| HAL | `MySystem/` | 硬件抽象层（GPIO/PWM/Encoder/Timer） |
| Hardware | `Hardware/` | 设备驱动（OLED/Key/RGB/MPU6050/Buzzer/Encoder_Key）+ 协议（Serial_base） |
| Function | `Function/` | 控制算法（MPU6050_Angle/Con_Task/Con_Motor/Control）+ 串口移植层（Serial_porting） |
| Software | `Software/` | 通用中间件（MyPID/Queue） |
| Mode | `Mode/` | 应用模式（Manager/Mode_1/Mode_2/Mode_3） |
| App | `App/` | 状态机/菜单/统一初始化 |
| Tools | `Tools/` | 工具（LED闪烁/LED_Flash/Timer_Counter） |

## 全局依赖链

```
ti_msp_dl_config.h (SysConfig 生成, 60+ 厂商头文件)
  ← MySystem.h (HAL 总入口)
    ← AllHeader.h (项目统一入口)
      ← 所有 .c 文件
```

**任何 `#include "AllHeader.h"` 或 `#include "MySystem.h"` 的文件都会获得全部 TI DriverLib 符号。**

## GPIO 引脚分配（对照 empty.syscfg + README.md）

| 模块 | 符号 | 引脚 | 类型 |
|------|------|------|------|
| LED | MyGPIO_LED0 | PB22 | 输出 |
| LED | MyGPIO_LED_Time | PB9 | 输出 |
| Key | MyGPIO_Key0 | PB21 | 输入上拉 |
| Key | MyGPIO_Key1 | PA15 | 输入上拉 |
| Key | MyGPIO_Key2 | PA17 | 输入上拉 |
| OLED | MyGPIO_OLED_SCL | PA31 | 输出（软件I2C） |
| OLED | MyGPIO_OLED_SDA | PA28 | 输出（软件I2C） |
| MotorA | MyGPIO_Motor_A_IN1 | PB4 | PWM (TIMA1 CCP0) |
| MotorA | MyGPIO_Motor_A_IN2 | PB12 | 输出 |
| MotorB | MyGPIO_Motor_B_IN1 | PB5 | PWM (TIMA1 CCP1) |
| MotorB | MyGPIO_Motor_B_IN2 | PB13 | 输出 |
| EncoderA | Motor_A_Encoder | PB14/PB1 | 输入 双边沿中断 |
| EncoderB | Motor_B_Encoder | PB11/PB10 | 输入 双边沿中断 |
| RGB | MyGPIO_RGB_R/G/B | PB6/PB7/PB8 | 输出 |
| Buzzer | MyGPIO_Buzzer | PB23 | 输出（+5V有源） |
| Elec | MyGPIO_Elec | PB24 | 输出（电磁铁） |
| TCRT | MyGPIO_TCRT | PA13 | 输入上拉 |
| EC11 | MyGPIO_EC11_S1 | PA12 | 输入上拉 下降沿中断 |
| EC11 | MyGPIO_EC11_S2 | PA14 | 输入上拉 下降沿中断 |
| EC11 | MyGPIO_EC11_Key | PA16 | 输入上拉 |
| Stepper1 | MyGPIO_Stepper_En/Dir | PA18/PB17 | 输出 |
| Stepper2 | MyGPIO_Stepper2_En/Dir | PB18/PB19 | 输出 |
| Y8 | MyGPIO_Y8_CLK | PA22 | 输出 |
| Y8 | MyGPIO_Y8_DAT | PB20 | 输入上拉 |

> **换芯片时只需修改 `MySystem/MySystem.c` 中的 port/pin 宏，其他文件无需修改。**

## 中断体系

| 中断源 | 周期 | 回调/句柄 | 功能 | 定义位置 |
|--------|------|---------|------|---------|
| TIMER_0 | 1ms | `Timer_1ms_Callback` | 按键扫描、LED 闪烁 | `MySystem/MyTimer.c` |
| TIMER_1 | 20ms | `Timer_20ms_Callback` | 模式 tick 分发 | `MySystem/MyTimer.c` |
| GROUP1 (GPIOB) | 异步 | `GROUP1_IRQHandler` | 编码器 A/B 电机测速 | `Mode/Mode_G.c` |
| GROUP1 (GPIOA) | 异步 | `GROUP1_IRQHandler` | EC11 旋转编码器 | `Mode/Mode_G.c` |
| UART_0 | 异步 | `UART_0_INST_IRQHandler` | Serial1 RX 中断（状态机） | `Function/Serial_porting.c` |
| UART_1 | 异步 | `UART_1_INST_IRQHandler` | Serial2 RX 中断（状态机） | `Function/Serial_porting.c` |
| UART_4 | 异步 | `UART_4_INST_IRQHandler` | Serial4 RX 中断（状态机） | `Function/Serial_porting.c` |

## 当前状态：F407 核心模块移植阶段

### 已完成移植

| 模块 | 文件 | 状态 |
|------|------|------|
| MyPWM | `MySystem/MyPWM.h/c` | ✅ 结构体对齐 F407（Compare_Max/Min），SetCompare 双限幅 |
| MyEncoder | `MySystem/MyEncoder.h/c` | ✅ MyEncoder_Pins 引脚对封装，双路编码器 ISR |
| Serial | `Function/Serial_porting.h/c` | ✅ 显式状态机（Idle/HEX/ABC），HEX校验和+超时+帧尾验证，阻塞发送 |
| Serial_base | `Hardware/Serial_base.h/c` | ✅ 协议层不改动（纯逻辑） |
| Encoder_Key | `Hardware/Encoder_Key.h/c` | ✅ EC11 旋转编码器，GPIOA 下降沿中断+方向判断，NVIC 参照 MyEncoder 模式 |
| GPIO 引脚 | `MySystem/MyGPIO.h` + `MySystem/MySystem.c` | ✅ 全部 25 个引脚声明完成，对照 syscfg+README |

### Serial 状态机架构（借鉴待移植库 Status 0/1/2 模式）

```
Idle ──(收到 0xFF)──→ HEX ──(帧完成/错误/超时)──→ Idle
  │                    │
  │                    ├──(字节2 ≠ 0xAA)→ Idle（假帧头）
  │                    └──(10ms超时)→ Idle
  │
  └──(收到 '@')──→ ABC ──(帧完成/超长)──→ Idle
```

**新增函数（借鉴待移植库）：**
- `Serial_SendBytes()` — 原始字节发送
- `Serial_Send_HEX_Package()` — 构造并发送 HEX 帧（含 XOR 校验）
- `Serial_CheckCmd()` — strcmp 精确匹配（补充 Serial_Check_Str 的 strstr 子串匹配）

### EC11 旋转编码器架构

```
EC11 旋转
  │  S1 (PA12) ↓ FALL_INT
  │  S2 (PA14) ↓ FALL_INT
  ▼
GROUP1_IRQHandler → DL_INTERRUPT_GROUP1_IIDX_GPIOA → EC11_Encoder_ISR()
       ├─ S1下降沿 + S2低 → Encoder_Count++
       └─ S2下降沿 + S1低 → Encoder_Count--

EC11 按键 → MyGPIO_EC11_Key (PA16) → Key_Tick 轮询 → KEY_3
```

### 关键 API 使用方式

```c
// 串口发送
Serial_printf(&Serial1, "Hello %d\r\n", 42);

// HEX 帧发送
int16_t data[] = {100, 200, 300};
Serial_Send_HEX_Package(&Serial2, data, 3);

// ABC 协议接收
if (Serial_GetNewPackageFlag_ABC(&Serial2)) {
    if (Serial_Check_Str(&Serial2, "TarXY")) { /* 子串匹配 */ }
    if (Serial_CheckCmd(&Serial2, "Start"))  { /* 精确匹配 */ }
    Serial_SetIntData(&Serial2, "Update", "Update=%d", &val);
}

// HEX 协议接收
if (Serial_GetNewPackageFlag_HEX(&Serial1)) {
    uint8_t len = Serial_GetHexLen(&Serial1);
    for (uint8_t i = 0; i < len; i++) {
        int16_t val = Serial_GetHexData(&Serial1, i);
    }
}

// 电机编码器读取（20ms 周期调用）
int cnt = MyEncoder_Get_CNT(&Motor_A_Encoder);          // 增量清零
int total = MyEncoder_Get_Total_CNT(&Motor_A_Encoder);   // 累计

// EC11 旋转编码器读取
int16_t delta = Encoder_Get();   // 读后自动清零

// EC11 按键（通过 Key 模块）
if (Key_Check(KEY_3, KEY_SINGLE)) { /* EC11按键单击 */ }
```

### 文件修改规范

- 移植时适配层文件（`Serial_porting.c` 等）完全重写为 MSPM0 DriverLib
- 协议/算法层文件（`Serial_base.c`、`Serial_Parse_*`）不改动
- 新硬件驱动使用 MySystem 风格：`MyGPIO_ReadPin/WritePin` + pin 实例
- GPIO 中断使能参照 `MyEncoder_Init` 模式：`NVIC_ClearPendingIRQ` + `NVIC_EnableIRQ`
- 每个模块移植后编译验证
- 使用 `Claude_Change.md` 记录变更

## 关键文件

- `MySystem/MySystem.h` — HAL 总入口，include 全部 DriverLib
- `MySystem/MySystem.c` — 全部 GPIO/PWM/Encoder 实例定义（换芯片唯一修改点）
- `MySystem/MyGPIO.h` — 全部 GPIO 引脚声明（含物理引脚注释）
- `App/AllHeader.h` — 项目统一头文件，include 所有模块
- `App/AllHeader.c` — `Initial_All()` 全局初始化
- `Mode/Mode_G.c` — 模式管理器 + GROUP1_IRQHandler 总入口（GPIOA+GPIOB）
- `Mode/Mode_2.c` — 综合测试模式（Encoder+Serial1/2）
- `Function/Serial_porting.c` — 串口状态机 ISR + HEX/ABC 协议解析
- `Hardware/Encoder_Key.c` — EC11 旋转编码器驱动（GPIOA 中断）
- `README.md` — 引脚配置、UART/编码器/电机引脚表
- `empty.syscfg` — SysConfig 配置文件
- `Claude_Change.md` — 变更记录
