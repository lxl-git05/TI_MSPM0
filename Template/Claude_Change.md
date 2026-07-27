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
| empty.c | ./empty.c | 修改 | main() 中 3 组 switch 分发新增 Mode_4/5/6 的 Loop/Exit/Setup 分支 |

## 2026-07-22 16:30 | Stepper_PWM 步进电机驱动 STM32→MSPM0 移植

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyPWM.h | ./MySystem/MyPWM.h | 修改 | 结构体新增 Tim_Clock+Tim_IRQn 字段；新增 SetLoadValue/GetTimClock/EnableIT API |
| MyPWM.c | ./MySystem/MyPWM.c | 修改 | 实现新 API；MyPWM_GetFre 改用 Tim_Clock 替代 MySystem_Fre |
| MySystem.c | ./MySystem/MySystem.c | 修改 | 新增 MyPWM_Stepper1/2 实例定义（含时钟频率+中断号） |
| Stepper_PWM.c | ./Hardware/Stepper_PWM.c | 修改 | 完全移植：STM32 HAL→MSPM0 DriverLib + MyPWM API（ARR/Load/IRQ 全部替换） |
| Con_Stepper.c | ./Function/Con_Stepper.c | 修改 | 新增 EN 引脚使能（PA18/PB18 拉低）；PID+限位初始化 |
| Orange.h | ./Hardware/Orange.h | 新增 | 香橙派视觉变量声明（x_real/y_real/x_tar/y_tar） |
| Orange.c | ./Hardware/Orange.c | 新增 | 香橙派视觉变量默认定义 |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | 1ms Tick 新增 Speed_Tick×2+Pos_Tick×2；新增 TIMG6/TIMG7 中断服务 |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增 Stepper_PWM.h/Orange.h/Con_Stepper.h |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All 新增 Stepper_Init() 调用 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 步进电机综合测试例程（6种子模式：瞬时/ramp/定位/三角形/快速/循环） |
| Encoder_Key.c | ./Hardware/Encoder_Key.c | 修改 | EC11 方向判断修复（双向检测：另一相电平决定正反转） |
| empty.syscfg | ./empty.syscfg | 修改 | EC11 引脚新增 interruptPriority="2"（修复中断不触发） |

## 2026-07-22 17:00 | AT24C02 EEPROM STM32→MSPM0 硬件I2C移植 + Mode_2读写测试

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| bsp_at24c02.h | ./AT24/bsp_at24c02.h | 修改 | 移除STM32 SW/HW I2C全部分支，改为MSPM0硬件I2C声明（I2C_1+地址0x50） |
| bsp_at24c02.c | ./AT24/bsp_at24c02.c | 修改 | 删除全部SW I2C位敲打和STM32 HAL代码，使用MyI2C库IIC_WriteBytes/IIC_ReadBytes实现读写 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 清理旧测试代码，新增AT24C02自动递增读写验证测试（OLED显示Wr/Rd/OK/FAIL） |

## 2026-07-23 10:00 | Control任务审查 + Con_Task枚举补全 + Mode_4任务编排 + Stepper拆分为双任务

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Con_Task.h | ./Function/Con_Task.h | 修改 | 枚举补全：TASK_WAIT_TIME/TASK_MOTOR_A_ANGLE/TASK_MOTOR_B_ANGLE/TASK_STEPPER1_ANGLE/TASK_STEPPER2_ANGLE（TASK_COUNT=6） |
| Control.h | ./Function/Control.h | 修改 | 声明补全：修复命名 Task_Motor_Angle→Task_Motor_A_Angle；新增 MotorB/Stepper1/Stepper2 回调声明（共5组） |
| Control.c | ./Function/Control.c | 修改 | TASK_STEPPER_ANGLE 拆分为 TASK_STEPPER1_ANGLE + TASK_STEPPER2_ANGLE（双电机电源隔离）；Setup增加Buzzer_OFF/Stepper_PWM_Stop保护；max_speed/acc可通过p[1]/p[3]配置；IsExit改用单电机判定API |
| Mode_4.c | ./Mode/Mode_4.c | 修改 | Con_Task 集成：任务表注册(6项)+预设演示序列(7步：Wait→MA→Wait→MB→Wait→S1→S2自动串行隔离)+按键动态入队+OLED状态显示+Tick分发 |

## 2026-07-23 16:30 | 新增任务6: 小车MPU旋转固定角度

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyPID.h | ./Software/MyPID.h | 修改 | 新增 PID_Param_Reset 声明（PID历史参数清零，从 Car2 移植） |
| MyPID.c | ./Software/MyPID.c | 修改 | 新增 PID_Param_Reset 实现（LastError/PreError/SumError/realPoint/pout/iout/dout/setPoint 全部清零） |
| Con_Task.h | ./Function/Con_Task.h | 修改 | 枚举新增 TASK_CAR_YAW（小车MPU旋转: p[0]=角度°(+CW/-CCW), p[1]=角度容差, p[2]=角速度容差） |
| Control.h | ./Function/Control.h | 修改 | 新增任务6声明：Task_Car_Yaw_Setup/Tick/IsExit |
| Control.c | ./Function/Control.c | 修改 | 新增任务6实现：static Car_Yaw_PID(PD Kp=6 Kd=20, 参照Car2 Con_MPU) + Setup(yaw归零+PID复位+设目标) + Tick(MPU_Real.yaw→PID计算→差速A-/B+) + IsExit(MPU6050_Turn_Yaw_Is_Ok_Ex双阈值检查→停车) |
| Mode_4.c | ./Mode/Mode_4.c | 修改 | 任务表+OLED显示 新增 TASK_CAR_YAW；演示序列加入注释示例 Con_Task_Enqueue(TASK_CAR_YAW, 180, 0, 0, 0) |

## 2026-07-23 17:00 | PID_Angle 移入 Con_Motor 模块

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Con_Motor.h | ./Function/Con_Motor.h | 修改 | 清理旧注释桩，新增 MPU6050角度环区段：extern PID_Angle + PID_Angle_Init/Reset/Tar_Yaw/Get_Yaw/Tick 声明 |
| Con_Motor.c | ./Function/Con_Motor.c | 修改 | Con_Motor_Init末尾调用PID_Angle_Init()；新增MPU6050角度环实现：全局PID_Angle(PD Kp=6 Kd=20 Out±100) + Reset(yaw归零+PID复位) + Tar_Yaw(设目标) + Get_Yaw(读yaw) + Tick(MPU6050_Angle_Update_Tick→PID→差速A-/B+) |
| Control.c | ./Function/Control.c | 修改 | 任务6精简：删除static Car_Yaw_PID/Car_Yaw_PID_Inited；Setup改为调用PID_Angle_Reset()+PID_Angle_Tar_Yaw()；Tick改为调用PID_Angle_Tick() |

## 2026-07-23 18:00 | ICM42688 驱动移植 + MPU6050 API 补齐

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Imu_Types.h | ./Hardware/Imu_Types.h | 新增 | IMU通用类型定义(ImuOffset_Typedef/ImuCali_Typedef/ImuReal_Typedef)，从F407移植 |
| ICM_42688_base.h | ./Hardware/ICM_42688_base.h | 修改 | 重写为完整驱动头文件：ICM42688_Raw_Data结构体 + 6个公有API(Init/WriteReg/ReadReg/GetData/GetID/Update_Data)，API对齐MPU6050 |
| ICM_42688_base.c | ./Hardware/ICM_42688_base.c | 修改 | 重写为TI DriverLib版驱动：完整寄存器宏+量程/ODR/灵敏度宏+硬件I2C_1读写+12字节批量读取+Init(复位→ACCEL→GYRO→低噪声)+Update_Data(灵敏度转换) |
| MPU6050_base.h | ./Hardware/MPU6050_base.h | 修改 | API补齐：新增WriteReg/ReadReg/GetData/GetID声明，对齐F407版本 |
| MPU6050_base.c | ./Hardware/MPU6050_base.c | 修改 | 新增MPU6050_ReadReg(单字节)/MPU6050_GetData(原始int16批量)/MPU6050_GetID(WHO_AM_I)三个公有函数 |
| MPU6050_Angle.h | ./Function/MPU6050_Angle.h | 修改 | 新增#include "Imu_Types.h"；原有类型定义加#ifndef __IMU_TYPES_H守卫，兼容Imu_Types.h避免重定义 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 改为ICM42688测试模式：Setup(Init+ID校验0x47)+Tick(Update_Data+OLED四轴显示)+Loop(传感器标识) |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增#include "Imu_Types.h" |

## 2026-07-23 18:30 | ICM42688 量程寄存器编码修正

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| ICM_42688_base.c | ./Hardware/ICM_42688_base.c | 修改 | 修正 FS_SEL 编码：ICM42688 与 MPU6050 相反（000=±16g/±2000, 011=±2g/±250）；Init 新增复位前 WHO_AM_I 校验 |

## 2026-07-23 19:00 | 引入 ICM42688 Mahony AHRS 滤波 + Mode2 示例

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| ICM42688_Mahony.h | ./Hardware/ICM42688_Mahony.h | 新增 | Mahony AHRS 头文件：四元数+PI重力修正，无万向节死锁；KP=5.12 KI=0.001；extern GyroBias供AT24C02读写；API: Init/Update_Tick/Calibrate/Yaw_Abs_Get/Reset |
| ICM42688_Mahony.c | ./Hardware/ICM42688_Mahony.c | 新增 | Mahony AHRS 实现（208行纯数学，直接可移植）：陀螺去偏→加速度归一化→四元数投影→叉积误差→PI修正→RK1积分→欧拉角提取→绝对yaw解绕 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | 改为Mahony AHRS测试模式：Setup(Init标定)+Loop(OLED显示R/P/Y 1位小数)+Tick(Update_Tick+Serial1 CSV输出) |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增 #include "ICM42688_Mahony.h" |

## 2026-07-23 19:15 | ICM42688_Mahony 新增转向完成检查函数

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| ICM42688_Mahony.h | ./Hardware/ICM42688_Mahony.h | 修改 | 新增 ICM42688_Turn_Yaw_Is_Ok_Ex / ICM42688_Turn_Yaw_Is_Ok 声明（双阈值 角度+角速度 检查） |
| ICM42688_Mahony.c | ./Hardware/ICM42688_Mahony.c | 修改 | 新增两函数实现：fabsf(targetYaw - ICM_Mahony_Real.yaw) + fabsf(ICM_Raw_Data.GZ - GyroBiasZ) 双条件判断 |

## 2026-07-23 20:00 | I2C 超时保护 + ICM42688 重试机制

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyI2C.h | ./MySystem/MyI2C.h | 修改 | 新增 IIC_WriteBytes_Ex/IIC_ReadBytes_Ex (bool返回)+ IIC_Reset 声明 |
| MyI2C.c | ./MySystem/MyI2C.c | 修改 | 重写硬件I2C封装：_Ex超时版(I2C_TIMEOUT=10ms计数器, 每个while循环带tick递减, 数据流自动续时)+IIC_Reset(DL_I2C_resetControllerTransfer+SYSCFG_DL_init全恢复)+旧接口兼容(失败自动调用Reset) |
| ICM_42688_base.c | ./Hardware/ICM_42688_base.c | 修改 | ICM42688_Update_Data改用_Ex版本+3次重试+每次失败IIC_Reset恢复总线 |

## 2026-07-23 20:15 | I2C 复位次数监控（宏开关）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| MyI2C.h | ./MySystem/MyI2C.h | 修改 | 新增 I2C_DEBUG_RESET_COUNT 宏开关（注释即关）+ extern volatile uint32_t IIC_Reset_Count |
| MyI2C.c | ./MySystem/MyI2C.c | 修改 | IIC_Reset 内 IIC_Reset_Count++ (ifdef 守卫)；新增全局变量定义 |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | Loop 中 OLED 第2行显示 I2C_Rst 计数 (ifdef 守卫，开宏自动显示) |

## 2026-07-24 10:00 | IMU 统一 API 层：陀螺仪底层解耦（IMU.h/c 新建 + 全项目引用替换）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| IMU.h | ./IMU/IMU.h | 新增 | 统一 API 层头文件：内置3种类型定义(ImuOffset/ImuCali/ImuReal)+宏传感器切换(ICM/MPU)+宏映射(IMU_*→ICM*/MPU*)+Turn_Yaw声明 |
| IMU.c | ./IMU/IMU.c | 新增 | 统一 API 层实现：IMU_Turn_Yaw_Is_Ok_Ex/Is_Ok（基于 IMU_Yaw_Abs_Get 绝对累计yaw，纯角度死区判断） |
| Imu_Types.h | ./IMU/Imu_Types.h | 删除 | 类型定义已合并至 IMU.h，不再需要独立 Types 文件 |
| ICM42688_Mahony.h | ./IMU/ICM42688_Mahony.h | 修改 | #include Imu_Types.h → IMU.h；移除 ICM42688_Turn_Yaw_Is_Ok_Ex/Is_Ok 声明（移至 IMU.h） |
| ICM42688_Mahony.c | ./IMU/ICM42688_Mahony.c | 修改 | 移除 ICM42688_Turn_Yaw_Is_Ok_Ex/Is_Ok 实现（移至 IMU.c） |
| AllHeader.h | ./App/AllHeader.h | 修改 | 3行ICM/#include → 1行 #include "IMU.h" |
| AllHeader.c | ./App/AllHeader.c | 修改 | ICM42688_Mahony_Init(0) → IMU_Mahony_Init(0) |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | ICM42688_Mahony_Update_Tick() → IMU_Mahony_Update_Tick() |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | ICM_Mahony_Real.* → IMU_Mahony_Real.*; ICM_Yaw_Abs_Get() → IMU_Yaw_Abs_Get() |
| Mode_5.c | ./Mode/Mode_5.c | 修改 | ICM_Yaw_Abs_Get() → IMU_Yaw_Abs_Get() |
| Control.c | ./Function/Control.c | 修改 | ICM42688_Turn_Yaw_Is_Ok_Ex(3参) → IMU_Turn_Yaw_Is_Ok_Ex(2参，移除gyro_tol)；删除未使用的gyro_tol变量 |
| Con_Motor.c | ./Function/Con_Motor.c | 修改 | #include ICM42688_Mahony.h → IMU.h; ICM_Yaw_Abs_Get() → IMU_Yaw_Abs_Get() |
| Con_Motor.h | ./Function/Con_Motor.h | 修改 | 注释"MPU6050角度环" → "IMU角度环" |

> **核心成果**：业务代码全部改用 `IMU_*` 统一 API，不再直接依赖 ICM42688 符号。切换传感器只需修改 `IMU.h` 中一行宏（`#define IMU_USE_MPU6050`），上层零改动。

## 2026-07-24 15:00 | 参考Car1重构Serial_porting：错误中断处理 + 状态机改进

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Function/Serial_porting.h | 修改 | 新增Serial_RX_FLAG_Typedef枚举(RX_OK/RX_BUSY/RX_WAIT/RX_Error)；Struct新增rx_temp+Status字段替换旧rxState；新增Serial_Rx_State_Check()声明 |
| Serial_porting.c | ./Function/Serial_porting.c | 修改 | ★借鉴Car1重构：Status 0/1/2状态机(Serial_Rx_State_Check)；Serial_Data_Check_HEX/ABC分离(借鉴Car1)；ISR改为rx_temp→State_Check→Data_Check模式；★新增DL_UART_MAIN_IIDX_RX_ERROR处理(防止噪声FIFO锁死)；保留所有原有API(Serial_Check_Str/CheckCmd/超时机制) |

> **核心改进**：参考Car1项目的`System/Serial.c`，引入rx_temp字节暂存+Status状态机+错误中断处理三重保护，解决噪声环境下的潜在FIFO锁死问题。原有API完全兼容。

## 2026-07-24 15:10 | 新增Serial3 (UART_2, PB15/PB16)

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Serial_porting.h | ./Function/Serial_porting.h | 修改 | 新增 #define Serial3_Enable 1 + extern Serial_Typedef Serial3 |
| Serial_porting.c | ./Function/Serial_porting.c | 修改 | 新增Serial3实例+初始化+UART_2_INST_IRQHandler(UART2 PB15/PB16,115200) |
| Mode_4.c | ./Mode/Mode_4.c | 修改 | Serial2→Serial3全替换；OLED标签S1/S3；修复%len的%sd%d格式符

## 2026-07-24 13:42 | Mode_3/5/6 合并到 Mode_3，用 MODE3_SELECT 宏切换功能

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Mode_3.h | ./Mode/Mode_3.h | 修改 | 新增 #define MODE3_SELECT 功能选择宏(1=电机PID/2=陀螺仪角度环/3=步进电机) |
| Mode_3.c | ./Mode/Mode_3.c | 修改 | ★合并Mode_3/5/6：三个子功能用 #if MODE3_SELECT 隔离(各自独立_Impl函数)，公共接口委托分发；#else 触发 #error 编译报错 |
| Mode_G.h | ./Mode/Mode_G.h | 修改 | 枚举移除 Mode_5/Mode_6(已合并到Mode_3)；Mode_3注释更新为多功能模式 |
| Mode_G.c | ./Mode/Mode_G.c | 修改 | Timer_20ms移除 Mode_5_Tick/Mode_6_Tick；默认next_mode改为Mode_3 |
| empty.c | ./empty.c | 修改 | 三个switch(Loop/Exit/Setup)移除所有 Mode_5/Mode_6 case |
| AllHeader.h | ./App/AllHeader.h | 修改 | 移除 #include "Mode_5.h" 和 "Mode_6.h" |

> **使用方式**：修改 `Mode_3.h` 中 `MODE3_SELECT` 值为 1/2/3 切换功能，重新编译即可。旧 Mode_5.h/c、Mode_6.h/c 文件保留（不再参与编译），可手动删除。

## 2026-07-24 13:42 | Mode_4任务表迁移到Control.c + Orange寻迹任务化

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Orange.c | ./Hardware/Orange.c | 修改 | ★Bug修复: PID_Update(&PID_Oran_Y, ...) 第二个参数从 PID_Oran_X.realPoint_Now 改为 PID_Oran_Y.realPoint_Now |
| Con_Task.h | ./Function/Con_Task.h | 修改 | 枚举新增 TASK_ORAN_TRACK（香橙派寻迹追踪） |
| Control.h | ./Function/Control.h | 修改 | 新增 extern Control_TaskTable[TASK_COUNT] 全局共享任务表声明；新增 Task_Oran_Track_Setup/Tick/IsExit 声明 |
| Control.c | ./Function/Control.c | 修改 | ★新增全局共享任务表 Control_TaskTable（含全部7个任务注册）；新增 TASK_ORAN_TRACK 实现（Setup设置PID目标+Tick调用Oran_XY_PID_Update+IsExit容差200ms稳态/超时检测） |
| Mode_4.c | ./Mode/Mode_4.c | 修改 | 删除本地 Mode4_TaskTable；改为引用全局 Control_TaskTable；OLED新增 TASK_ORAN_TRACK 标签 |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All() 新增 Oran_XY_Init()（在 Param_AT24C02_Init 之前调用，确保默认PID初始化后被EEPROM覆盖） |

> **架构改进**：任务表从 Mode_4 本地提取为 Control.c 全局共享，后续所有 Con_Mode 统一调用 `Con_Task_Init(Control_TaskTable, TASK_COUNT)`。新增任务只需在 Con_Task.h 枚举 + Control.h 声明 + Control.c 表注册 + Control.c 实现。Orange寻迹任务支持：p[0]=goal_x, p[1]=goal_y, p[2]=容差(默认10), p[3]=超时ms(0=不限)，200ms稳态防抖退出。

## 2026-07-24 14:15 | 新增Con_Task_Skip强制跳过当前任务

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Con_Task.h | ./Function/Con_Task.h | 修改 | 新增 Con_Task_Skip() 声明（强制完成当前任务，跳过IsExit判断，自动进入下一个任务） |
| Con_Task.c | ./Function/Con_Task.c | 修改 | 提取 Con_Task_RecordComplete() 静态辅助函数（消除Loop/Skip重复代码）；新增 Con_Task_Skip() 实现（无任务时直接返回，有任务时记录耗时+重置状态机） |

> **新增API**: `Con_Task_Skip(void)` — 当IsExit条件无法覆盖所有硬件异常场景（电机堵转、传感器故障、通信超时等）时，调用方可在按键/串口/超时检测中调用此函数跳过当前卡死的任务。与 Con_Task_Clear() 的区别：Clear 清空队列+终止当前任务（全部放弃），Skip 仅跳过当前任务、保留队列后续任务继续执行。任务记录中标记为 "Skip" 以便调试区分。

## 2026-07-24 15:00 | 实现整车直行位置环 TASK_CAR_STRAIGHT（双电机+IMU偏航修正）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Motor.h | ./Hardware/Motor.h | 修改 | Motor_Param_Typedef 新增 Wheel_Cm 字段；取消注释 Motor_Pos_Update 声明 |
| Motor.c | ./Hardware/Motor.c | 修改 | 实现 Motor_Pos_Update()：编码器 total_cnt→距离(cm)，公式 total_cnt×Wheel_Cm/(time_Fre×PPR×ReductionRatio) |
| Con_Motor.h | ./Function/Con_Motor.h | 修改 | 取消注释 Pos API（Motor_SetPos/Get_Pos/Is_Pos/Clear）；新增 Motorx_Pos_Update_Tick 声明；新增 Car Straight 控制器声明（PID_Car_Straight + Init/Reset/Tick） |
| Con_Motor.c | ./Function/Con_Motor.c | 修改 | ★核心实现：①Motor_Param 增加 Wheel_Cm=20.0；②PID_Pos 初始化（Kp=4,Kd=3,Out±200）；③实现全部 Pos API + Motorx_Pos_Update_Tick；④新增 PID_Car_Straight 整车直行控制器（双编码器平均距离→位置PID→IMU偏航P修正→差速输出）；⑤Con_Motor_Init 中调用 PID_Car_Straight_Init |
| Con_Task.h | ./Function/Con_Task.h | 修改 | 枚举新增 TASK_CAR_STRAIGHT（p[0]=目标cm≤0=永不停, p[1]=容差cm, p[2]=max_speed） |
| Control.h | ./Function/Control.h | 修改 | 新增 Task_Car_Straight_Setup/Tick/IsExit 声明 |
| Control.c | ./Function/Control.c | 修改 | Control_TaskTable 新增 TASK_CAR_STRAIGHT 注册；实现3个回调（Setup清零编码器+yaw基准、Tick调 PID_Car_Straight_Tick、IsExit: p[0]≤0永假/双电机到位检查） |
| Mode_3.c | ./Mode/Mode_3.c | 修改 | MODE3_SELECT==1 改造：Motor_Pos_Enable(bool)→Motor_Loop_Mode(uint8_t, 0=Speed/1=Angle/2=Pos)；KEY_2 循环3态切换；Pos模式调PID_Pos+Motor_SetPos；Tick调用Motorx_Pos_Update_Tick；OLED/Telemetry 适配3模式 |
| Mode_4.c | ./Mode/Mode_4.c | 修改 | OLED switch 新增 TASK_CAR_STRAIGHT 标签 ">>> Car Straight" |

> **架构**: 位置环分两层——①单电机层 Motorx_Pos_Update_Tick（同Angle模式，供Mode_3调参）；②整车层 PID_Car_Straight_Tick（双编码器平均+IMU yaw P修正，Kp=5.0）。TASK_CAR_STRAIGHT 的特殊语义：p[0]≤0 时 IsExit 永远返回 false，需外部 Con_Task_Skip() 强制退出（用于"一直直走"场景）。

## 2026-07-25 | 实现TJC_LCD串口屏调参库 + 修复Serial4 ISR

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| TJC_LCD.h | ./Tools/TJC_LCD.h | 修改 | 填充API声明：LCD_KEY_1~6宏、LCD_Param_Set/Pressed函数 |
| TJC_LCD.c | ./Tools/TJC_LCD.c | 修改 | 核心实现：Serial4 ABC协议解析、按键/滑块状态管理、0-100→[min,max]映射 |
| AllHeader.h | ./App/AllHeader.h | 修改 | tools层新增 #include "TJC_LCD.h" |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All() tools段新增 TJC_LCD_Init() |
| Mode_2.c | ./Mode/Mode_2.c | 修改 | LCD演示：4个虚拟按键(Kp±/Kd±) + 2个滑块(Kp 0-100整数 / Ki 0.0-2.0浮点) |
| Serial_porting.c | ./Function/Serial_porting.c | 修改 | ★修复Serial4 ISR函数名 UART_4→UART_3（解决Default_Handler死循环） |
| Serial_porting.h | ./Function/Serial_porting.h | 修改 | 注释修正 UART_4→UART_3 |

## 2026-07-26 | TJC_LCD 极简重写（无Init/Process，自包含函数+flag恢复模式）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| TJC_LCD.h | ./Tools/TJC_LCD.h | 修改 | 重写：移除Init/Process/Key_Pressed/Param_Set，新增LCD_Key_Check/Set_Int/Float/Cmd_Check，LCD_PARAM_X宏 |
| TJC_LCD.c | ./Tools/TJC_LCD.c | 修改 | 重写：无状态设计，每函数独立获取/恢复flag，strstr+sscanf解析，内置0-100→[min,max]映射 |
| AllHeader.c | ./App/AllHeader.c | 修改 | 移除 TJC_LCD_Init() 调用（无Init设计不需要） |
| Mode_5.c | ./Mode/Mode_5.c | 修改 | TJC_LCD实验：4按键(Kp±/Kd±)+2滑块(整数+浮点映射)+通用指令Reset |

## 2026-07-26 15:00 | Mode_6 物品搜索→定位→夹取状态机

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Mode_6.c | ./Mode/Mode_6.c | 修改 | ★实现5状态机：Init(等按键)→No_First(自转搜索:90°→360°CCW, Oran_Item[0]>0即中断)→No_Item(停车等待)→Find_Item(TASK_ORAN_TRACK逼近)→Get_Item(Elec_ON+等500ms)→Over；新增Search_Phase_Typedef子阶段枚举+Con_Task_Init+Exit电机/电磁铁清理 |

## 2026-07-26 15:45 | Mode_2 找寻终点（方案A风格）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Mode_2.c | ./Mode/Mode_2.c | 修改 | ★方案A风格实现找寻终点：3函数API(Find_Home_Start/Run/Tick)，内部直接操作PID_Angle_*/Oran_XY_PID_Update不依赖Con_Task；状态机Init→Rotate45(相对旋转45°+发@Find_Home:1$#)→Tracking(x/y对齐±10+稳定200ms)→Over；预留后续封装为TASK_FIND_HOME的接口 |

## 2026-07-26 17:00 | Mode_3 统一调参菜单（Menu_Param 任务队列框架）

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.h | ./Function/Menu_Param.h | 重写 | 任务队列菜单框架API：TuneTaskID枚举（12项含哨兵）+TuneTaskDef描述结构体（category/name/type/pid/stp/回调）+Menu_Tune_Init/Loop/Tick/GetActive |
| Menu_Param.c | ./Function/Menu_Param.c | 重写 | ★核心实现（~340行）：参照Con_Task模式，s_tune_table[]静态任务表（11个调参目标）、BROWSE/ACTIVE双状态机、KEY_1/KEY_2导航（分类跳转+子项切换）、Serial1 ABC协议路由（PID: Kp/Ki/Kd/Goal, Stepper: Speed/Pos/Stop）、PID指针判断自动Tick分发（Motorx_Angle/Pos_Update_Tick, PID_Car_Straight/PID_Angle_Tick）、IMU校准回调 |
| Mode_3.c | ./Mode/Mode_3.c | 重写 | 替换MODE3_SELECT全部旧代码（~300行→25行薄封装）：Setup→Menu_Tune_Init, Loop→Menu_Tune_Loop, Tick→Menu_Tune_Tick, Exit→Motor_Stop+OLED_Clear |
| AllHeader.h | ./App/AllHeader.h | 修改 | Function层新增 #include "Menu_Param.h" |
| Debug/makefile | ./Debug/makefile | 修改 | ORDERED_OBJS新增 ./Function/Menu_Param.o |
| Debug/Function/subdir_vars.mk | ./Debug/Function/subdir_vars.mk | 修改 | 6个列表(C_SRCS/C_DEPS/OBJS+QUOTED变体)均新增 Menu_Param 条目 |

> **架构**：参照 Con_Task 的枚举+描述表模式，Menu_Param 提供统一的调参任务队列框架。新增调参目标只需两步：①TuneTaskID枚举加ID ②s_tune_table[]数组加行，无需改动框架代码。任务类型支持PID（OLED显示Kp/Ki/Kd/Goal/Real/Set+Serial1调参+自动电机控制）、ACTION（一次性回调）、STEPPER（开环Speed/Pos/Stop）。导航：KEY_1分类跳转/KEY_2子项切换/KEY_1_LONG激活-退出。Mode_1保持不变（IMU偏置+ParamEdit）。

## 2026-07-26 14:00 | 所有PID Tick补全Serial1调试输出

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.h | ./Function/Menu_Param.h | 修改 | 新增 Tune_MotorB_Speed_Tick 声明（补全 Motor_B Speed Tick） |
| Menu_Param.c | ./Function/Menu_Param.c | 修改 | 7个PID Tick全部加入Serial_printf输出(goalPoint/realPoint_Now/setPoint)；新建Tune_MotorB_Speed_Tick；头注释更新；Menu_Tune_Table[3] NULL→Tune_MotorB_Speed_Tick |

> **改动范围**：仅涉及8个PID调参任务的20ms Tick（Motor_A/B Speed/Angle/Pos + Car_Straight + Gyro_Yaw），Gyro_Cal/Stepper1/2的NULL Tick保持不变。导航逻辑、按键映射、菜单渲染全部未改。

## 2026-07-26 14:10 | Gyro_Cal/Stepper 补全 Tick 调试输出

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.h | ./Function/Menu_Param.h | 修改 | 新增 Tune_Gyro_Cal_Tick / Tune_Stepper_S1_Tick / Tune_Stepper_S2_Tick 声明 |
| Menu_Param.c | ./Function/Menu_Param.c | 修改 | 新增3个Tick：Gyro_Cal输出biasX/Y/Z(%.4f)，Stepper1/2输出Pos_Tar/Pos_Now/Speed_Now；Menu_Tune_Table三处NULL→新Tick；头注释更新 |

> **当前状态**：11个调参任务全部有Tick输出，无NULL Tick。PID任务输出goal/real/set，GyroCal输出三轴偏置，Stepper输出位置+速度。

## 2026-07-26 16:00 | 新增舵机驱动库 + Mode_5循环演示

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Servo.h | ./Hardware/Servo.h | 新增 | 舵机驱动头文件：Servo_Typedef结构体（PWM+角度/脉宽范围）、Init/SetAngle/SetPulse_us/GetAngle API |
| Servo.c | ./Hardware/Servo.c | 新增 | 舵机驱动实现：Init中SetLoadValue(20000→50Hz)+归中，SetAngle线性映射角度→脉宽→Compare，双层限幅（MyPWM层+Servo层） |
| MySystem.c | ./MySystem/MySystem.c | 修改 | 新增 MyPWM_Servo1(CCP0/PA29) + MyPWM_Servo2(CCP1/PA2) 实例，Compare_Max=2500/Min=500 |
| MyPWM.h | ./MySystem/MyPWM.h | 修改 | 新增 MyPWM_Servo1/2 extern 声明 |
| AllHeader.h | ./App/AllHeader.h | 修改 | 新增 #include "Servo.h" |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All() 中新增 Servo_Init x2（0~180°/500~2500us） |
| Mode_5.c | ./Mode/Mode_5.c | 修改 | 舵机循环演示：双舵机同步0°→180°→0°，步进1°/tick(20ms)=50°/s，Exit时归中90° |

> **SysConfig配置**：用户已将 PWM4(PWM_Servo/TIMG8) 的 clockDivider=8、prescale=3（总分频×32→1MHz）。PWM周期由C代码 MyPWM_SetLoadValue(20000) 运行时设置（50Hz）。每个计数=1µs，pulse_us值直接等于Compare值。

## 2026-07-27 15:30 | 移植Y8_Driver巡线模块并在Mode_5编写OLED演示

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Y8_Driver.h | ./Hardware/Y8_Driver.h | 新增 | Y8 8路巡线传感器驱动头文件（从F407移植），声明Y8_Data[8]/Y8_Width[8]/Y8_Bias/PID_Track和4个API函数 |
| Y8_Driver.c | ./Hardware/Y8_Driver.c | 新增 | Y8驱动实现（从F407移植）：硬件移位寄存器协议(CLK+DAT)、多采样投票滤波(P>50%)、加权位置→atan2角度、5窗口滑动中值滤波、PID巡线一体化。适配MSPM0: 延时us*4(@32MHz)、GPIO名MyGPIO_Y8_CLK/DAT |
| AllHeader.h | ./App/AllHeader.h | 修改 | Hardware层新增 #include "Y8_Driver.h" |
| AllHeader.c | ./App/AllHeader.c | 修改 | Initial_All() 中新增 Y8_Init()（硬件+巡线PID初始化） |
| Mode_5.c | ./Mode/Mode_5.c | 修改 | 实现Y8巡线OLED演示：每20ms Tick读取传感器→5次采样滤波→显示8路状态方块(实心=黑线)/角度偏移/原始字节/黑线计数，Key0双击切模式 |
| subdir_vars.mk | ./Debug/Hardware/subdir_vars.mk | 修改 | 注册 Y8_Driver.c 到构建系统（C_SRCS/C_DEPS/OBJS 及其 __QUOTED 变体） |
| makefile | ./Debug/makefile | 修改 | ORDERED_OBJS 新增 ./Hardware/Y8_Driver.o，clean 新增对应 .o/.d 清理 |

## 2026-07-27 16:00 | Mode_5 Y8巡线功能集成到Menu_Param调参菜单

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.h | ./Function/Menu_Param.h | 修改 | 新增 TUNE_Y8_TRACK 枚举（TUNE_COUNT之前）+ Setup/Run/Tick 回调声明 |
| Menu_Param.c | ./Function/Menu_Param.c | 修改 | 新增 TUNE_Y8_TRACK 完整实现：Setup清零PID+设定goal=0，Run用Serial1 ABC协议调Kp/Ki/Kd+OLED_ShowPID显示6行，Tick调用Y8_PID_Update+CSV输出；s_labels加{"Y8","Track"}，任务表加表项；文件头加#include "Y8_Driver.h" |

## 2026-07-27 16:20 | TUNE_Y8_TRACK增加KEY_2切换展示模式

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.c | ./Function/Menu_Param.c | 修改 | TUNE_Y8_TRACK新增KEY_2单击切换双模：s_y8_mode=0→展示模式(OLED 8路二进制+角度+停电机)，s_y8_mode=1→巡线模式(Serial1调参+PID显示+电机差速)；Tick同步分流(展示=Y8_Data_Update只读，巡线=Y8_PID_Update全控) |

## 2026-07-27 17:00 | Mode_4 香橙派参数调节移植到 Menu_Param 作为 TUNE_ORANGE_PARAM 任务

| 文件名 | 文件路径（相对工作区） | 操作类型 | 说明 |
|--------|----------------------|----------|------|
| Menu_Param.h | ./Function/Menu_Param.h | 修改 | 新增 TUNE_ORANGE_PARAM 枚举（索引12）+ Tune_Orange_Setup/Run/Tick 回调声明 |
| Menu_Param.c | ./Function/Menu_Param.c | 修改 | 新增 Tune_Orange_Setup（Param_Init+注册6个Oran_Param+@start:6$#请求），Tune_Orange_Run（Param_Loop提供EC11编辑体验+非编辑态OLED显示6参数+KEY_2请求数据），Tune_Orange_Tick（20ms Serial1 CSV输出），s_labels/Menu_Tune_Table 分别追加行 |
