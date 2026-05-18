#ifndef __MODE_CON_1_H
#define __MODE_CON_1_H

#include "ti_msp_dl_config.h"

#include "AllHeader.h"

void Mode_Con_1_Setup(void) ;

void Mode_Con_1_Loop(void) ;

void Mode_Con_1_Exit(void) ;

void Mode_Con_1_Tick(void) ;

// 控制台记录状态逻辑
void Car_Status_Store(void) ;

// 小车状态转换台
void Car_Control_Change(void) ;

#endif


