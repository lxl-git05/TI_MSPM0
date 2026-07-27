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

---

## 12. Agent 移植任务规范（给 AI Agent 用）

> 本节是结构化指令，供 Agent 快速移植 Mode_X 到 Menu_Param。人阅读也可参考。

### 12.1 移植检查清单（Agent 自检）

每移植一个任务，Agent 必须完成以下 5 步，缺一不可：

| 步骤 | 文件 | 操作 | 验证方法 |
|------|------|------|----------|
| 1 | `Function/Menu_Param.h` | 在 `TUNE_COUNT` 前加枚举值 | 枚举连续、声明签名匹配 |
| 2 | `Function/Menu_Param.h` | 加 `Setup/Run/Tick` 回调声明 | 签名：`void Tune_Xxx_Setup(float p[4])` |
| 3 | `Function/Menu_Param.c` | 在 `s_labels[]` 追加标签行 | 与枚举索引位置一致 |
| 4 | `Function/Menu_Param.c` | 在 `Menu_Tune_Table[]` 追加表项行 | `{ Setup, Run, IsExit, Tick }` 四字段 |
| 5 | `Function/Menu_Param.c` | 实现 3~4 个回调函数 | 编译通过 |

### 12.2 标准移植模板（Agent 填空用）

Agent 移植时，直接复制以下模板填入 `{PLACEHOLDER}` 即可：

#### 步骤 1+2：Menu_Param.h — 枚举 + 声明

```c
// 在 TUNE_COUNT 之前插入：
    TUNE_{TASK_NAME},         // {一句话描述}

// 在 Tune_AlwaysFalse 声明之前插入：
void Tune_{TaskName}_Setup     (float p[4]);
void Tune_{TaskName}_Run       (float p[4]);
void Tune_{TaskName}_Tick      (float p[4]);
// 如果需要自定义 IsExit（如 Gyro_Cal），加：
bool Tune_{TaskName}_IsExit    (float p[4]);
```

#### 步骤 3：s_labels[] 追加

```c
    { "{Cat}",    "{Name}"    },  // TUNE_{TASK_NAME}
```

- `{Cat}` 用现有分类名（Motor_A/Motor_B/Car/Gyro/Stepper/Y8/Orange）或新分类（≤7 字符）
- `{Name}` 用任务名（≤8 字符）

#### 步骤 4：Menu_Tune_Table[] 追加

```c
    // TUNE_{TASK_NAME}
    { Tune_{TaskName}_Setup,       Tune_{TaskName}_Run,       Tune_AlwaysFalse, Tune_{TaskName}_Tick },
```

- IsExit 通常用 `Tune_AlwaysFalse`（手动 Skip 退出）
- 只有交互型任务（如 Gyro_Cal）才需要自定义 IsExit

#### 步骤 5：回调实现（在 s_labels 之前插入）

**模板 A — PID 型：**

```c
// ==================== TUNE_{TASK_NAME} ====================
void Tune_{TaskName}_Setup(float p[4])
{
    {初始化目标值，如 Motor_SetSpeed(&Motor_A, xxx)}
}

void Tune_{TaskName}_Run(float p[4])
{
    if (Serial_RoutePID(&{PID结构体}))
        {Goal 变化后的处理，如 Motor_SetSpeed(&Motor_A, xxx)};
    OLED_ShowPID("{Cat}", "{Name}", &{PID结构体});
}

void Tune_{TaskName}_Tick(float p[4])
{
    {20ms 硬件更新函数};
    Serial_printf(&Serial1, "%.2f,%.2f,%.2f\n",
        {PID}.goalPoint, {PID}.realPoint_Now, {PID}.setPoint);
}
```

**模板 B — 交互型（带状态机 + 按键触发）：**

```c
// ==================== TUNE_{TASK_NAME} ====================
static int      s_{task}_state = 0;
static uint32_t s_{task}_timer = 0;

void Tune_{TaskName}_Setup(float p[4])
{
    s_{task}_state = 0;
    OLED_Clear();
    OLED_Printf(0, 0, OLED_6X8, "{Title}:");
}

void Tune_{TaskName}_Run(float p[4])
{
    OLED_Printf(0, 10, OLED_6X8, "{关键参数展示}");

    if (s_{task}_state == 0)
    {
        OLED_Printf(0, 50, OLED_6X8, "K2:{Action} K1:Back");
        if (Key_Check(KEY_2, KEY_SINGLE))
        {
            s_{task}_state = 1;
            s_{task}_timer = Timer_Get_Ms();
        }
    }
    else if (s_{task}_state == 1)
    {
        OLED_Printf(0, 50, OLED_6X8, "Working...");
        if (Timer_Get_Ms() - s_{task}_timer >= {超时ms})
        {
            {执行操作};
            s_{task}_state = 2;
        }
    }
}

bool Tune_{TaskName}_IsExit(float p[4]) { return (s_{task}_state == 2); }

void Tune_{TaskName}_Tick(float p[4])
{
    Serial_printf(&Serial1, "{CSV格式}\n", {参数...});
}
```

**模板 C — 纯 Run 型（无 PID，无状态机）：**

```c
// ==================== TUNE_{TASK_NAME} ====================
void Tune_{TaskName}_Setup(float p[4])
{
    {可选初始化};
    OLED_Clear();
}

void Tune_{TaskName}_Run(float p[4])
{
    {OLED 显示关键数据};
    {按键处理 — 注意避开 KEY_1 长按（框架用于 Skip）};
    {Serial1 ABC 命令处理};
}

void Tune_{TaskName}_Tick(float p[4])
{
    Serial_printf(&Serial1, "{CSV格式}\n", {参数...});
}
```

### 12.3 Agent 适配常见组件指南

Agent 遇到以下组件时，按下方模式适配：

#### 12.3.1 模式有自己的状态机 / 独占按键

> 例：Mode_4 的 `Param_Loop()` 占用 KEY_1 长按

**方案：**
1. Run 回调中调用第三方状态机（如 `Param_Loop()`）
2. 如果第三方占用了 KEY_1 长按，则任务的 Skip 按键会被覆盖，这是**可接受的**（与原 Mode 行为一致）
3. 在任务注释里明确写出按键冲突和退出方式

```c
// 注释模板：
// KEY_1 长按 = {由第三方处理的功能}（由 {第三方函数} 处理）
// 退出任务：切换 Mode 或 LCD_Param_Skip 命令
```

#### 12.3.2 模式依赖 Serial2/Serial3 而非 Serial1

Menu_Param 框架的 `Serial_RoutePID()` 和 Tick 输出默认走 Serial1。如果原 Mode 使用其他串口：

- **Tick 输出**：直接改 `Serial_printf` 的目标串口
- **ABC 调参**：不要用 `Serial_RoutePID()`，自己在 Run 中处理 ABC 协议

```c
// 自定义串口 ABC 处理示例：
if (Serial_GetNewPackageFlag_ABC(&Serial{2/3/4}))
{
    Serial_SetFloatData(&Serial{2/3/4}, "Kp", "Kp=%f", &pid->Kp);
    // ...
}
```

#### 12.3.3 模式需要 EC11 旋转编码器

如果原 Mode 使用 EC11（通过 `Encoder_Get()`），注意：

- **Menu_Param 框架不调用 `Encoder_Get()`**，由各任务自行调用
- 如果使用 `Param_Loop()`（ParamEdit 库），它内部会调用 `Encoder_Get()`
- 如果自己实现 EC11 处理，在 Run 中调用 `Encoder_Get()` 并在 Tick 中清零（或 Run 中清零）

#### 12.3.4 模式需要 MIDDLE_KEY / KEY_3 等非常用按键

- `KEY_1` 在 Menu_Param 中用于导航（浏览模式: 单击下一项 / 长按入队；运行模式: 长按 Skip）
- `KEY_1` 的单击在任务运行中**空闲**（Menu_Param 不消费），可复用
- `KEY_2` 单击完全空闲，推荐用作任务内部辅助按键
- `KEY_3`（EC11 按键）完全空闲
- `KEY_0` 留给 Mode_G 切换模式，任务内**不要占用**

### 12.4 已有移植案例（Agent 参考）

| 任务 | 枚举 | 模板类型 | 源文件 | 复杂度 | 特殊处理 |
|------|------|----------|--------|--------|----------|
| Y8 Track | `TUNE_Y8_TRACK` | A (PID) + 双模切换 | `Mode/Mode_5.c` | 中 | KEY_2 切换展示/巡线模式，Tick 分流 |
| Orange Param | `TUNE_ORANGE_PARAM` | C (纯Run) + Param_Loop | `Mode/Mode_4.c` | 低 | 复用 ParamEdit 库，KEY_1 冲突已文档化 |

**Agent 移植前必须阅读源 Mode 文件和对应 Menu_Param.c 中的回调实现。** 直接用 Read 工具打开对比学习。

### 12.5 编译验证

Agent 移植完成后，告诉用户运行以下命令验证编译：

```bash
# 在 CCS 中 Build Project，或命令行：
make -C Debug -j8
```

常见编译错误：
- `undefined reference to Tune_Xxx_Setup` → 忘了在 .c 中实现回调
- `TUNE_COUNT undeclared` → 枚举值插入位置错误（应在 TUNE_COUNT 之前）
- `implicit declaration` → 忘了在 .h 中加声明

### 12.6 Agent 输出规范

Agent 完成移植后，输出格式：

```
## {任务名} — {源Mode} → Menu_Param 任务

### 修改文件
- `Function/Menu_Param.h` — 枚举 + 声明
- `Function/Menu_Param.c` — 标签 + 表项 + 回调实现

### 回调说明
| 回调 | 行为 |
|------|------|
| Setup | ... |
| Run   | ... |
| Tick  | ... |

### 按键分配
| 按键 | 功能 |
|------|------|
| ... | ... |

### 退出方式
- KEY_1 长按 Skip / LCD_Param_Skip / 切换 Mode
```
