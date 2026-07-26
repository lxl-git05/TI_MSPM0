# Menu_Param 调参菜单框架手册

> 基于 Con_Task 任务队列调度器的统一调参系统，覆盖全部 PID + IMU 校准 + 步进电机控制。

---

## 1. 架构概览

```
TuneTaskID 枚举 (Menu_Param.h)
    │  11 个调参任务 ID
    ▼
Setup / Run / IsExit / Tick 回调 (Menu_Param.c)
    │  每个任务 4 个生命周期函数
    ▼
Menu_Tune_Table[] 任务描述表 (Menu_Param.c)
    │  按 TuneTaskID 索引
    ▼
Con_Task 调度器 (Con_Task.c)
    │  出队 → Setup → Run → IsExit → 下一个
    ▼
Mode_3 (Mode_3.c)
    │  薄封装：Setup 调 Init，Loop 调 Loop
    ▼
20ms ISR (Mode_G.c)
    │  Con_Task_Tick() → 当前任务 .Tick()
```

---

## 2. 需要包含的文件

| 文件 | 作用 | 必须 |
|------|------|------|
| `Function/Menu_Param.h` | 枚举 + 回调声明 + API 声明 | ✅ |
| `Function/Menu_Param.c` | 11 个任务实现 + 任务表 + 菜单导航 | ✅ |
| `Function/Con_Task.h` | 任务调度器 API（Menu_Param 依赖） | ✅ |
| `Function/Con_Task.c` | 调度器实现 | ✅ |
| `Software/Queue.h` | 环形队列（Con_Task 依赖） | ✅ |
| `Software/Queue.c` | 队列实现 | ✅ |
| `Mode/Mode_3.h` | Mode_3 声明 | ✅ |
| `Mode/Mode_3.c` | Mode_3 实现（薄封装） | ✅ |
| `App/AllHeader.h` | 项目统一头文件（需 `#include "Menu_Param.h"`） | ✅ |
| `Hardware/Key.h` | 按键事件（KEY_SINGLE/DOUBLE/LONG） | 依赖 |
| `Hardware/OLED.h` | OLED 显示 | 依赖 |
| `Hardware/Motor.h` | Motor_A/B 结构体（PID 访问） | 依赖 |
| `Function/Con_Motor.h` | PID_Car_Straight / PID_Angle | 依赖 |
| `Hardware/Stepper_PWM.h` | Stepper1/2 结构体 | 依赖 |
| `IMU/IMU.h` | IMU_Mahony_Real / GyroBias | 依赖 |

**编译配置：** `Debug/makefile` 和 `Debug/Function/subdir_vars.mk` 需包含 `Menu_Param.o`。

---

## 3. 初始化流程

### 3.1 系统启动（AllHeader.c → Initial_All）

```c
void Initial_All(void)
{
    // ... 硬件初始化 ...
    Con_Motor_Init();       // ★ 先初始化 PID（设置默认 Kp/Ki/Kd）
    // ... 其他初始化 ...
    // Menu_Param 无需在此初始化，由 Mode_3_Setup 按需调用
}
```

### 3.2 进入 Mode_3（Mode_3.c）

```c
#include "Mode_3.h"
#include "AllHeader.h"

void Mode_3_Setup(void)
{
    OLED_Clear();
    Menu_Tune_Init();           // ★ 注册 Menu_Tune_Table + 清空队列
}

void Mode_3_Loop(void)
{
    Menu_Tune_Loop();           // ★ 每帧调用：Con_Task_Loop + 按键处理
}

void Mode_3_Tick(void)
{
    // 空！Con_Task_Tick() 已在 Timer_20ms_Callback 中全局调用
}

void Mode_3_Exit(void)
{
    Con_Task_Clear();           // 清空队列 + 终止当前任务
    Motor_Stop(&Motor_A);       // 停止硬件
    Motor_Stop(&Motor_B);
    OLED_Clear();
}
```

**关键点：** `Mode_3_Tick()` 为空是因为 `Con_Task_Tick()` 在 `Mode_G.c` 的 20ms ISR 中全局分发，无需 Mode 层重复调用。

---

## 4. 11 个调参任务一览

| 索引 | 枚举 | 分类 | 类型 | Setup | Run | IsExit | Tick |
|------|------|------|------|-------|-----|--------|------|
| 0 | `TUNE_MOTOR_A_SPEED` | Motor_A | PID速度 | `Motor_SetSpeed` | Serial调参+OLED | AlwaysFalse | Serial 三值 |
| 1 | `TUNE_MOTOR_A_ANGLE` | Motor_A | PID角度 | `Motor_SetAngle` | Serial调参+OLED | AlwaysFalse | 更新+Serial |
| 2 | `TUNE_MOTOR_A_POS` | Motor_A | PID位置 | `Motor_SetPos` | Serial调参+OLED | AlwaysFalse | 更新+Serial |
| 3 | `TUNE_MOTOR_B_SPEED` | Motor_B | PID速度 | `Motor_SetSpeed` | Serial调参+OLED | AlwaysFalse | Serial 三值 |
| 4 | `TUNE_MOTOR_B_ANGLE` | Motor_B | PID角度 | `Motor_SetAngle` | Serial调参+OLED | AlwaysFalse | 更新+Serial |
| 5 | `TUNE_MOTOR_B_POS` | Motor_B | PID位置 | `Motor_SetPos` | Serial调参+OLED | AlwaysFalse | 更新+Serial |
| 6 | `TUNE_CAR_STRAIGHT` | Car | PID直行 | `PID_Car_Straight_Reset` | Serial调参+OLED | AlwaysFalse | 更新+Serial |
| 7 | `TUNE_GYRO_YAW` | Gyro | PID偏航 | `PID_Angle_Reset` | Serial调参+OLED+Yaw三大参数 | AlwaysFalse | 更新+Serial |
| 8 | `TUNE_GYRO_CAL` | Gyro | 交互校准 | 初始化状态机 | 展示Bias+KEY_2触发校准 | state==2退出 | Bias三值 |
| 9 | `TUNE_STEPPER_S1` | Stepper | 步进控制 | — | Serial+OLED | AlwaysFalse | Pos/Speed三值 |
| 10 | `TUNE_STEPPER_S2` | Stepper | 步进控制 | — | Serial+OLED | AlwaysFalse | Pos/Speed三值 |

### 任务类型说明

**PID 型（0-7）：**
- Run 中通过 Serial1 ABC 协议接收 `Kp=%f` / `Ki=%f` / `Kd=%f` / `Goal=%f`
- Goal 变化时自动调用对应 `Motor_Set*` 更新目标
- Tick（20ms）调用硬件更新函数 + Serial1 输出 `goalPoint,realPoint_Now,setPoint`

**交互校准型（8）：**
- Run 中实时显示当前 BiasX/Y/Z 值
- KEY_2 单击 → 等待 1s 稳定 → 自动校准 → 写 EEPROM → 退出
- Tick（20ms）输出 `BiasX,BiasY,BiasZ` 到 Serial1

**步进型（9-10）：**
- Run 中通过 Serial1 ABC 协议接收 `Speed=%f` / `Pos=%f` / `Stop`
- Tick（20ms）输出 `Pos_Tar,Pos_Now,Speed_Now` 到 Serial1

---

## 5. 按键操作

### 浏览模式（无任务运行）

| 按键 | 操作 | 说明 |
|------|------|------|
| KEY_1 单击 | 下一个任务 | 平铺循环（0→1→...→10→0） |
| KEY_1 长按 | 入队激活 | 将当前光标位置的任务加入调度器 |
| KEY_0 双击 | 切换模式 | 全局按键（Mode_G.c），退出 Mode_3 |

### 运行模式（任务执行中）

| 按键 | 操作 | 说明 |
|------|------|------|
| KEY_1 长按 | Skip | 强制终止当前任务，出队下一个 |
| KEY_2 单击 | 触发校准 | **仅 TUNE_GYRO_CAL**：开始校准流程 |

---

## 6. 串口调参（Serial1 ABC 协议）

所有 PID 型任务支持通过 Serial1 实时修改参数：

```
Kp=8.5        ← 设置比例增益
Ki=0.05       ← 设置积分增益
Kd=1.2        ← 设置微分增益
Goal=90       ← 设置目标值（角度°/距离cm）
```

步进电机任务支持：

```
Speed=200     ← 设置速度
Pos=180       ← 移动到绝对位置
Stop          ← 紧急停止
```

**20ms Tick 输出格式（Serial1 CSV）：**

| 任务类型 | 输出格式 | 说明 |
|---------|---------|------|
| PID 型 | `goal,real,set` | 目标值/实际值/输出值 (%.2f) |
| Gyro_Cal | `BiasX,BiasY,BiasZ` | 陀螺仪零偏 (%.4f) |
| Stepper | `Pos_Tar,Pos_Now,Speed_Now` | 目标位置/当前位置/当前速度 |

**Yaw Loop 额外输出：**
| 任务 | 输出格式 | 说明 |
|------|---------|------|
| Gyro_Yaw | `Yaw:角度 Tar:目标 Out:PID输出` | 每帧 Loop 输出 IMU 物理量 |

---

## 7. 如何新增任务

只需改 **2 个文件**，无需动框架代码。

### 步骤 1：Menu_Param.h — 加枚举

在 `TUNE_COUNT` 之前加新 ID：

```c
typedef enum {
    TUNE_MOTOR_A_SPEED = 0,
    // ... 现有 11 个 ...
    TUNE_STEPPER_S2,
    TUNE_MY_NEW_TASK,        // ★ 新增（必须在 TUNE_COUNT 之前）
    TUNE_COUNT
} TuneTaskID;
```

### 步骤 2：Menu_Param.h — 加回调声明

```c
// 在回调声明区追加：
void Tune_MyNew_Setup (float p[4]);
void Tune_MyNew_Run   (float p[4]);
void Tune_MyNew_Tick  (float p[4]);
bool Tune_MyNew_IsExit(float p[4]);  // 如果不用自动退出则复用 Tune_AlwaysFalse
```

### 步骤 3：Menu_Param.c — 加标签

在 `s_labels[]` 数组中追加：

```c
static const TuneLabel s_labels[TUNE_COUNT] = {
    // ... 现有 11 个 ...
    { "Stepper", "S2"       },
    { "MyCat",   "MyName"   },  // ★ 新增
};
```

### 步骤 4：Menu_Param.c — 写回调函数

#### 模板 A：PID 型任务

```c
// ==================== TUNE_MY_NEW_PID ====================
void Tune_MyNew_Setup(float p[4])
{
    // 初始化硬件、设置目标值
    Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint);
}
void Tune_MyNew_Run(float p[4])
{
    // Serial1 调参 + OLED 显示
    if (Serial_RoutePID(&Motor_A.PID_s))
        Motor_SetSpeed(&Motor_A, Motor_A.PID_s.goalPoint);
    OLED_ShowPID("MyCat", "MyPID", &Motor_A.PID_s);
}
void Tune_MyNew_Tick(float p[4])
{
    // 20ms: 硬件更新 + Serial1 调试输出
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        Motor_A.PID_s.goalPoint, Motor_A.PID_s.realPoint_Now, Motor_A.PID_s.setPoint);
}
```

#### 模板 B：交互型任务（带状态机 + 按键触发）

```c
// ==================== TUNE_MY_INTERACTIVE ====================
static int      s_my_state = 0;
static uint32_t s_my_timer = 0;

void Tune_MyNew_Setup(float p[4])
{
    s_my_state = 0;
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "My Task:");
}
void Tune_MyNew_Run(float p[4])
{
    // 展示关键参数
    OLED_Printf(0, 10, OLED_6X8, "Val:%.2f", some_value);

    if (s_my_state == 0)
    {
        OLED_Printf(0, 50, OLED_6X8, "K2:Do K1:Back");
        if (Key_Check(KEY_2, KEY_SINGLE))
        {
            s_my_state = 1;
            s_my_timer = Timer_Get_Ms();
        }
    }
    else if (s_my_state == 1)
    {
        OLED_Printf(0, 50, OLED_6X8, "Working...");
        // 执行操作（可能需要延时）
        if (Timer_Get_Ms() - s_my_timer >= 2000)
        {
            DoSomething();
            s_my_state = 2;  // 完成 → IsExit 返回 true
        }
    }
}
bool Tune_MyNew_IsExit(float p[4]) { return (s_my_state == 2); }
void Tune_MyNew_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.2f\n", some_value);
}
```

#### 模板 C：只有 Run 的任务（无 Setup / 用 AlwaysFalse）

```c
void Tune_MyNew_Run(float p[4])
{
    if (Serial_GetNewPackageFlag_ABC(&Serial1))
    {
        // 处理 Serial1 命令
    }
    OLED_Printf(0, 10, OLED_6X8, "Status:OK");
}
void Tune_MyNew_Tick(float p[4])
{
    Serial_printf(&Serial1, "%.1f,%.1f\n", val1, val2);
}
```

### 步骤 5：Menu_Param.c — 注册到任务表

在 `Menu_Tune_Table[]` 数组末尾（`TUNE_COUNT` 之前）加行：

```c
Task_Descriptor_Typedef Menu_Tune_Table[TUNE_COUNT] = {
    // ... 现有 11 个 ...
    // TUNE_STEPPER_S2
    { NULL,                    Tune_Stepper_S2_Run,   Tune_AlwaysFalse, Tune_Stepper_S2_Tick },
    // TUNE_MY_NEW_TASK
    { Tune_MyNew_Setup,        Tune_MyNew_Run,        Tune_AlwaysFalse, Tune_MyNew_Tick },
};
```

**表项四个字段：** `{ Setup, Run, IsExit, Tick }` — 不需要的回调填 `NULL`。

---

## 8. 如何修改任务次序

任务次序由 **两个数组** 的排列顺序共同决定：

### 8.1 简单重排（推荐）

只需调整 `s_labels[]` 的定义顺序 + 对应的回调实现顺序 + `Menu_Tune_Table[]` 的行顺序。

**示例：** 把 `TUNE_GYRO_CAL` 从索引 8 移到索引 0（第一个）：

1. `TuneTaskID` 枚举 — 不改（枚举值自动跟随位置）
2. `s_labels[]` — 把 `{ "Gyro", "Cal" }` 移到数组第一行
3. `Menu_Tune_Table[]` — 把对应表项移到数组第一行
4. 回调函数实现 — 顺序无所谓（只要声明在前）

> **本质：** `TuneTaskID` 枚举从 0 开始自增，`s_labels[]` 和 `Menu_Tune_Table[]` 按枚举索引访问。把哪行放第一，哪个任务就是索引 0。

### 8.2 保持枚举值不变重排

如果需要枚举值保持不变（比如已有代码引用了 `TUNE_MOTOR_A_SPEED`），可以显式指定枚举值：

```c
typedef enum {
    TUNE_GYRO_CAL    = 0,   // 手动指定值
    TUNE_MOTOR_A_SPEED = 1,
    TUNE_MOTOR_A_ANGLE = 2,
    // ...
    TUNE_COUNT
} TuneTaskID;
```

然后对应调整 `s_labels[]` 和 `Menu_Tune_Table[]` 的索引位置。

---

## 9. 任务生命周期详解

```
                  Con_Task_Enqueue(type, 0,0,0,0)
                         │
                         ▼
              ┌─────────────────────┐
              │  队列 (FIFO, 50项)   │
              └─────────────────────┘
                         │
              Con_Task_Loop() 出队  │
                         ▼
              ┌─────────────────────┐
              │  Step 1: Setup()    │  调用一次
              │  Task_SetupDone=true│
              └─────────────────────┘
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
      ┌─────────────┐       ┌─────────────┐
      │ 每帧: Run()  │       │ 20ms: Tick() │
      │ (Loop上下文) │       │ (ISR上下文)  │
      └─────────────┘       └─────────────┘
              │
              ▼
      ┌─────────────────────┐
      │  IsExit() == true?  │──否──→ 继续 Run
      └─────────────────────┘
              │是
              ▼
      ┌─────────────────────┐
      │  记录耗时 → 空闲     │
      │  自动出队下一个任务   │
      └─────────────────────┘

      中断路径: KEY_1 长按 → Con_Task_Skip() → 强制退出
```

**关键时序：**
- `Run()` 在主循环中调用（帧率取决于 Loop 速度）
- `Tick()` 在 20ms 定时器 ISR 中调用（精确 50Hz）
- 如果 `IsExit` 在 Setup 完成后的第一个 Loop 周期返回 true，任务只执行一帧
- `Con_Task_Skip()` 不会停止硬件（调用方负责 `Motor_Stop`）

---

## 10. 调试输出总览

| 输出通道 | 频率 | 内容 |
|---------|------|------|
| OLED | 每帧 | PID: Kp/Ki/Kd/Goal/Real/Set（6行）；Stepper: Pos/Tar/Speed；GyroCal: Bias三值 |
| Serial1 Tick | 20ms | PID: `goal,real,set`；GyroCal: `Bx,By,Bz`；Stepper: `Pos_Tar,Pos_Now,Speed_Now` |
| Serial1 Loop | 每帧 | Gyro_Yaw: `Yaw:角度 Tar:目标 Out:PID输出` |
| Serial1 ABC | 按需 | 接收 Kp/Ki/Kd/Goal/Speed/Pos/Stop 命令 |

---

## 11. 注意事项

1. **PID 默认值：** 在 `Con_Motor_Init()` 中设置，启动时即生效。调参修改后断电即失（当前未持久化）。
2. **Gyro_Cal 校准：** 校准结果自动写入 AT24C02 EEPROM，断电保持。校准前有 1s 等待期，确保小车静止。
3. **步进电机：** 开环控制，无 PID。Stepper1/2 的 `Setup` 为 NULL（无需启动初始化）。
4. **队列容量：** 50 个任务，足够使用。`Con_Task_Clear()` 在 Mode_3_Exit 时清空。
5. **边界检查：** 任务类型越界会导致 LED 快闪 + 死循环（防御性编程）。
6. **Tick 不阻塞：** 所有 Tick 在 ISR 上下文中执行，必须快速返回。不要在里面加延时或重量级操作。
