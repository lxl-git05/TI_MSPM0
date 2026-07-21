#ifndef __MYSYSTEM_H
#define __MYSYSTEM_H

/* ====================================================================
 * MySystem.h — 芯片底层抽象层
 *
 * 【跨芯片移植指南】只需修改以下两处：
 * 1. 芯片底层头文件（ti_msp_dl_config.h 等）
 *    替换为目标芯片对应的头文件
 * 2. MySystem_Fre 改为目标芯片主频（单位：Hz）
 *
 * 其他文件（Hardware / Function / Mode）无需修改
 * ==================================================================== */

// -------------------- 芯片主频（Hz）【移植时修改这里】--------------------
#define MySystem_Fre 32000000  // MSPM0G3507: 32MHz

// -------------------- 芯片底层头文件 --------------------
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include "ti_msp_dl_config.h"   // SysConfig 生成的引脚宏（GPIO_LED_PORT 等）

// -------------------- C标准库 --------------------
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// -------------------- 抽象层头文件 --------------------
#include "MyGPIO.h"
#include "MyPWM.h"
#include "MyEncoder.h"
#include "MyTimer.h"

#endif // !__MYSYSTEM_H
