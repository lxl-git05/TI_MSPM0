#ifndef __DELAY_H
#define __DELAY_H

#include "ti_msp_dl_config.h"

void Delay_Global_Tick(void) ;

void Delay_ms(uint32_t delay_ms) ;

uint32_t Get_Time_ms(void) ;

#endif
