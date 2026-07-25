# Serial 串口模块 — 完整说明书

> MSPM0G3507 多协议串口驱动，支持 HEX（带 XOR 校验）和 ABC（文本/JSON 式）两种帧协议。
> 中断逐字节接收 + 显式状态机 + 错误中断恢复，覆盖 DMA 和中断两种模式。

---

## 目录

- [1. 概述与架构](#1-概述与架构)
- [2. 硬件配置](#2-硬件配置)
- [3. 文件结构](#3-文件结构)
- [4. 协议格式](#4-协议格式)
  - [4.1 HEX 协议（带 XOR 校验）](#41-hex-协议带-xor-校验)
  - [4.2 ABC 协议（文本帧）](#42-abc-协议文本帧)
- [5. 接收状态机](#5-接收状态机)
  - [5.1 状态转换图](#51-状态转换图)
  - [5.2 ISR 调度流程](#52-isr-调度流程)
- [6. API 完整参考](#6-api-完整参考)
  - [6.1 初始化](#61-初始化)
  - [6.2 发送 API](#62-发送-api)
  - [6.3 HEX 接收 API](#63-hex-接收-api)
  - [6.4 ABC 接收 API](#64-abc-接收-api)
  - [6.5 调试 API](#65-调试-api)
- [7. 调试打印说明](#7-调试打印说明)
- [8. 移植指南](#8-移植指南)
  - [8.1 新增串口实例](#81-新增串口实例)
  - [8.2 移植到其他 MCU](#82-移植到其他-mcu)
  - [8.3 SysConfig 注意事项](#83-sysconfig-注意事项)
- [9. 常见问题与注意事项](#9-常见问题与注意事项)
- [10. 高级主题](#10-高级主题)
  - [10.1 中断模式 vs DMA 模式](#101-中断模式-vs-dma-模式)
  - [10.2 多中断环境优先级](#102-多中断环境优先级)
  - [10.3 高吞吐量场景](#103-高吞吐量场景)
- [11. 实战示例代码](#11-实战示例代码)
  - [示例 1：最小初始化](#示例-1最小初始化从头新建项目)
  - [示例 2：调试日志打印](#示例-2调试日志打印serial1-做-printf)
  - [示例 3：编码器数据上报](#示例-3编码器数据上报hex-协议一对多)
  - [示例 4：PID 在线调参](#示例-4pid-在线调参abc-协议vofa-配合)
  - [示例 5：双协议混合](#示例-5双协议混合同一串口同时收-hex-和-abc)
  - [示例 6：树莓派指令通信](#示例-6树莓派指令通信命令--解析--应答)
  - [示例 7：完整 Mode 生命周期](#示例-7完整-mode-生命周期setup--loop--tick--exit)
  - [示例 8：错误处理](#示例-8错误处理检查错误码并分级响应)
  - [示例 9：调试开关](#示例-9调试开关key-长按输出统计一键诊断)
  - [示例 10：双车通信](#示例-10双车通信car1--car2-蓝牙)
  - [示例 11：串口屏交互](#示例-11串口屏交互tjc-串口屏-abc-协议)
- [附录 A：关键宏定义速查](#附录-a关键宏定义速查)
- [附录 B：错误码速查](#附录-b错误码速查)

---

## 1. 概述与架构

### 分层设计

```
┌─────────────────────────────┐
│  应用层 (Mode_1/2/3)        │  调用 API：Serial_printf / GetNewPackageFlag …
├─────────────────────────────┤
│  适配层 Serial_porting.c/h  │  ISR、状态机、printf、DMA、MSPM0 DriverLib 调用
├─────────────────────────────┤
│  协议层 Serial_base.c/h     │  帧常量定义、数据结构、错误码（纯 C，跨平台不动）
└─────────────────────────────┘
```

- **协议层** (`Serial_base.h/c`)：无任何 MCU 依赖，定义帧格式、缓冲区大小、错误码。移植时**不改动**。
- **适配层** (`Serial_porting.h/c`)：绑定 MSPM0 DriverLib，实现 ISR、初始化、状态机、协议解析、发送。移植时**重点修改**。

### 关键设计决策

| 项目 | F407 原版 | MSPM0 移植版 |
|------|----------|-------------|
| 接收方式 | DMA + 硬件空闲中断 | 逐字节 RX 中断 + 软件状态机 |
| 帧检测 | 在回调中查 rxBuf[0] | 显式 Status 状态机 (0=Idle / 1=HEX / 2=ABC) |
| 错误恢复 | 依赖硬件 IDLE | 错误中断 + FIFO 清空 + 状态机自动复位 |
| HEX 校验 | 无校验（2字节/字） | XOR 校验（3字节/字） |
| 发送 | DMA 环形缓冲 | DMA（有则用）+ 轮询（fallback） |

---

## 2. 硬件配置

### UART 实例

| 实例 | UART | TX | RX | 波特率 | 用途 | 接收模式 |
|------|------|----|----|--------|------|---------|
| Serial1 | UART_0 | PA10 | PA11 | 115200 | USB 调试口 | 中断 |
| Serial2 | UART_1 | PA8 | PA9 | 115200 | 树莓派通信 | 中断 |
| Serial3 | UART_2 | PB15 | PB16 | 115200 | 蓝牙双车通信 | 中断 |
| Serial4 | UART_4 | PA26 | PA25 | 115200 | 串口屏 | 中断 |

> **注意**：PA10/PA11 是 XDS110 调试器的虚拟串口引脚，**不可更改**。

### 使能控制

在 `Serial_porting.h` 中通过宏控制每个串口实例：

```c
#define Serial1_Enable      // 启用 UART_0
#define Serial2_Enable      // 启用 UART_1
//#define Serial3_Enable    // 注释 = 禁用，不编译、不占 RAM
//#define Serial4_Enable
```

---

## 3. 文件结构

```
Serial/
├── README.md              ← 本文件
├── Serial_base.h          协议层头文件：帧常量、错误码、数据结构定义
├── Serial_base.c          协议常量初始化（跨平台不变）
├── Serial_porting.h       适配层头文件：Serial_Typedef 结构体、所有 API 声明
├── Serial_porting.c       适配层实现：ISR、状态机、printf、DMA、解析
├── Serial_Drive.h         已废弃（旧驱动占位头文件）
└── Serial_Drive.c         已废弃（旧驱动占位实现）
```

### 依赖关系

```
ti_msp_dl_config.h (SysConfig)
  ← Serial_porting.h → Serial_base.h
    ← AllHeader.h
      ← 所有 Mode_*.c
```

`#include "Serial_porting.h"` 即获得全部串口 API 和实例（Serial1/Serial2/…）。

---

## 4. 协议格式

### 4.1 HEX 协议（带 XOR 校验）

用于**可靠数据传输**（传感器数据、控制指令等）。每字（int16_t）3 字节传输，含 XOR 校验码。

```
帧格式（定长 LEN 字段）：

 Byte 0    1    2    3       4       5       6       7       8     ...   N-2   N-1
┌──────┬──────┬─────┬───────┬───────┬───────┬───────┬───────┬───────┬─────┬──────┬──────┐
│ 0xFF │ 0xAA │ LEN │ D1_H  │ D1_L  │ D1_CK │ D2_H  │ D2_L  │ D2_CK │ ... │ 0x55 │ 0xFE │
└──────┴──────┴─────┴───────┴───────┴───────┴───────┴───────┴───────┴─────┴──────┴──────┘
 帧头1  帧头2  字数  高位    低位    校验    高位    低位    校验           帧尾1  帧尾2
                 (1B)  (1B)   (1B)   (1B)    (1B)   (1B)   (1B)           (1B)  (1B)

字段说明：
  LEN      — int16_t 数据个数（1 字节），有效范围 0~Serial_RX_MAX_WORDS(256)
  Dx_H/L   — 第 x 个数据的高/低 8 位
  Dx_CK    — 校验码 = Dx_H ⊕ Dx_L
  LEN=0 时，帧仅含帧头+帧尾：[0xFF][0xAA][0x00][0x55][0xFE]

帧总长 = 5 + LEN × 3 字节
有效载荷 = LEN × sizeof(int16_t) 字节（0~512 字节）
```

#### 发送示例

```c
// 发送 3 个 int16_t 数据
uint16_t data[3] = { 0x1234, 0x5678, 0x9ABC };
Serial_Send_HEX_Package(&Serial2, data, 3);
// 实际发送字节: FF AA 03 12 34 26 56 78 2E 9A BC 26 55 FE
//              (0x12^0x34=0x26, 0x56^0x78=0x2E, 0x9A^0xBC=0x26)
```

#### 接收示例

```c
uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
if (has_hex) {
    uint8_t len = Serial_GetHexLen(&Serial2);          // 数据个数
    int16_t d0 = Serial_GetHexData(&Serial2, 0);       // 第0个字
    int16_t d1 = Serial_GetHexData(&Serial2, 1);       // 第1个字
}
```

### 4.2 ABC 协议（文本帧）

用于**调试/调参/简单指令**。ASCII 可打印字符，兼容 Vofa+/串口助手等工具。

```
帧格式：
  Byte 0    1 ... N    N+1   N+2
┌──────┬────────────┬──────┬──────┐
│  '@' │  DATA...   │  '$' │  '#' │
└──────┴────────────┴──────┴──────┘
 帧头   有效载荷      帧尾1  帧尾2

DATA 为 ASCII 可打印字符串
最大有效载荷长度 = Serial_ABC_BUF_SIZE - 1 = 39 字节（不含帧头帧尾和 '\0'）
```

#### 发送示例

```c
Serial_printf(&Serial2, "@Kp=12.5,Kd=3.2$#\r\n");
// 发送端仅需以 @ 开头、$# 结尾即可
```

#### 接收示例

```c
uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);
if (has_abc) {
    // 方式1：子串匹配
    if (Serial_Check_Str(&Serial2, "Kp")) {
        float kp;
        Serial_SetFloatData(&Serial2, "Kp", "Kp=%f", &kp);
    }

    // 方式2：精确匹配（命令解析）
    if (Serial_CheckCmd(&Serial2, "@RESET")) {
        // 执行复位操作
    }

    // 方式3：整数提取
    int id;
    Serial_SetIntData(&Serial2, "ID", "ID=%d", &id);
}
```

### 协议选择建议

| 场景 | 推荐协议 |
|------|---------|
| 传感器数据、编码器数值 | HEX |
| PID 参数调试、在线调参 | ABC |
| 双车关键指令（启动/停止/速度） | HEX |
| 串口屏交互 | ABC |
| 大批量数据传输 (>40 字节/帧) | HEX（ABC 限制 39 字节有效载荷） |

---

## 5. 接收状态机

### 5.1 状态转换图

```
                     ┌──────────────────────────────┐
                     │                              │
                     ▼                              │
              ┌──────────┐                          │
     ┌───────→│  Idle(0) │←─────────────────────┐   │
     │        └────┬─────┘                        │   │
     │       0xFF  │  '@'                         │   │
     │        ┌────▼─────┐                        │   │
     │        │  HEX(1)  │                        │   │
     │        └────┬─────┘                        │   │
     │      0xFE   │ LEN非法/溢出/超时              │   │
     │        ┌────▼─────┐                        │   │
     │        │ 回调解析  │                        │   │
     │        └──────────┘                        │   │
     │                                            │   │
     │        ┌──────────┐                        │   │
     │        │  ABC(2)  │────────────────────────┘   │
     │        └────┬─────┘    '#' / 溢出               │
     │          '#' │                                  │
     │        ┌────▼─────┐                             │
     │        │ 回调解析  │                             │
     │        └──────────┘                             │
     │  任何完成/错误自动回到 Idle                       │
     └─────────────────────────────────────────────────┘
```

**状态定义**（`Serial_Typedef.Status`）：

| 值 | 状态 | 含义 |
|----|------|------|
| 0 | Idle | 空闲，等待帧头字节 (0xFF 或 '@') |
| 1 | HEX | 正在接收 HEX 帧，等待帧尾 0xFE |
| 2 | ABC | 正在接收 ABC 帧，等待帧尾 '#' |

**错误恢复**：任何错误（LEN 非法、帧尾不匹配、溢出、超时）都会**自动复位到 Idle**，不会锁死。

**HEX LEN 早期溢出检测**：收到 rxBuf[2]（LEN 字段）后，计算预期帧长 `expected = 5 + LEN×3`。后续每字节检查是否超出预期，实现快速错误检测（而非等到 773 字节缓冲区满）。

### 5.2 ISR 调度流程

```
UART_x_INST_IRQHandler()                    ← 中断入口（启动代码绑定）
  └─ Serial_RxISR_Dispatch(&SerialX)        ← 统一分发
       ├─ DL_UART_MAIN_IIDX_RX              ← 中断模式：读 FIFO → rx_temp
       ├─ DL_UART_MAIN_IIDX_DMA_DONE_RX     ← DMA模式：DMA已搬运到 rx_temp
       ├─ DL_UART_MAIN_IIDX_DMA_DONE_TX     ← TX DMA 完成 → 环形缓冲推进
       ├─ DL_UART_MAIN_IIDX_RX_TIMEOUT_ERROR← 空闲超时 → 清空 FIFO
       ├─ DL_UART_MAIN_IIDX_OVERRUN_ERROR   ← 错误中断 → 读丢弃字节
       ├─ DL_UART_MAIN_IIDX_FRAMING_ERROR
       ├─ DL_UART_MAIN_IIDX_BREAK_ERROR
       ├─ DL_UART_MAIN_IIDX_PARITY_ERROR
       └─ DL_UART_MAIN_IIDX_NOISE_ERROR
            └─ Serial_Rx_ProcessByte()      ← 状态机 + 协议解析
                 ├─ Serial_Rx_State_Check() ← 逐字节状态机（Idle→HEX/ABC）
                 ├─ Serial_Parse_HEX()      ← HEX 帧校验+解析（ISR内执行）
                 └─ Serial_Parse_ABC()      ← ABC 帧校验+解析（ISR内执行）
```

> **注意**：`Serial_Parse_HEX/ABC` 在 ISR 上下文中执行。对于大帧（LEN≈100 字），解析耗时约 40μs（Cortex-M0+ @80MHz）。这是可接受的，但不要向解析函数增加更多逻辑。

---

## 6. API 完整参考

### 6.1 初始化

```c
void Serial_Init(void);
```

- **何时调用**：`Initial_All()` 中，在 NVIC 全局使能之前
- **作用**：初始化协议常量 + 逐个调用 `Serial_Initial()` 配置硬件
- **自动开启的中断**：
  - 非 DMA 模式：`RX | RX_TIMEOUT_ERROR | OVERRUN_ERROR | FRAMING_ERROR | BREAK_ERROR`
  - DMA 模式：`DMA_DONE_TX | OVERRUN_ERROR | FRAMING_ERROR | BREAK_ERROR`

### 6.2 发送 API

#### Serial_printf — 格式化发送

```c
void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...);
```

- **DMA 模式**：格式化 → 写入环形缓冲 → 触发 DMA 发送（非阻塞）
- **中断模式**（dma==NULL）：格式化 → 轮询逐字节发送（**阻塞！耗时与字符串长度成正比**）
- **缓冲区限制**：格式化缓冲区 128 字节，超长自动截断

```c
Serial_printf(&Serial1, "Hello World\r\n");
Serial_printf(&Serial1, "Encoder A: %d, B: %d\r\n", cntA, cntB);
Serial_printf(&Serial2, "@Kp=%.2f,Kd=%.2f$#\r\n", 12.5f, 3.2f);
```

> **提醒**：中断模式下的 `Serial_printf` 是阻塞的，会占用主循环时间。高频率日志建议用 DMA 模式或减小输出量。

#### Serial_send_string — 轮询发字符串

```c
void Serial_send_string(Serial_Typedef *pSerial, char *str);
```

- 逐字符轮询发送，直到遇到 `'\0'`
- 适用于 DMA 不可用时发送短消息

#### Serial_SendBytes — 发送原始字节

```c
void Serial_SendBytes(Serial_Typedef *pSerial, uint8_t *buf, uint16_t len);
```

- 逐字节轮询发送，不关心内容
- 适用于发送二进制数据

#### Serial_Send_HEX_Package — 发送 HEX 帧

```c
void Serial_Send_HEX_Package(Serial_Typedef *pSerial, uint16_t *data, uint8_t count);
```

- 自动拼装帧头 (0xFF 0xAA) + LEN + 数据 (每字3字节含XOR校验) + 帧尾 (0x55 0xFE)
- count 上限 256

```c
uint16_t encoder_data[4] = { 100, 200, 300, 400 };
Serial_Send_HEX_Package(&Serial2, encoder_data, 4);    // 发送 4 个字的 HEX 帧
```

### 6.3 HEX 接收 API

#### Serial_GetNewPackageFlag_HEX — 获取新帧标志（★ 每轮只调一次）

```c
uint8_t Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial);
```

- **"读且清零"** 操作，类似于信号量 Take
- 返回 1 表示有新帧，返回 0 表示没有
- **⚠️ 同一轮 Loop 中多次调用会竞争消费，第二次永远抓不到！** 必须用局部变量存结果：

```c
// ✅ 正确
uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
if (has_hex) { /* 处理 */ cnt++; }

// ❌ 错误：第二次调用时 flag 已被清零
if (Serial_GetNewPackageFlag_HEX(&Serial2)) { /* 处理A */ }
if (Serial_GetNewPackageFlag_HEX(&Serial2)) cnt++;   // 几乎永远 false
```

#### Serial_GetHexData — 读取第 index 个字

```c
int16_t Serial_GetHexData(Serial_Typedef *pSerial, uint8_t index);
```

- index 越界返回 0
- 校验失败的字保留上一次的值（不更新）

#### Serial_GetHexLen — 获取帧中数据个数

```c
uint8_t Serial_GetHexLen(Serial_Typedef *pSerial);
```

- 返回值与帧中 LEN 字段一致

#### Serial_GetError_HEX — 获取 HEX 错误码

```c
int Serial_GetError_HEX(Serial_Typedef *pSerial);
```

- 返回值见 [附录 B：错误码速查](#附录-b错误码速查)

### 6.4 ABC 接收 API

#### Serial_GetNewPackageFlag_ABC — 获取新帧标志（★ 每轮只调一次）

```c
uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial);
```

- 与 HEX 版本同样的"读清零"语义，同样每轮只调一次

#### Serial_Check_Str — 子串匹配

```c
bool Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord);
```

- 使用 `strstr` 实现，检查 KeyWord 是否为帧内容的子串
- 适用于检测帧中是否包含某个字段名

```c
if (Serial_Check_Str(&Serial2, "Kp")) { /* 帧中包含 Kp */ }
```

#### Serial_CheckCmd — 精确匹配

```c
bool Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd);
```

- 使用 `strcmp` 实现，检查帧内容是否完全等于 cmd
- 适用于命令解析（精确指令匹配）

```c
if (Serial_CheckCmd(&Serial2, "@RESET")) { /* 收到复位指令 */ }
```

#### Serial_SetFloatData — 提取浮点数

```c
bool Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data);
```

- KeyWord 匹配 → `sscanf(cmd, ...)` 解析
- Vofa+ 示例：发送 `@Kp=12.50$#` → 接收端 `Serial_SetFloatData(&Serial2, "Kp", "Kp=%f", &kp_var)`

#### Serial_SetIntData — 提取整数

```c
bool Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data);
```

- 同上，解析整数

#### Serial_GetError_ABC — 获取 ABC 错误码

```c
int Serial_GetError_ABC(Serial_Typedef *pSerial);
```

### 6.5 调试 API

#### Serial_PrintDebug — 打印调试统计

```c
#ifdef Serial_Debug
void Serial_PrintDebug(Serial_Typedef *pSerial);
#endif
```

- 仅在 `#define Serial_Debug` 时编译
- 输出示例和字段含义见 [第 7 节](#7-调试打印说明)

---

## 7. 调试打印说明

### 启用方式

```c
// Serial_porting.h
#define Serial_Debug    // 启用。注释掉则关闭，dbg 字段不占 RAM
```

启用后，每个 `Serial_Typedef` 实例额外包含 8 个 `uint32_t` 计数器（32 字节），并在 `Serial_PrintDebug()` 中可输出。

### 输出示例及字段含义

```
=== Serial Debug ===
  RX Bytes:    15250      ← 总接收字节数（含帧头帧尾等所有协议开销）
  Frames Det:  50         ← 状态机检测到完整帧边界的次数（HEX+ABC合计，含后续校验失败的帧）
  Parse OK:    48         ← 真正校验通过的有效帧数（HEX:XOR全对 / ABC:帧头帧尾全对）
  Lost:        2          ← ★ 丢帧数：新帧到达时旧帧尚未被主循环消费（GetNewPackageFlag 未调用）
  Err Head:    0          ← 帧头校验失败次数（HEX头≠0xFF 0xAA 或 ABC头≠'@'）
  Err Tail:    2          ← 帧尾校验失败次数（HEX尾≠0x55 0xFE 或 ABC尾≠'$' '#'）
  Overflow:    0          ← 协议层溢出（帧长超过预期/非法LEN/缓冲区满）
  HW Errors:   143        ← 硬件层错误（OVERRUN/FRAMING/BREAK/NOISE/PARITY）
==================
```

### 健康状态判断

| 指标 | 健康 | 警告 | 说明 |
|------|------|------|------|
| `Lost` | =0 | >0 | 主循环来不及消费！加快 tick 频率或改用 DMA |
| `Frames Det - Parse OK` | ≤1 | >5 | 通信质量差，帧受损率高 |
| `Err Head + Err Tail` | ≈0 | >Parse OK 的 10% | 线路噪声大，检查接线/共地 |
| `Overflow` | =0 | >0 | 帧长超过限制，检查发送端或增大 `Serial_RX_MAX_WORDS` |
| `HW Errors` | <100 | >1000 | 可能 USB 频繁插拔/波特率不匹配/长线缆噪声 |

> **`HW Errors` 在 USB 插拔瞬间出现几十次是正常的**。如果持续快速增长才需要排查。

---

## 8. 移植指南

### 8.1 新增串口实例

假设需要添加 UART_3（PB19=TX, PB18=RX）作为 Serial5：

**Step 1：Serial_porting.h — 添加使能宏 + 外部声明**

```c
// 在现有使能宏附近添加
#define Serial5_Enable      // UART_3 (TX=PB19, RX=PB18)

// 在现有外部声明附近添加
#ifdef Serial5_Enable
extern Serial_Typedef Serial5;
#endif
```

**Step 2：Serial_porting.c — 添加实例定义 + 初始化调用**

```c
// 在全局实例区添加
#ifdef Serial5_Enable
Serial_Typedef Serial5;
#endif

// 在 Serial_Init() 中添加
#ifdef Serial5_Enable
    Serial_Initial(&Serial5, UART_3_INST, NULL, 0, UART_3_INST_INT_IRQN,
                   NULL, 0);
#endif
```

**Step 3：Serial_porting.c — 添加 ISR 入口**

```c
void UART_3_INST_IRQHandler(void)
{
#ifdef Serial5_Enable
    Serial_RxISR_Dispatch(&Serial5);
#endif
}
```

**Step 4：SysConfig 中配置 UART_3**：设置引脚、波特率、启用接收。确保 `ti_msp_dl_config.h` 中生成 `UART_3_INST` 等宏。

**完成**。无需修改状态机或协议层。

### 8.2 移植到其他 MCU

**保持不变（直接复制）**：
- `Serial_base.h` — 纯 C 数据结构，无 MCU 依赖
- `Serial_base.c` — 协议常量初始化
- 协议解析函数 `Serial_Parse_HEX/ABC` 的核心逻辑（CRC/XOR 计算部分）

**需要重写（MCU 相关）**：
- `Serial_Initial()` — UART/DMA/NVIC 初始化
- `uart_send_char()` — 单字节阻塞发送
- `Serial_Rx_State_Check()` 中 `rx_temp` 的赋值方式（中断读寄存器 vs DMA 自动搬运）
- 所有 `DL_UART_*` / `DL_DMA_*` / `NVIC_*` 调用 → 替换为目标 MCU 的 HAL
- 中断使能标志位 → 替换为目标 MCU 的对应宏
- `Serial_printf` 中 DMA 环形缓冲部分 → 替换为目标 MCU 的 DMA API
- 所有 `UART_x_INST_IRQHandler` → 替换为目标 MCU 的中断向量名

**典型移植工作量**：约 **200 行**适配层代码需要重写，协议层零改动。

### 8.3 SysConfig 注意事项

| 陷阱 | 症状 | 解决 |
|------|------|------|
| UART 曾配置 DMA RX，后改为中断 | 数据能发不能收 | `DL_UART_Main_disableDMAReceiveEvent(uart, DL_UART_DMA_INTERRUPT_RX)` + 重新生成 |
| Ctrl+B 编译 ≠ 重新生成 | 改了 .syscfg 但无效 | Ctrl+S 在 .syscfg 编辑器中保存才触发代码生成 |
| 未启用错误中断 | 噪声导致 FIFO 锁死 | 当前代码已在 `Serial_Initial` 中自动启用，无需手动配置 |

---

## 9. 常见问题与注意事项

### ⚠️ 规则 1：每轮只调一次 GetNewPackageFlag

```c
// ✅ 正确
uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
if (has_hex) { ... }

// ❌ 错误：多次调用竞争消费
if (Serial_GetNewPackageFlag_HEX(&Serial2)) { ... }
if (Serial_GetNewPackageFlag_HEX(&Serial2)) cnt++;  // 永远抓不到
```

### ⚠️ 规则 2：ABC 帧有效载荷 ≤ 39 字节

`Serial_ABC_BUF_SIZE = 40`（含 `'\0'` 结尾符）。如果发送端发送超过 39 字节有效载荷的 ABC 帧，会被截断。
对于大块数据，**必须使用 HEX 协议**。

### ⚠️ 规则 3：中断模式下 Serial_printf 是阻塞的

非 DMA 模式（当前所有实例）下，`Serial_printf` 逐字符等待 TX 完成。发送长字符串（如调试打印）会阻塞主循环。建议：
- 调试打印放在低频操作中（如 Key 长按触发）
- 高频日志减少字符串长度
- 考虑使用 DMA 模式实现非阻塞发送

### ⚠️ 规则 4：rxBuf 是单缓冲，存在覆盖风险

当前 `rxBuf` 只有一个。如果主循环消费速度 < 帧到达速度，新帧数据会覆盖旧帧。使用 `Lost` 计数器监控此情况。

### ⚠️ 规则 5：解析函数在 ISR 内执行

`Serial_Parse_HEX/ABC` 在 ISR 上下文中执行。对于大帧（LEN≈100 字），耗时约 40μs。MSPM0 UART 有 8 字节 FIFO（694μs 缓冲），当前安全。但**不要向解析函数增加耗时操作**（如 printf 调试）。

### 常见坑点速查

| 现象 | 可能原因 | 排查步骤 |
|------|---------|---------|
| 能发不能收 | DMA 残留 | 检查 `Serial_Initial` 中是否调用了 `disableDMAReceiveEvent` |
| 收到数据但 Parse OK=0 | 协议不匹配 | 检查发送端帧格式是否符合 HEX (0xFF 0xAA ... 0x55 0xFE) 或 ABC (@...$#) |
| Lost 持续增长 | 主循环太慢 | 加快 tick 频率、减少 printf 调用、或切换 DMA 模式 |
| HW Errors 快速增长 | 线路噪声/波特率 | 检查接线、共地、波特率设置 |
| 编译报错 UART_x_INST 未定义 | SysConfig 未配置 | 在 .syscfg 中添加对应 UART 并保存生成 |

---

## 10. 高级主题

### 10.1 中断模式 vs DMA 模式

| 维度 | 中断模式（当前默认） | DMA 模式 |
|------|-------------------|---------|
| 中断频率 | 每字节一次 | 每传输完成一次 |
| CPU 占用 | 逐字节处理 | 几乎零占用 |
| 实现复杂度 | 低 | 需要配置 DMA 通道+TX 环形缓冲 |
| 适用场景 | 低频通信（<10KB/s） | 高频通信（≥10KB/s） |
| 当前实例 | Serial1/2/3/4 均用此模式 | 当前仅框架支持，未启用 |

**切换到 DMA 模式**：在 `Serial_Init()` 中将对应实例的第一个 `NULL` 替换为实际的 DMA 基址和通道号。

```c
// 中断模式（当前）
Serial_Initial(&Serial1, UART_0_INST, NULL, 0, UART_0_INST_INT_IRQN, NULL, 0);

// DMA 模式（需先在 SysConfig 中配置 DMA 通道）
Serial_Initial(&Serial1, UART_0_INST, DMA, DMA_UART_0_RX_Channel_CHAN_ID,
               UART_0_INST_INT_IRQN, DMA, DMA_UART_0_TX_Channel_CHAN_ID);
```

### 10.2 多中断环境优先级

当前项目中断源（按**建议优先级**从高到低）：

| 优先级 | 中断源 | 周期/触发 | 最晚响应时限 | ISR 耗时要求 |
|--------|--------|---------|-------------|------------|
| **最高** | TIMER_0 | 1ms | <100μs | <50μs |
| **高** | UART_0/1/4 | 异步（87μs/字节） | <694μs（FIFO 8字节） | 每字节 <10μs，帧尾解析 <200μs |
| **中** | GROUP1 (编码器) | 异步（电机脉冲） | 脉冲间隔 > ISR 耗时即可 | <10μs |
| **低** | TIMER_1 | 20ms | <1ms | 不限（但不能阻塞更高优先级） |

**关键原则**：
- TIMER_0 ISR 必须极短（<50μs），给 UART 留足 FIFO 缓冲时间
- 不要在 UART ISR 内加 printf 或其他耗时操作
- NVIC 优先级值越小 = 优先级越高（Cortex-M 标准）

### 10.3 高吞吐量场景

对于 **50 帧 × 300 字节** 级别的持续数据流（~15KB/1.3 秒 @115200bps）：

**当前架构已够用**，但需注意：

1. **监控 `Lost` 计数器**：如果 > 0，主循环跟不上了
2. **提高消费频率**：主线 20ms Tick → 改为 10ms 或更短
3. **升级方案**（如果 Lost 持续增长）：
   - **方案 A**：改用 DMA 接收（降低中断频率）
   - **方案 B**：双缓冲 `rxBuf[2][Serial_RX_BUF_SIZE]` + 乒乓切换 + 解析移到主循环
   - **方案 C**：RX 使用 DMA 双缓冲模式（DMA 自动乒乓）+ ISR 仅设标志位

---

## 11. 实战示例代码

> 以下所有示例均可直接复制到项目中编译运行。前提：已 `#include "AllHeader.h"` 并在 `Initial_All()` 中调用过 `Serial_Init()`。

### 示例 1：最小初始化（从头新建项目）

**场景**：一个全新 CCS 项目，只启用 Serial1 做调试打印。

**Step 1 — SysConfig**：在 `.syscfg` 中添加 UART_0，配置 TX=PA10、RX=PA11、115200 8N1，勾选 "Receive"。

**Step 2 — Serial_porting.h**：确保至少启用 Serial1。

```c
#define Serial1_Enable      // UART_0 (TX=PA10, RX=PA11)
//#define Serial2_Enable    // 暂时不需要，注释掉省 RAM
//#define Serial_Debug      // 调试计数器，初期先打开
```

**Step 3 — AllHeader.h**：引入串口头文件。

```c
#include "Serial_porting.h"
```

**Step 4 — AllHeader.c**：在初始化函数中调用。

```c
#include "AllHeader.h"

void Initial_All(void)
{
    // ... 其他初始化（GPIO、OLED 等）...

    Serial_Init();          // ★ 串口初始化（必须在 NVIC 全局使能之前）

    // __enable_irq();      // 全局使能中断（通常由 main 调用）
}
```

**Step 5 — main.c 或 Mode 文件**：发送第一条消息。

```c
void Mode_X_Setup(void)
{
    Serial_printf(&Serial1, "System Ready.\r\n");
    Serial_printf(&Serial1, "Firmware v1.0, Freq=%d MHz\r\n", 80);
}
```

**验证**：用串口助手打开 COM 口（115200 8N1），应看到 `System Ready.`。

---

### 示例 2：调试日志打印（Serial1 做 printf）

**场景**：在开发阶段用 Serial1 打印变量、状态机切换、错误信息。

```c
// ---- 带时间戳的日志宏 ----
#ifdef Serial_Debug
#define DEBUG_LOG(fmt, ...) \
    Serial_printf(&Serial1, "[%dms] " fmt "\r\n", g_sys_tick, ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...)  ((void)0)
#endif

// ---- 使用 ----
void Mode_1_Loop(void)
{
    static int last_encoder_a = 0;
    int enc_a = MyEncoder_Get_CNT(&Motor_A_Encoder);

    if (enc_a != last_encoder_a)
    {
        DEBUG_LOG("EncA changed: %d → %d", last_encoder_a, enc_a);
        last_encoder_a = enc_a;
    }
}
```

> **性能提示**：`DEBUG_LOG` 宏在非 debug 编译时完全不产生代码。调试时建议通过 Key 长按触发打印，避免每轮都输出。

---

### 示例 3：编码器数据上报（HEX 协议，一对多）

**场景**：将两路编码器 + MPU6050 Yaw 角通过 Serial2 上报给树莓派，每 100ms 发一次。

**发送端（MSPM0）**：

```c
void Mode_2_Setup(void)
{
    Serial_printf(&Serial2, "Encoder Report Start.\r\n");
}

void Mode_2_Loop(void)
{
    static uint32_t last_send = 0;

    // 每 100ms 发一帧
    if (g_sys_tick - last_send >= 100)
    {
        last_send = g_sys_tick;

        uint16_t data[3];
        data[0] = (uint16_t)(int16_t)MyEncoder_Get_CNT(&Motor_A_Encoder);  // 编码器A
        data[1] = (uint16_t)(int16_t)MyEncoder_Get_CNT(&Motor_B_Encoder);  // 编码器B
        data[2] = (uint16_t)(int16_t)MPU6050_Get_Yaw();                    // Yaw角

        Serial_Send_HEX_Package(&Serial2, data, 3);
    }
}
```

**接收端（树莓派 Python）**：

```python
# 帧格式：FF AA 03 D0_H D0_L D0_CK D1_H D1_L D1_CK D2_H D2_L D2_CK 55 FE
# 有效载荷：3 个 int16_t（编码器A, 编码器B, Yaw角）

def parse_hex_frame(data):
    if len(data) < 7 or data[0] != 0xFF or data[1] != 0xAA:
        return None
    LEN = data[2]
    words = []
    for i in range(LEN):
        dh = data[3 + i*3]
        dl = data[3 + i*3 + 1]
        ck = data[3 + i*3 + 2]
        val = (dh << 8) | dl
        if (dh ^ dl) == ck:     # XOR 校验通过
            words.append(val - 65536 if val > 32767 else val)  # int16
    return words

# 使用
frame = parse_hex_frame(serial_bytes)
if frame:
    print(f"EncA={frame[0]}, EncB={frame[1]}, Yaw={frame[2]/10.0}°")
```

---

### 示例 4：PID 在线调参（ABC 协议，Vofa+ 配合）

**场景**：用 Vofa+ 或串口助手发送 ABC 帧实时调节 PID 参数，MSPM0 解析后立即生效。

**MSPM0 端**：

```c
// 全局 PID 参数
float g_kp = 5.0f, g_ki = 0.1f, g_kd = 2.0f;
int   g_target_speed = 500;    // 目标速度（编码器脉冲/秒）

void Mode_2_Loop(void)
{
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);
    if (!has_abc) return;

    // ---- 逐个字段匹配（顺序无关）----
    if (Serial_Check_Str(&Serial2, "Kp"))
    {
        float new_kp;
        if (Serial_SetFloatData(&Serial2, "Kp", "Kp=%f", &new_kp))
        {
            g_kp = new_kp;
            Serial_printf(&Serial2, "@Kp=%.2f,OK$#\r\n", g_kp);  // 回传确认
        }
    }

    if (Serial_Check_Str(&Serial2, "Ki"))
    {
        float new_ki;
        if (Serial_SetFloatData(&Serial2, "Ki", "Ki=%f", &new_ki))
            g_ki = new_ki;
    }

    if (Serial_Check_Str(&Serial2, "Kd"))
    {
        float new_kd;
        if (Serial_SetFloatData(&Serial2, "Kd", "Kd=%f", &new_kd))
            g_kd = new_kd;
    }

    // ---- 整数参数 ----
    if (Serial_Check_Str(&Serial2, "Speed"))
    {
        int new_speed;
        // Vofa+ 只能用浮点滑块 → 用 %.0f 模拟整数
        if (Serial_SetIntData(&Serial2, "Speed", "Speed=%d", &new_speed))
            g_target_speed = new_speed;
    }

    // ---- 精确命令 ----
    if (Serial_CheckCmd(&Serial2, "@GET_PARAMS"))
    {
        Serial_printf(&Serial2,
            "@Kp=%.2f,Ki=%.2f,Kd=%.2f,Speed=%d$#\r\n",
            g_kp, g_ki, g_kd, g_target_speed);
    }
}
```

**Vofa+ 端配置**：

```
发送按钮1:  @Kp=8.00$#
发送按钮2:  @Ki=0.20$#
发送按钮3:  @Kd=3.00$#
发送按钮4:  @Speed=600$#
发送按钮5:  @GET_PARAMS$#
```

---

### 示例 5：双协议混合（同一串口同时收 HEX 和 ABC）

**场景**：Serial2 同时用 ABC 收调参指令、用 HEX 收传感器数据。

```c
void Mode_2_Loop(void)
{
    // ★ 每个协议只调一次，存结果
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);

    // ---- 处理 HEX（传感器数据）----
    if (has_hex)
    {
        uint8_t len = Serial_GetHexLen(&Serial2);
        for (uint8_t i = 0; i < len; i++)
        {
            int16_t val = Serial_GetHexData(&Serial2, i);
            // 存入环形缓冲区供后续处理...
        }
    }

    // ---- 处理 ABC（调参指令）----
    if (has_abc)
    {
        if (Serial_Check_Str(&Serial2, "Kp"))
        {
            float kp;
            Serial_SetFloatData(&Serial2, "Kp", "Kp=%f", &kp);
            // 更新 PID...
        }
    }
}
```

> **关键**：`GetNewPackageFlag_HEX` 和 `GetNewPackageFlag_ABC` 操作的是**不同**的标志位（`HEX_Data.frame_valid` 和 `ABC_Data.Serial_New_Package_Flag`），互不干扰。同一轮中如果 HEX 和 ABC 帧都到达，两者都能被消费。

---

### 示例 6：树莓派指令通信（命令 → 解析 → 应答）

**场景**：树莓派通过 Serial2 发 ASC 指令控制小车执行动作。

**通信协议定义**：

| 指令 | 格式 | 含义 |
|------|------|------|
| 直行 | `@GO:SPEED$#` | 以指定速度直行 |
| 转弯 | `@TURN:ANGLE$#` | 转指定角度（度） |
| 停止 | `@STOP$#` | 立即停车 |
| 询问状态 | `@STATUS$#` | 回复当前状态 |
| HEX 数据下发 | HEX 帧（LEN=4） | [目标速度][目标角度][0][0] |

**MSPM0 完整实现**：

```c
typedef enum { IDLE, RUNNING, TURNING, STOPPED } Car_State;
static Car_State g_state = IDLE;
static int g_cmd_speed = 0, g_cmd_angle = 0;

void Mode_2_Setup(void)
{
    Serial_printf(&Serial2, "@READY$#\r\n");
}

void Mode_2_Loop(void)
{
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);

    // ===== 方式1：HEX 指令（可靠，用于关键数据）=====
    if (has_hex)
    {
        g_cmd_speed = Serial_GetHexData(&Serial2, 0);
        g_cmd_angle = Serial_GetHexData(&Serial2, 1);

        if (g_cmd_speed == 0 && g_cmd_angle == 0)
            g_state = STOPPED;
        else if (g_cmd_angle == 0)
            g_state = RUNNING;
        else
            g_state = TURNING;

        // HEX 应答也用 HEX 帧，包含 XOR 校验
        uint16_t ack[1] = { (uint16_t)g_state };
        Serial_Send_HEX_Package(&Serial2, ack, 1);
    }

    // ===== 方式2：ABC 指令（可读，用于调试/低频命令）=====
    if (has_abc)
    {
        if (Serial_Check_Str(&Serial2, "GO"))
        {
            Serial_SetIntData(&Serial2, "GO", "GO:%d", &g_cmd_speed);
            g_state = RUNNING;
            g_cmd_angle = 0;
            Serial_printf(&Serial2, "@GO_ACK:SPEED=%d$#\r\n", g_cmd_speed);
        }
        else if (Serial_Check_Str(&Serial2, "TURN"))
        {
            Serial_SetIntData(&Serial2, "TURN", "TURN:%d", &g_cmd_angle);
            g_state = TURNING;
            Serial_printf(&Serial2, "@TURN_ACK:ANGLE=%d$#\r\n", g_cmd_angle);
        }
        else if (Serial_CheckCmd(&Serial2, "@STOP"))
        {
            g_state = STOPPED;
            g_cmd_speed = 0;
            g_cmd_angle = 0;
            Serial_printf(&Serial2, "@STOP_ACK$#\r\n");
        }
        else if (Serial_CheckCmd(&Serial2, "@STATUS"))
        {
            Serial_printf(&Serial2,
                "@STATE=%d,SPEED=%d,ANGLE=%d$#\r\n",
                (int)g_state, g_cmd_speed, g_cmd_angle);
        }
    }

    // ===== 执行动作（根据全局状态）=====
    switch (g_state)
    {
        case RUNNING:
            // Motor_SetSpeed(g_cmd_speed);
            break;
        case TURNING:
            // Stepper_Turn(g_cmd_angle);
            break;
        case STOPPED:
            // Motor_Stop();
            break;
        default: break;
    }
}
```

---

### 示例 7：完整 Mode 生命周期（Setup → Loop → Tick → Exit）

**场景**：新建 Mode_5，展示标准四函数生命周期中串口的使用。

```c
// ======================= Mode_5.h =======================//
#ifndef __MODE_5_H
#define __MODE_5_H
void Mode_5_Setup(void);
void Mode_5_Loop(void);
void Mode_5_Tick(void);    // 每 20ms 调用一次
void Mode_5_Exit(void);
#endif

// ======================= Mode_5.c =======================//
#include "Mode_5.h"
#include "AllHeader.h"

// 局部变量
static float  g_kp, g_ki, g_kd;
static uint32_t g_last_hex_report = 0;
static uint32_t g_package_count  = 0;

// ── Setup：进入模式时执行一次 ──
void Mode_5_Setup(void)
{
    // 初始参数
    g_kp = 5.0f;  g_ki = 0.1f;  g_kd = 2.0f;

    Serial_printf(&Serial2, "@MODE5_READY$#\r\n");
    OLED_Printf(0, 0, OLED_6X8, "Mode5:SerialTest");
}

// ── Loop：主循环，越快越好（被 while(1) 反复调用）──
void Mode_5_Loop(void)
{
    // ★ 必须先存储 flag，再分支处理
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial2);

    // ---- 消费 HEX ----
    if (has_hex)
    {
        g_package_count++;
        uint8_t len = Serial_GetHexLen(&Serial2);

        // 根据 LEN 区分数据类型
        if (len == 3)
        {
            // 编码器上报（示例3）
            int16_t enc_a  = Serial_GetHexData(&Serial2, 0);
            int16_t enc_b  = Serial_GetHexData(&Serial2, 1);
            int16_t yaw    = Serial_GetHexData(&Serial2, 2);
            OLED_Printf(0, 10, OLED_6X8, "A:%d B:%d", enc_a, enc_b);
        }
        else if (len == 1)
        {
            // 单指令字（示例6 应答）
            int16_t cmd = Serial_GetHexData(&Serial2, 0);
            OLED_Printf(0, 20, OLED_6X8, "CMD:%d", cmd);
        }
    }

    // ---- 消费 ABC ----
    if (has_abc)
    {
        if (Serial_Check_Str(&Serial2, "Kp"))
            Serial_SetFloatData(&Serial2, "Kp", "Kp=%f", &g_kp);
        if (Serial_Check_Str(&Serial2, "Ki"))
            Serial_SetFloatData(&Serial2, "Ki", "Ki=%f", &g_ki);
        if (Serial_Check_Str(&Serial2, "Kd"))
            Serial_SetFloatData(&Serial2, "Kd", "Kd=%f", &g_kd);
    }

    // ---- 按键触发调试 ----
    if (Key_Check(KEY_2, KEY_LONG))
        Serial_PrintDebug(&Serial2);
}

// ── Tick：每 20ms 被 TIMER_1 中断调用 ──
void Mode_5_Tick(void)
{
    // 每 500ms 发送一次状态报告
    if (g_sys_tick - g_last_hex_report >= 500)
    {
        g_last_hex_report = g_sys_tick;

        // 用 HEX 发送编码器累计值（可靠）
        uint16_t data[2];
        data[0] = (uint16_t)(int16_t)MyEncoder_Get_Total_CNT(&Motor_A_Encoder);
        data[1] = (uint16_t)(int16_t)MyEncoder_Get_Total_CNT(&Motor_B_Encoder);
        Serial_Send_HEX_Package(&Serial2, data, 2);
    }
}

// ── Exit：退出模式时执行一次 ──
void Mode_5_Exit(void)
{
    Serial_printf(&Serial2, "@MODE5_EXIT$#\r\n");

    // 清空残留帧（可选）
    Serial_GetNewPackageFlag_HEX(&Serial2);
    Serial_GetNewPackageFlag_ABC(&Serial2);
}
```

---

### 示例 8：错误处理（检查错误码并分级响应）

**场景**：在 Loop 中监控串口通信质量，根据错误类型执行不同恢复策略。

```c
void Mode_2_Loop(void)
{
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial2);

    if (has_hex)
    {
        uint8_t len = Serial_GetHexLen(&Serial2);
        for (uint8_t i = 0; i < len; i++)
        {
            int16_t val = Serial_GetHexData(&Serial2, i);
            // 处理数据...
        }
    }
    else
    {
        // 没有新帧时，检查是否有错误发生
        int err = Serial_GetError_HEX(&Serial2);

        switch (err)
        {
            case Serial_Err_None:
                // 一切正常，无操作
                break;

            case Serial_Err_HEX_Head:
                // 帧头异常：可能是噪声触发的假帧头
                // 不影响后续帧，状态机已自动恢复
                break;

            case Serial_Err_HEX_Tail:
                // 帧尾异常：可能是线路噪声或帧被截断
                // 连续发生时考虑降低波特率
                static uint8_t tail_err_count = 0;
                tail_err_count++;
                if (tail_err_count > 10)
                {
                    Serial_printf(&Serial2, "@WARN:TailErr=%d$#\r\n", tail_err_count);
                    tail_err_count = 0;
                }
                break;

            case Serial_Err_HEX_Len_OverFlow:
                // LEN 超限：发送端 bug 或协议不匹配
                Serial_printf(&Serial2, "@FATAL:LEN_Overflow$#\r\n");
                break;

            default:
                break;
        }
    }

    // ABC 同样的错误检查
    int abc_err = Serial_GetError_ABC(&Serial2);
    if (abc_err != Serial_Err_None)
    {
        // ABC 错误通常不影响功能（下一帧会恢复正常）
        // 仅做记录
    }
}
```

---

### 示例 9：调试开关（Key 长按输出统计，一键诊断）

**场景**：在任意 Mode 中用按键触发 `Serial_PrintDebug` 输出通信统计，帮助现场排查问题。

```c
// ---- Mode 中 ----
void Mode_2_Loop(void)
{
    // ... 正常的串口收发 ...

    // Key1 长按：打印 Serial2 调试统计
    if (Key_Check(KEY_1, KEY_LONG))
    {
        Serial_printf(&Serial1, "\r\n");       // 空行分隔
        Serial_PrintDebug(&Serial2);            // ★ 输出到 Serial1（调试口）
    }

    // Key2 长按：清零计数器（重新开始统计）
    if (Key_Check(KEY_2, KEY_LONG))
    {
        Serial2.dbg_rx_bytes    = 0;
        Serial2.dbg_rx_frames   = 0;
        Serial2.dbg_parse_ok    = 0;
        Serial2.dbg_frame_lost  = 0;
        Serial2.dbg_err_head    = 0;
        Serial2.dbg_err_tail    = 0;
        Serial2.dbg_err_overflow = 0;
        Serial2.dbg_err_hardware = 0;
        Serial_printf(&Serial1, "Serial2 counters cleared.\r\n");
    }
}
```

**典型现场排查流程**：

1. 运行小车 1 分钟 → Key1 长按查看统计
2. 如果 `Lost > 0` → 主循环跟不上，加快 tick 频率
3. 如果 `HW Errors > 1000` → 检查接线或换短 USB 线
4. 如果 `Parse OK < Frames Det` → 通信质量差，检查共地
5. Key2 长按清零 → 改变一个参数 → 再测 1 分钟 → 对比

---

### 示例 10：双车通信（Car1 ↔ Car2 蓝牙）

**场景**：两辆小车通过蓝牙串口（Serial3）交换状态信息，用 HEX 协议保证数据可靠。

**Car1 发送端**：

```c
void Car1_Report_Status(void)
{
    uint16_t data[4];
    data[0] = (uint16_t)(int16_t)MyEncoder_Get_CNT(&Motor_A_Encoder); // 位置X
    data[1] = (uint16_t)(int16_t)MyEncoder_Get_CNT(&Motor_B_Encoder); // 位置Y
    data[2] = (uint16_t)(int16_t)MPU6050_Get_Yaw();                   // 朝向
    data[3] = (uint16_t)g_car1_state;                                  // 状态码

    Serial_Send_HEX_Package(&Serial3, data, 4);
}
```

**Car2 接收端**：

```c
void Car2_Read_Car1(void)
{
    uint8_t has_hex = Serial_GetNewPackageFlag_HEX(&Serial3);
    if (!has_hex) return;

    uint8_t len = Serial_GetHexLen(&Serial3);
    if (len != 4) return;          // 不是状态帧，忽略

    int16_t car1_x     = Serial_GetHexData(&Serial3, 0);
    int16_t car1_y     = Serial_GetHexData(&Serial3, 1);
    int16_t car1_yaw   = Serial_GetHexData(&Serial3, 2);
    int16_t car1_state = Serial_GetHexData(&Serial3, 3);

    // 根据对方状态调整自身策略
    switch (car1_state)
    {
        case 1:  /* Car1 在巡线 */   break;
        case 2:  /* Car1 在路口 */   break;
        case 3:  /* Car1 已完成 */   break;
    }
}
```

> **为什么双车通信用 HEX 而非 ABC**：HEX 帧每字带 XOR 校验，且解析快。蓝牙容易受干扰，用 HEX 可以识别并丢弃损坏的数据字（校验失败保留原值），ABC 受损则整帧丢弃。

---

### 示例 11：串口屏交互（TJC 串口屏 ABC 协议）

**场景**：串口屏通过 Serial4 发送按键事件，MSPM0 解析后切换页面或更新显示。

```c
void Mode_2_Loop(void)
{
    uint8_t has_abc = Serial_GetNewPackageFlag_ABC(&Serial4);
    if (!has_abc) return;

    // 串口屏按键格式：@page0.btn1.press$#
    if (Serial_Check_Str(&Serial4, "btn1"))
    {
        // 按键1被按下 → 切换到参数页面
        Serial_printf(&Serial4, "page1.txt_PID=\"Kp=%.2f\"\xFF\xFF\xFF",
                      g_kp);
    }
    else if (Serial_Check_Str(&Serial4, "btn2"))
    {
        // 按键2被按下 → 开始运行
        g_state = RUNNING;
        Serial_printf(&Serial4, "page0.txt_State=\"RUNNING\"\xFF\xFF\xFF");
    }
    else if (Serial_Check_Str(&Serial4, "slider"))
    {
        // 滑块值改变
        int speed;
        Serial_SetIntData(&Serial4, "slider", "slider=%d", &speed);
        g_target_speed = speed;
    }
}
```

> **串口屏注意事项**：TJC 屏的结束符是 `\xFF\xFF\xFF`，不是 `$#`。需要在发送端手动拼接。

---

## 附录 A：关键宏定义速查

| 宏 | 定义 | 所在文件 | 说明 |
|----|------|---------|------|
| `Serial_RX_MAX_WORDS` | 256 | Serial_base.h | HEX 帧最大接收字数 |
| `Serial_RX_BUF_SIZE` | 3×256+5=773 | Serial_base.h | rxBuf 缓冲区字节数 |
| `Serial_ABC_BUF_SIZE` | 40 | Serial_base.h | ABC 字符串缓冲区字节数 |
| `Serial_Wait_Tail_MAX` | 25 | Serial_base.h | ABC 帧尾等待超时 |
| `TX_BUF_SIZE` | 512 | Serial_porting.h | TX DMA 环形缓冲大小 |
| `Serial1_Enable` | 定义 | Serial_porting.h | 启用 UART_0 |
| `Serial_Debug` | 定义 | Serial_porting.h | 启用调试计数器 |

## 附录 B：错误码速查

```c
typedef enum {
    Serial_Err_None          = 0x00,  // 无错误

    // ABC 错误（0x01~0x0F）
    Serial_Err_ABC_Head      = 0x01,  // ABC 帧头错误（首字节≠'@'）
    Serial_Err_ABC_Tail      = 0x02,  // ABC 帧尾错误（未找到 '$' '#'）

    // HEX 错误（0x10~0xFF）
    Serial_Err_HEX_Head      = 0x10,  // HEX 帧头错误（首字节≠0xFF 或次字节≠0xAA）
    Serial_Err_HEX_Tail      = 0x20,  // HEX 帧尾错误（未找到 0x55 0xFE）
    Serial_Err_HEX_Len_OverFlow = 0x30, // LEN 超过 Serial_RX_MAX_WORDS(256)
} Serial_Error_Typedef;
```

---

> **最后更新**：2026-07-25
> **适用项目**：TI MSPM0G3507 智能小车 Template 分支 `Serial_Rebuild`
