## 2026-07-21 14:30 | MySystem 深度扫描 + 框架分析 + TODO 规划

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| CLAUDE.md | ./CLAUDE.md | 新增 | 项目架构文档，包含分层结构、依赖链、中断体系、TODO 规划 |

## 2026-07-21 15:30 | Phase 2+3: MySystem 重构 + OLED/Key/LED_Flash 移植

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MySystem.h | ./MySystem/MySystem.h | 修改 | 对齐 F407 风格，添加详细注释，引入 stdbool.h/stdio.h |
| MyGPIO.h | ./MySystem/MyGPIO.h | 修改 | 新增 22 个引脚声明（含空占位），API 重命名 My_GPIO→MyGPIO，保留兼容宏 |
| MyGPIO.c | ./MySystem/MyGPIO.c | 修改 | 实例定义移至 MySystem.c，仅保留 WritePin/ReadPin API |
| MySystem.c | ./MySystem/MySystem.c | 修改 | 集中定义所有 GPIO/PWM/Encoder 实例（从各 .c 移入），空引脚 {0,0} 占位 |
| MyPWM.c | ./MySystem/MyPWM.c | 修改 | 移除实例定义（已移至 MySystem.c） |
| MyEncoder.c | ./MySystem/MyEncoder.c | 修改 | 移除实例定义（已移至 MySystem.c） |
| MyTimer.h | ./MySystem/MyTimer.h | 修改 | 改为 include MySystem.h |
| MyPWM.h | ./MySystem/MyPWM.h | 修改 | 改为 include MySystem.h |
| MyEncoder.h | ./MySystem/MyEncoder.h | 修改 | 改为 include MySystem.h |
| Key.h | ./Hardware/Key.h | 修改 | 改为 include MySystem.h |
| Key.c | ./Hardware/Key.c | 修改 | Key_GetState 改用 MyGPIO_ReadPin |
| OLED.c | ./Hardware/OLED.c | 修改 | W_SCL/W_SDA 改用 MyGPIO_WritePin，改为 include MySystem.h |
| LED_Flash.h | ./Tools/LED_Flash.h | 修改 | 从 F407 复制，改为 include MySystem.h |
| LED_Flash.c | ./Tools/LED_Flash.c | 修改 | 从 F407 复制，全面改用 MyGPIO（结构体+API） |
| RGB.h | ./Hardware/RGB.h | 修改 | 改为 include MySystem.h |
| TCRT.h | ./Hardware/TCRT.h | 修改 | 改为 include MySystem.h |
| Mode_1.h | ./Mode/Mode_1.h | 修改 | 改为 include MySystem.h |
| Mode_2.h | ./Mode/Mode_2.h | 修改 | 清理多余依赖，改为 include MySystem.h |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 编写 OLED+Key+LED 集成测试（Key1/2 控制 LED 模式切换） |
| Mode_3.h | ./Mode/Mode_3.h | 修改 | 改为 include MySystem.h |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | 添加模式分发（Setup/Loop/Exit/Tick），Key0→Key2，修复 Timer_Init→Timer_Initial |
| AllHeader.h | ./App/AllHeader.h | 修改 | 移除重复的 Timer_Initial 声明 |
| AllHeader.c | ./App/AllHeader.c | 修改 | 移除递归 bug 的 Timer_Init 包装函数 |
| Claude_Change.md | ./Claude_Change.md | 新增 | 变更记录文件 |
| project-architecture.md | (memory) | 新增 | 项目完整分层架构记忆 |
| my-system-coupling-analysis.md | (memory) | 新增 | MySystem 与 TI DriverLib 耦合分析记忆 |
| todo-my-system-rebuild.md | (memory) | 新增 | MySystem 重构 6 Phase 计划记忆 |
| MEMORY.md | (memory) | 新增 | 记忆索引文件 |

## 2026-07-21 15:30 | README.md 硬件配置审查与补充

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| README.md | ./README.md | 修改 | 修正编码器中断描述(PB1/PB10)、LED_B黄色修正；新增PWM/定时器/DMA/时钟/调试/开发环境章节；新增Tianmengxing特殊引脚提醒 |

## 2026-07-22 10:30 | 移植 F407 MyPWM API 到 MSPM0

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyPWM.h | ./MySystem/MyPWM.h | 修改 | 结构体 PWM_MAX→Compare_Max，新增 Compare_Min 下限；SetCompare 参数 uint16_t→float 对齐 F407 |
| MyPWM.c | ./MySystem/MyPWM.c | 修改 | Init 改为读取 LOAD 寄存器校验+参数补全；SetCompare 改为双限幅[Min,Max]；GetFre 改为读取 LOAD 寄存器计算 |
| MySystem.c | ./MySystem/MySystem.c | 修改 | PWM 实例初始化增加 Compare_Min 字段（0.0f） |

## 2026-07-22 11:00 | 移植 F407 MyEncoder → MSPM0（GPIO双引脚中断模式）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyEncoder.h | ./MySystem/MyEncoder.h | 修改 | 新增 MyEncoder_Pins 子结构体封装 port+pin_A+pin_B（2个GPIO作为一个编码器整体）；MyEncoder_Counter_Tick 重命名为 MyEncoder_ISR |
| MyEncoder.c | ./MySystem/MyEncoder.c | 修改 | 移除 GROUP1_IRQHandler（上移至 Mode_G 总入口）；适配新结构体字段 access；代码注释重组 |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | 新增 GROUP1_IRQHandler 总中断分发器（DL_Interrupt_getPendingGroup + IIDX switch），case GPIOB → MyEncoder_ISR |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All 新增 MyEncoder_Init 初始化两路编码器 NVIC 中断 |
| MySystem.c | ./MySystem/MySystem.c | 修改 | Encoder 实例初始化改为嵌套初始化列表（MyEncoder_Pins 子结构体） |
| Encoder.h | ./MySystem/Encoder.h | 删除 | 冗余包装层（零外部引用），功能已内聚到 MyEncoder |
| Encoder.c | ./MySystem/Encoder.c | 删除 | 同上 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 编码器测试显示：OLED 展示 A/B 两路脉冲增量（20ms周期）和累计脉冲数 |

## 2026-07-22 16:00 | HEX模式bug修复：假帧头锁死/错误码残留/校验失败静默

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Function/Serial_porting.h | 修改 | 新增 frame_start_tick 字段 + HEX_FRAME_TIMEOUT_TICKS 宏(10ms) |
| Serial_porting.c | ./Function/Serial_porting.c | 修改 | ISR: LEN非法/超长/帧尾不匹配/超时→立即复位；Parse_HEX: 成功清err + 校验失败清零 |
| Timer_Counter.h | ./Tools/Timer_Counter.h | 修改 | 新增 Timer_Get_Ticks() 声明（ISR中超时判断用） |
| Timer_Counter.c | ./Tools/Timer_Counter.c | 修改 | 新增 Timer_Get_Ticks() 实现（直接读TIMG12计数器） |

## 2026-07-22 17:00 | ISR状态机重构 + 新增Send/Check函数（借鉴待移植库+F4参考）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Function/Serial_porting.h | 修改 | 新增 Serial_Rx_State 枚举(Idle/HEX/ABC) + rxState字段 + Serial_SendBytes/Serial_Send_HEX_Package/Serial_CheckCmd 声明 |
| Serial_porting.c | ./Function/Serial_porting.c | 修改 | ISR完全重写为显式状态机(switch-case)；新增 Serial_SendBytes/Serial_Send_HEX_Package/Serial_CheckCmd；Head2验证防止0xFF误触发 |
| CLAUDE.md | ./CLAUDE.md | 修改 | 更新标题：状态机重构记录 |
| Claude_Change.md | ./Claude_Change.md | 修改 | 本轮变更记录 |

## 2026-07-22 17:30 | 完善MySystem引脚声明（对照syscfg+README）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyGPIO.h | ./MySystem/MyGPIO.h | 修改 | 新增 EC11_S1/S2 声明；Y8_Addr0/1/2 → Y8_CLK/DAT（对齐syscfg命名）；所有引脚补充物理位置注释 |
| MySystem.c | ./MySystem/MySystem.c | 修改 | 9个 {0,0} 占位填充实际引脚(Buzzer/Elec/TCRT/EC11_Key/Stepper_En+Dir/Stepper2_En+Dir)；新增 EC11_S1/S2 定义；Y8 重命名；保留 Key3={0,0} 预留 |

## 2026-07-22 18:00 | 移植EC11旋转编码器（Encoder_Key模块，MySystem方案）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Encoder_Key.h | ./Hardware/Encoder_Key.h | 新增 | EC11编码器头文件：Encoder_Init/Get/ISR声明 |
| Encoder_Key.c | ./Hardware/Encoder_Key.c | 新增 | 移植F407逻辑到MSPM0：GPIOA中断检测S1/S2下降沿 → 去抖 → 方向判断；NVIC使能参照MyEncoder_Init模式 |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | GROUP1_IRQHandler 新增 DL_GPIO_MULTIPLE_GPIOA_INT_IIDX 分支→EC11_Encoder_ISR |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增 #include "Encoder_Key.h" |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All 新增 Encoder_Init() 调用 |
| Key.c | ./Hardware/Key.c | 修改 | Key_GetState 新增 KEY_3 → MyGPIO_EC11_Key 映射（替代注释掉的 Key3 占位） |

## 2026-07-22 15:00 | 新增 Mode_4/5/6 和 Con_Mode_1~6 模块框架

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Mode_4.h | ./Mode/Mode_4.h | 新增 | Mode_4 头文件（Setup/Loop/Tick/Exit 声明） |
| Mode_4.c | ./Mode/Mode_4.c | 新增 | Mode_4 实现（OLED 显示 "===Mode_4==="） |
| Mode_5.h | ./Mode/Mode_5.h | 新增 | Mode_5 头文件（Setup/Loop/Tick/Exit 声明） |
| Mode_5.c | ./Mode/Mode_5.c | 新增 | Mode_5 实现（OLED 显示 "===Mode_5==="） |
| Mode_6.h | ./Mode/Mode_6.h | 新增 | Mode_6 头文件（Setup/Loop/Tick/Exit 声明） |
| Mode_6.c | ./Mode/Mode_6.c | 新增 | Mode_6 实现（OLED 显示 "===Mode_6==="） |
| Con_Mode_1.h | ./Function/Con_Mode_1.h | 新增 | Con_Mode_1 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_1.c | ./Function/Con_Mode_1.c | 新增 | Con_Mode_1 实现（空函数框架） |
| Con_Mode_2.h | ./Function/Con_Mode_2.h | 新增 | Con_Mode_2 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_2.c | ./Function/Con_Mode_2.c | 新增 | Con_Mode_2 实现（空函数框架） |
| Con_Mode_3.h | ./Function/Con_Mode_3.h | 新增 | Con_Mode_3 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_3.c | ./Function/Con_Mode_3.c | 新增 | Con_Mode_3 实现（空函数框架） |
| Con_Mode_4.h | ./Function/Con_Mode_4.h | 新增 | Con_Mode_4 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_4.c | ./Function/Con_Mode_4.c | 新增 | Con_Mode_4 实现（空函数框架） |
| Con_Mode_5.h | ./Function/Con_Mode_5.h | 新增 | Con_Mode_5 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_5.c | ./Function/Con_Mode_5.c | 新增 | Con_Mode_5 实现（空函数框架） |
| Con_Mode_6.h | ./Function/Con_Mode_6.h | 新增 | Con_Mode_6 头文件（Setup/Loop/Tick/Exit 声明） |
| Con_Mode_6.c | ./Function/Con_Mode_6.c | 新增 | Con_Mode_6 实现（空函数框架） |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增 Mode_4/5/6 和 Con_Mode_1~6 的 include |
| Mode_G.h | ./Mode/Mode_G.h | 修改 | 枚举新增 Mode_4/5/6 |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | Switch 分发新增 Mode_4/5/6 的 Setup/Loop/Exit/Tick 分支 |
