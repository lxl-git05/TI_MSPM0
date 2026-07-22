# TI MSPM0 智能小车项目 CLAUDE.md

> 基于 2026-07-21 深度分析生成，2026-07-22 更新（MyPWM/MyEncoder/Serial 移植完成 + HEX修复 + 状态机重构）

## 项目概述

TI MSPM0G3507 (Cortex-M0+) 智能小车竞赛项目。支持巡线、路口识别、MPU6050 姿态控制、双车蓝牙通信。

## 分层架构

| 层 | 目录 | 职责 |
|---|------|------|
| HAL | `MySystem/` | 硬件抽象层（GPIO/PWM/Encoder/Timer） |
| Hardware | `Hardware/` | 设备驱动（OLED/Key/RGB/MPU6050/Buzzer）+ 协议（Serial_base） |
| Function | `Function/` | 控制算法（MPU6050_Angle/Con_Task）+ 串口移植层（Serial_porting） |
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

## 中断体系

| 中断源 | 周期 | 回调/句柄 | 功能 | 定义位置 |
|--------|------|---------|------|---------|
| TIMER_0 | 1ms | `Timer_1ms_Callback` | 按键扫描、LED 闪烁 | `MySystem/MyTimer.c` |
| TIMER_1 | 20ms | `Timer_20ms_Callback` | 模式 tick 分发 | `MySystem/MyTimer.c` |
| GROUP1 (GPIOB) | 异步 | `GROUP1_IRQHandler` | 编码器 A/B 计数 | `Mode/Mode_G.c` |
| UART_0 | 异步 | `UART_0_INST_IRQHandler` | Serial1 RX 中断 | `Function/Serial_porting.c` |
| UART_1 | 异步 | `UART_1_INST_IRQHandler` | Serial2 RX 中断 | `Function/Serial_porting.c` |
| UART_4 | 异步 | `UART_4_INST_IRQHandler` | Serial4 RX 中断 | `Function/Serial_porting.c` |

## 当前状态：F407 核心模块移植阶段

### 已完成移植

| 模块 | 文件 | 状态 |
|------|------|------|
| MyPWM | `MySystem/MyPWM.h/c` | ✅ 结构体对齐 F407（Compare_Max/Min），SetCompare 双限幅 |
| MyEncoder | `MySystem/MyEncoder.h/c` | ✅ MyEncoder_Pins 引脚对封装，双路编码器 ISR |
| Serial | `Function/Serial_porting.h/c` | ✅ 中断接收+阻塞发送，ABC/HEX 协议复用 |
| Serial_base | `Hardware/Serial_base.h/c` | ✅ 协议层不改动（纯逻辑） |

### 关键 API 使用方式

```c
// 串口发送
Serial_printf(&Serial1, "Hello %d\r\n", 42);

// ABC 协议接收
if (Serial_GetNewPackageFlag_ABC(&Serial2)) {
    if (Serial_Check_Str(&Serial2, "TarXY")) { /* 处理 */ }
    Serial_SetIntData(&Serial2, "Update", "Update=%d", &val);
}

// 编码器读取（20ms 周期调用）
int cnt = MyEncoder_Get_CNT(&Motor_A_Encoder);         // 增量清零
int total = MyEncoder_Get_Total_CNT(&Motor_A_Encoder);  // 累计
```

### 文件修改规范

- 移植时适配层文件（`Serial_porting.c` 等）完全重写为 MSPM0 DriverLib
- 协议/算法层文件（`Serial_base.c`、`Serial_Parse_*`）不改动
- 每个模块移植后编译验证
- 使用 `Claude_Change.md` 记录变更

## 关键文件

- `MySystem/MySystem.h` — HAL 总入口，include 全部 DriverLib
- `App/AllHeader.h` — 项目统一头文件，include 所有模块
- `App/AllHeader.c` — `Initial_All()` 全局初始化（Encoder/Serial/OLED/MPU6050...）
- `Mode/Mode_G.c` — 模式管理器 + GROUP1_IRQHandler 总入口
- `Mode/Mode_2.c` — 综合测试模式（Encoder+Serial1/2）
- `README.md` — 引脚配置、UART/编码器/电机引脚表
- `empty.syscfg` — SysConfig 配置文件
