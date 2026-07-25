# TI MSPM0 智能小车项目 CLAUDE.md

> 基于 2026-07-21 深度分析生成，2026-07-22 更新（Serial状态机重构 + HEX修复 + 引脚完善 + EC11移植）

## 项目概述

TI MSPM0G3507 (Cortex-M0+) 智能小车竞赛项目。支持巡线、路口识别、IMU 姿态控制（ICM42688/Mahony + 统一API解耦）、双车蓝牙通信。

## 分层架构

| 层 | 目录 | 职责 |
|---|------|------|
| HAL | `MySystem/` | 硬件抽象层（GPIO/PWM/Encoder/Timer） |
| Hardware | `Hardware/` | 设备驱动（OLED/Key/RGB/Buzzer/Encoder_Key/Stepper_PWM）+ 协议（Serial_base） |
| IMU | `IMU/` | 陀螺仪驱动+滤波+统一API层（ICM_42688_base/Mahony + IMU.h/c 解耦宏切换） |
| Function | `Function/` | 控制算法（Con_Task/Con_Motor/Control）+ 串口移植层（Serial_porting） |
| Software | `Software/` | 通用中间件（MyPID/Queue） |
| Con_Motor | `Function/` | 电机控制（速度环+角度环+位置环+整车直行） |
| Mode | `Mode/` | 应用模式（Manager/Mode_1/Mode_2/Mode_3/Mode_4） |
| App | `App/` | 状态机/菜单/统一初始化 |
| Tools | `Tools/` | 工具（LED闪烁/LED_Flash/Timer_Counter/TJC_LCD） |

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
| TCRT | MyGPIO_TCRT | PA13 | 输入 上拉 |
| EC11 | MyGPIO_EC11_S1 | PA12 | 输入 上拉 下降沿中断 |
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
| UART_4 | 异步 | `UART_3_INST_IRQHandler` | Serial4 RX 中断（状态机） | `Function/Serial_porting.c` |

## 当前状态：整车控制算法开发阶段

### 已完成模块

| 模块 | 文件 | 状态 |
|------|------|------|
| MyPWM | `MySystem/MyPWM.h/c` | ✅ 结构体对齐 F407（Compare_Max/Min），SetCompare 双限幅 |
| MyEncoder | `MySystem/MyEncoder.h/c` | ✅ MyEncoder_Pins 引脚对封装，双路编码器 ISR |
| Serial | `Function/Serial_porting.h/c` | ✅ ★Status 0/1/2状态机(借鉴Car1)+错误中断处理(防FIFO锁死)+rx_temp暂存模式，阻塞发送 |
| Serial_base | `Hardware/Serial_base.h/c` | ✅ 协议层不改动（纯逻辑） |
| Encoder_Key | `Hardware/Encoder_Key.h/c` | ✅ EC11 旋转编码器，GPIOA 下降沿中断+方向判断，NVIC 参照 MyEncoder 模式 |
| ICM42688 Driver | `IMU/ICM_42688_base.h/c` | ✅ MSPM0 DriverLib I2C 适配，±4g/±500°/s，重试+总线恢复 |
| ICM42688 Mahony | `IMU/ICM42688_Mahony.h/c` | ✅ 四元数+PI重力修正(Mahony)，绝对yaw解绕，零偏标定 |
| **IMU 统一API** | `IMU/IMU.h/c` | ✅ ★ 传感器解耦宏切换(ICM/MPU)，Types内置，Turn_Yaw到位检测 |
| GPIO 引脚 | `MySystem/MyGPIO.h` + `MySystem/MySystem.c` | ✅ 全部 25 个引脚声明完成，对照 syscfg+README |
| Con_Motor Pos API | `Function/Con_Motor.h/c` | ✅ ★ Motor_SetPos/Get_Pos/Is_Pos/Clear + Motorx_Pos_Update_Tick，Wheel_Cm=20cm |
| PID_Car_Straight | `Function/Con_Motor.c` | ✅ ★ 整车直行：双编码器平均+IMU偏航修正(Kp=5.0)+梯形变速(加速15cm/减速25cm) |
| TASK_CAR_STRAIGHT | `Function/Control.c` + `Con_Task.h` | ✅ ★ Con_Task直行任务：p[0]=目标cm(≤0=永远直行), p[1]=容差, p[2]=max_speed |
| Con_Task_Skip | `Function/Con_Task.h/c` | ✅ 强制跳过当前任务保留队列，记录标记"Skip" |
| Mode_3 PID调参 | `Mode/Mode_3.c` | ✅ MODE3_SELECT==1：4模式循环(Speed→Angle→Pos→Straight) |

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

**2026-07-24 重构（借鉴Car1）：**
- `Serial_Rx_State_Check()` — 统一字节处理入口，Status 0/1/2 状态机
- `Serial_Data_Check_HEX/ABC()` — 分离的数据检测+处理函数
- 所有 UART ISR 新增 `DL_UART_MAIN_IIDX_OVERRUN/BREAK/PARITY/FRAMING/NOISE_ERROR` 处理
- `Serial_Init()` 中使能 `OVERRUN|FRAMING|BREAK` 错误中断（防 FIFO 锁死）
- 新增 `Serial3` (UART_2, PB15/PB16, 115200)，实例编号：1=USB, 2=树莓派, 3=蓝牙, 4=串口屏

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

// ===== IMU 统一 API（底层解耦，不关心传感器型号） =====
IMU_Mahony_Init(1);                     // 初始化+自动零偏标定（需静止！）
IMU_Mahony_Update_Tick();               // 20ms Tick: 读传感器→Mahony解算
IMU_Mahony_Real.roll                    // 横滚角 (±180°)
IMU_Mahony_Real.pitch                   // 俯仰角 (±90°)
IMU_Mahony_Real.yaw                     // 偏航角 (±180°)
float abs_yaw = IMU_Yaw_Abs_Get();      // 绝对累计yaw（顺时针增大，无跳变）
IMU_Yaw_Abs_Reset();                    // 归零累计yaw
IMU_Turn_Yaw_Is_Ok(90.0f)              // 转到 90°±3° 了？(默认死区)
IMU_Turn_Yaw_Is_Ok_Ex(180.0f, 5.0f)    // 转到 180°±5° 了？(自定义死区)

// 切换传感器：IMU.h 中取消/注释 #define IMU_USE_MPU6050（一行改全局生效）

// ===== 位置环（单电机，cm） =====
Motor_SetPos(&Motor_A, 50.0f);          // 设置目标距离(cm)
float pos = Motor_Get_Pos(&Motor_A);     // 获取当前距离(cm)
Motor_Is_Pos(&Motor_A, 50, 2, 5.0f);    // 是否到达目标(容差2cm, 速度<5rpm)
Motorx_Pos_Update_Tick(&Motor_A, 1);     // 20ms Tick: 读编码器→PID→输出速度

// ===== 整车直行位置环（双电机+IMU偏航修正+梯形变速） =====
PID_Car_Straight_Init();                // 初始化（Kp=20, Kd=3, Out±100）
PID_Car_Straight_Reset();               // 清零编码器+记录起始yaw+清PID
PID_Car_Straight_SetSpeedParams(150);   // 巡航最高速度(rpm), 0=默认200
PID_Car_Straight_Tick();                // 20ms: 平均距离→PID→梯形限速→IMU修正→差速

// ===== Con_Task 直行任务 =====
Con_Task_Enqueue(TASK_CAR_STRAIGHT, 100, 2, 0, 0);  // 直行100cm, 容差2cm
Con_Task_Enqueue(TASK_CAR_STRAIGHT, 0, 0, 0, 0);    // 永远直行, Con_Task_Skip停止
Con_Task_Skip();                        // 强制跳过当前任务（保留队列后续任务）

// ===== TJC_LCD 串口屏调参 =====
TJC_LCD_Process();                      // Loop首行调用，解析Serial4 ABC数据
if (LCD_Key_Pressed(LCD_KEY_1)) { }     // 按键检测（读后清零）
LCD_Param_Set(1, &kp, 0, 100);          // 滑块1→整型 0~100 映射
LCD_Param_Set_Float(2, &ki, 0.0f, 2.0f); // 滑块2→浮点 0.0~2.0 映射
// 移植：在 #include "TJC_LCD.h" 前 #define TJC_LCD_SERIAL Serial2 等
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
- `IMU/IMU.h` — ★ 统一API层：类型定义+宏切换传感器(ICM/MPU)+宏映射+Turn_Yaw声明
- `IMU/IMU.c` — Turn_Yaw到位检测实函数（基于IMU_Yaw_Abs_Get）
- `IMU/ICM_42688_base.h/c` — ICM42688 I2C驱动（MSPM0适配）
- `IMU/ICM42688_Mahony.h/c` — Mahony AHRS 四元数滤波（纯数学，零移植成本）
- `Mode/Mode_2.c` — ★TJC_LCD调参演示（虚拟按键+滑块映射+OLED实时显示）
- `Tools/TJC_LCD.h/c` — ★TJC串口屏调参库（虚拟按键+滑块→变量映射，移植配置宏）
- `Mode/Mode_3.c` — PID调参模式（MODE3_SELECT=1: Speed/Angle/Pos/Straight 4循环切换）
- `Mode/Mode_4.c` — Con_Task 演示模式（任务序列+OLED+按键入队）
- `Function/Con_Motor.c` — ★电机控制三环（速度/角度/位置）+ PID_Car_Straight 整车直行（梯形变速+偏航修正）
- `Function/Control.c` — ★全局任务表 Control_TaskTable + 全部任务回调（含 TASK_CAR_STRAIGHT）
- `Function/Con_Task.h/c` — 任务队列调度器（含 Con_Task_Skip 强制跳过）
- `Function/Serial_porting.c` — ★串口状态机 ISR(Status 0/1/2) + 错误中断处理 + HEX/ABC 协议解析
- `Hardware/Motor.h` — Motor_Typedef 三环 PID + Motor_Param_Typedef（含 Wheel_Cm=20cm）
- `Hardware/Encoder_Key.c` — EC11 旋转编码器驱动（GPIOA 中断）
- `README.md` — 引脚配置、UART/编码器/电机引脚表
- `empty.syscfg` — SysConfig 配置文件
- `Claude_Change.md` — 变更记录
