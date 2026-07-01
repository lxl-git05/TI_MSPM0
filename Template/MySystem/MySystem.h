#ifndef __MYSYSTEM_H
#define __MYSYSTEM_H

/* 系统相关头文件,后续移植到别的芯片只需要修改本部分 */

// 1. TI MSPM0 DriverLib 与基础类型
#include <stdint.h>
#include "ti_msp_dl_config.h"

// 2. 本地底层通用
#include "MyGPIO.h"
#include "MyPWM.h"
#include "MyEncoder.h"
#include "MyTimer.h"

// 3. 芯片参数
#define MySystem_Fre CPUCLK_FREQ

#endif // !__MYSYSTEM_H
