[toc]

# 1. 引脚配置

## 1-1 板载引脚

| 引脚号 | 标签号(模糊标签) | 备注    |
| ------ | ---------------- | ------- |
| PB   22   | LED_PIN_0        | 板载LED |
| PB   21   | KEY_0            | 板载KEY |



## 1-2 普通GPIO

| 引脚号 | 标签号(模糊标签) | 备注                     |
| ------ | ---------------- | ------------------------ |
| PA31   | OLED_SCL         | 其实也是IIC,但是是软驱动 |
| PA28   | OLED_SDA         |                          |
| ×      | KEY_0            | 在板载引脚部分           |
| PA15   | KEY_1            |                          |
| PA17   | KEY_2            |                          |
|        |                  |                          |
| PB   6    | **LED_R**        | 红灯                     |
| PB   7    | **LED_G**        | 绿灯                     |
| PB   8    | **LED_B**        | 蓝灯(实际接了黄色LED)    |
| ==PB   9== | **LED_Time**     | 时间检测GPIO口           |
| PA 13 | TCRT | 红外检测-新增 |





## 1-3 电机控制配置

| 引脚号 | 标签号(模糊标签) | 备注         |
| ------ | ---------------- | ------------ |
| PB   4    | A_IN_1           | PWM          |
| PB   12   | A_IN_2           | 普通驱动     |
| PB   14   | A_Encoder_1      | 外部中断驱动(双边沿)     |
| PB   1    | A_Encoder_2      | 双边沿GPIO输入(无独立中断) |
|        |                  |              |
| PB   5    | B_IN_1           | PWM          |
| PB   13   | B_IN_2           | 普通驱动     |
| PB   11   | B_Encoder_1      | 外部中断驱动(双边沿)     |
| PB   10   | B_Encoder_2      | 双边沿GPIO输入(无独立中断) |



## 1-4 UART配置

| 引脚号 | 标签号(模糊标签) | 备注                                  |
| ------ | ---------------- | ------------------------------------- |
| PA10   | UART_0_TX        | USB可读写,作为调试端口,**引脚不可变** |
| PA11   | UART_0_RX        |                                       |
|        |                  |                                       |
| PA8    | UART_1_TX        | 与树莓派进行通信                      |
| PA9    | UART_1_RX        |                                       |
|        |                  |                                       |
| PB   15   | UART_2_TX        | 蓝牙                  |
| PB   16   | UART_2_RX        |                                       |



## 1-5 IIC配置

| 引脚号 | 标签号(模糊标签) | 备注  |
| ------ | ---------------- | ----- |
| PA1    | Y8_SCL           | IIC_0 |
| PA0    | Y8_SDA           |       |
|        |                  |       |
| PB   2    | MPU6050_SCL      | IIC_1 |
| PB   3    | MPU6050_SDA      |       |



## 1-6 PWM配置

| 定时器 | 通道 | 引脚   | 时钟频率 | 模式         | 周期(ARR) | 约PWM频率 | 用途       |
| ------ | ---- | ------ | -------- | ------------ | --------- | --------- | ---------- |
| TIMA1  | CCP0 | PB4    | 4MHz     | 边沿对齐递增 | 1000      | 约4kHz    | 电机A PWM  |
| TIMA1  | CCP1 | PB5    | 4MHz     | 边沿对齐递增 | 1000      | 约4kHz    | 电机B PWM  |

> 时钟分频器 = 8，BUSCLK=32MHz → 定时器时钟 = 4MHz，PWM频率 = 4MHz / 1000 ≈ 4kHz



## 1-7 定时器中断配置

| 定时器实例 | SysConfig名称 | 时钟源               | 预分频         | 周期  | 中断类型 | 装入值(Load) | 用途                                   |
| ---------- | ------------- | -------------------- | -------------- | ----- | -------- | ------------ | -------------------------------------- |
| TIMG0      | TIMER_0       | BUSCLK/1 (32MHz)     | —              | 1ms   | ZERO     | 31999        | 按键扫描、LED闪烁、全局延时            |
| TIMA0      | TIMER_1       | BUSCLK/8/100 (40kHz) | div=8, pre=100 | 20ms  | ZERO     | 799          | 电机PID更新、MPU6050姿态、模式tick     |

> TIMER_0 使用 TIMG0 (通用定时器)，TIMER_1 使用 TIMA0 (高级定时器)。两者均为周期性模式，由 ZERO 事件触发中断。



## 1-8 DMA配置

| DMA通道 | 外设    | 方向           | 触发源             | 传输模式     | 数据宽度 | 说明                  |
| -------- | ------- | -------------- | ------------------ | ------------ | -------- | --------------------- |
| CH0      | UART0   | 内存→外设 (b2f) | DMA_UART0_TX_TRIG  | 单次传输     | Byte     | UART_0 调试串口发送    |
| CH1      | UART0   | 外设→内存 (f2b) | DMA_UART0_RX_TRIG  | 全通道循环   | Byte     | UART_0 调试串口接收    |

> UART_0 使用 DMA 进行收发，UART_1 和 UART_2 使用 RX 中断方式接收。



## 1-9 调试接口与时钟

| 项目         | 配置                                        |
| ------------ | ------------------------------------------- |
| 调试接口     | SWD (PA19 SWDIO / PA20 SWCLK)               |
| 调试探针     | SEGGER J-Link                               |
| CPU时钟      | SYSOSC 32MHz                                |
| PLL          | 禁用                                        |
| HFXT         | 禁用                                        |
| targetConfig | `targetConfigs/MSPM0G3507.ccxml`            |

> 使用默认时钟树 (`SYSCTL.forceDefaultClkConfig = true`)，PLL 和外部高速晶振均未启用。



# 2. 开发环境与工具链

| 组件       | 版本/路径                                                                            |
| ---------- | ------------------------------------------------------------------------------------ |
| IDE        | CCS Theia v70.4.0                                                                    |
| 编译器     | TI ARM Clang (TICLANG) 4.0.4.LTS                                                     |
| SDK        | mspm0_sdk 2.09.00.01                                                                 |
| SysConfig  | 1.26.0+4407                                                                          |
| 烧录工具   | DSLite (UniFlash) + SEGGER J-Link                                                    |
| 烧录命令   | `dslite -c targetConfigs/MSPM0G3507.ccxml -e -r 2 -u Debug/Template.out`             |
| 编译命令   | `gmake -C Debug clean all`                                                           |
| SysConfig CLI | `sysconfig_cli.bat -s product.json --script empty.syscfg -o "." --compiler ticlang` |



## 编译到烧录完整流程

```
empty.syscfg (SysConfig脚本)
    ↓ SysConfig CLI生成
Debug/ti_msp_dl_config.c + ti_msp_dl_config.h
    ↓
TI ARM Clang 编译所有源文件
(App/ Function/ Hardware/ Mode/ System/ Tools/ MySystem/ *.c)
    ↓
TI ARM Linker (device_linker.cmd)
  Flash: 128KB @ 0x00000000
  SRAM:  32KB  @ 0x20200000
    ↓
Debug/Template.out (ELF)
    ↓ DSLite + J-Link + System Reset
MSPM0G3507 芯片运行
```



## Tianmengxing 开发板特殊引脚提醒

| 引脚            | 说明                                                         |
| --------------- | ------------------------------------------------------------ |
| PA21/PA23/PA2/PA18 | **避免使用** — 这些引脚在 Tianmengxing 板上有特殊用途       |
| PA10/PA11       | 天曚星默认 UART 引脚，本工程用作 UART_0 调试串口，**不可变更** |
| PA13            | 本工程用作 TCRT 红外循迹 GPIO 输入（SWD 使用 PA19/PA20，故 PA13 可安全复用） |



