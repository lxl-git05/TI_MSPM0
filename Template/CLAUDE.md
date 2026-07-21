# TI MSPM0 智能小车项目 CLAUDE.md

> 基于 2026-07-21 深度分析生成

## 项目概述

TI MSPM0G3507 (Cortex-M0+) 智能小车竞赛项目。支持巡线、路口识别、MPU6050 姿态控制、双车蓝牙通信。

## 分层架构

| 层 | 目录 | 职责 |
|---|------|------|
| HAL | `MySystem/` | 硬件抽象层（GPIO/PWM/Encoder/Timer）— **需要重构** |
| System | `System/` | 通用中间件（Task/Serial/I2C/PID/Stack/DList） |
| Hardware | `Hardware/` | 设备驱动（Motor/OLED/MPU6050/Key/BLE/Track/RGB） |
| Function | `Function/` | 控制算法（Motor/PID/MPU/Angle/Car/Track） |
| Mode | `Mode/` | 应用模式（Manager/PID调试/角度环/巡线） |
| App | `App/` | 状态机/菜单/统一初始化 |
| Tools | `Tools/` | 工具（按键调参/LED闪烁/计时/延时） |

## 全局依赖链

```
ti_msp_dl_config.h (SysConfig 生成, 60+ 厂商头文件)
  ← MySystem.h (HAL 总入口)
    ← AllHeader.h (项目统一入口)
      ← 所有 .c 文件
```

**任何 `#include "AllHeader.h"` 或 `#include "MySystem.h"` 的文件都会获得全部 TI DriverLib 符号。**

## 中断体系

| 中断源 | 周期 | 回调函数 | 功能 |
|--------|------|---------|------|
| TIMER_0 | 1ms | `Timer_1ms_Callback` | 按键扫描、LED 闪烁、全局延时 |
| TIMER_1 | 20ms | `Timer_20ms_Callback` | 电机更新、MPU6050 姿态、模式 tick |
| GROUP1 (GPIOB) | 异步 | `GROUP1_IRQHandler` | 编码器 A/B 计数（双边沿） |

## 当前状态：等待 MySystem 重构

**TODO 列表**（`TaskCreate` 中已记录 6 个 Phase）：

1. **Phase 1**: MySystem 全重构 — 设计干净 HAL 接口 + TI 后端
2. **Phase 2**: System 层适配新 HAL
3. **Phase 3**: Hardware 层适配新 HAL
4. **Phase 4**: Function 层适配新 HAL
5. **Phase 5**: Mode/App/Tools 层适配新 HAL
6. **Phase 6**: 集成测试与功能验证

## 文件修改规范

- 所有重构在 `hal/` 新目录中进行，保留原 `MySystem/` 作为参考
- 重构完成后逐步替换各层引用
- 每个 Phase 完成后编译验证
- 使用 `Claude_Change.md` 记录变更

## 关键文件

- `MySystem/MySystem.h` — HAL 总入口，重构的起点
- `App/AllHeader.h` — 项目统一头文件
- `App/Control.c` — 小车状态机（Stop/Forward/TurnL/TurnR/TurnF/TurnH）
- `README.md` — 引脚配置
- `Claude_Temp/MySystem-深度分析.md` — 详细架构分析
