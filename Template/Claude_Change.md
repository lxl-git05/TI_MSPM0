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
