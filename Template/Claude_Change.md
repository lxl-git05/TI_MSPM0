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

## 2026-07-25 19:00 | Serial1 接收回显函数实现

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_Drive.h | ./Serial/Serial_Drive.h | 修改 | 添加完整 Serial1 API：Init/SendByte/SendString/SendData/Printf/GetRxCount/GetRxBuffer/ClearRxBuffer/SetEchoMode/GetEchoMode |
| Serial_Drive.c | ./Serial/Serial_Drive.c | 修改 | 实现 UART_0_INST_IRQHandler (逐字节RX+缓冲+回显)、Serial1_Init (DMA清理+NVIC使能)、阻塞发送、缓冲管理 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 完整重写为串口回显测试界面：OLED 6行布局(Y=0/10/20/30/40/50)、Key1单击切换回显/Key1长按清缓冲/Key2单击发送测试消息/Key2长按打印摘要、20ms Tick刷新 |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All 中 Serial_Init() → Serial1_Init() |
| serial1-echo-mode2-guide.md | (memory) | 新增 | 记录 Serial1 开发规范：Mode_2 独占工作区、Key1/2 单击长按、OLED Y=10倍数、串口驱动逻辑在 Serial_Drive |

## 2026-07-25 20:30 | Serial_porting 优化：错误中断+FIFO防锁+ISR去重+LEN溢出检测

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.c | ./Serial/Serial_porting.c | 修改 | 4项优化：(1)非DMA模式补全DL_UART_MAIN_INTERRUPT_RX使能(之前只启用了RX_TIMEOUT_ERROR，可能导致逐字节RX不触发)；(2)DMA和非DMA模式均启用OVERRUN/FRAMING/BREAK错误中断(防止噪声FIFO锁死)；(3)4个ISR重复代码统一为Serial_RxISR_Dispatch+Serial_Rx_ProcessByte静态函数(从~120行缩减到~30行)；(4)HEX帧收到LEN字段后按预期帧长提前检测溢出(而非等到773字节缓冲区满) |
| Serial_porting.h | ./Serial/Serial_porting.h | 修改 | 新增 dbg_parse_ok 成功解析计数器（与 dbg_rx_frames 状态机帧检测分开统计） |

## 2026-07-25 20:45 | 加入成功解析计数器 dbg_parse_ok

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Serial/Serial_porting.h | 修改 | 新增 `dbg_parse_ok` 字段（校验全部通过的成功解析次数），与 `dbg_rx_frames`（状态机帧检测次数）区分 |
| Serial_porting.c | ./Serial/Serial_porting.c | 修改 | `dbg_rx_frames++` 移至 `Serial_Rx_ProcessByte`（状态机层）；`dbg_parse_ok++` 放在 `Serial_Parse_HEX/ABC` 成功路径（解析层）；`Serial_PrintDebug` 新增 `Parse OK` 行 |

## 2026-07-25 21:00 | 拆分 Overflow/HW Errors 计数器 + 丢帧检测

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Serial/Serial_porting.h | 修改 | 新增 `dbg_hardware`（硬件错误）和 `dbg_frame_lost`（丢帧检测）两个独立计数器 |
| Serial_porting.c | ./Serial/Serial_porting.c | 修改 | ISR 错误中断改用 `dbg_hardware++`（与协议层 `dbg_overflow` 分离）；HEX/ABC 解析成功前检测 flag 是否已置位→`dbg_frame_lost++`；`Serial_PrintDebug` 新增 `Lost` 和 `HW Errors` 行 |

## 2026-07-25 21:15 | Serial 模块完整说明书

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| README.md | ./Serial/README.md | 新增 | 串口模块完整说明书：架构、硬件配置、协议格式（HEX/ABC）、状态机、API参考、调试打印字段解释、移植步骤（新增实例/换MCU）、常见坑点速查表、高级主题（DMA/中断优先级/高吞吐量）|
